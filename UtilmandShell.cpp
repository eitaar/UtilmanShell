#include <windows.h>
#include <wtsapi32.h>
#include <userenv.h>
#include <tlhelp32.h>
#include <stdio.h>

#pragma comment(lib, "wtsapi32.lib")
#pragma comment(lib, "userenv.lib")
#pragma comment(lib, "advapi32.lib")

void Log(const char* msg, DWORD err = 0) {
    FILE* f = fopen("C:\\Windows\\Temp\\utilman_log.txt", "a");
    if (f) {
        if (err) fprintf(f, "%s (err=%lu)\n", msg, err);
        else fprintf(f, "%s\n", msg);
        fclose(f);
    }
}

// Get the PID of winlogon.exe for the specified session
DWORD GetWinlogonPid(DWORD sessionId) {
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnap == INVALID_HANDLE_VALUE) return 0;

    PROCESSENTRY32W pe = { sizeof(pe) };
    DWORD foundPid = 0;

    if (Process32FirstW(hSnap, &pe)) {
        do {
            if (_wcsicmp(pe.szExeFile, L"winlogon.exe") == 0) {
                DWORD pidSession = 0;
                if (ProcessIdToSessionId(pe.th32ProcessID, &pidSession) &&
                    pidSession == sessionId) {
                    foundPid = pe.th32ProcessID;
                    break;
                }
            }
        } while (Process32NextW(hSnap, &pe));
    }

    CloseHandle(hSnap);
    return foundPid;
}

int main() {
    Log("=== Start ===");

    // 1. Get the active session ID
    DWORD sessionId = WTSGetActiveConsoleSessionId();
    char buf[128];
    sprintf(buf, "Session ID: %lu", sessionId);
    Log(buf);

    if (sessionId == 0xFFFFFFFF || sessionId == 0) {
        Log("No valid active session");
        return 1;
    }

    // 2. Get the PID of winlogon.exe
    DWORD winlogonPid = GetWinlogonPid(sessionId);
    sprintf(buf, "Winlogon PID: %lu", winlogonPid);
    Log(buf);

    if (winlogonPid == 0) {
        Log("Winlogon not found");
        return 2;
    }

    // 3. Open the winlogon process
    HANDLE hWinlogon = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, winlogonPid);
    if (!hWinlogon) {
        Log("OpenProcess winlogon failed", GetLastError());
        return 3;
    }

    // 4. Get the winlogon token
    HANDLE hWinlogonToken = NULL;
    if (!OpenProcessToken(hWinlogon, TOKEN_DUPLICATE, &hWinlogonToken)) {
        Log("OpenProcessToken winlogon failed", GetLastError());
        CloseHandle(hWinlogon);
        return 4;
    }
    CloseHandle(hWinlogon);
    Log("Got winlogon token");

    // 5. Duplicate the token
    HANDLE hDupToken = NULL;
    if (!DuplicateTokenEx(hWinlogonToken, MAXIMUM_ALLOWED, NULL,
                          SecurityImpersonation, TokenPrimary, &hDupToken)) {
        Log("DuplicateTokenEx failed", GetLastError());
        CloseHandle(hWinlogonToken);
        return 5;
    }
    CloseHandle(hWinlogonToken);
    Log("Token duplicated");

    // 6. Create the environment block
    LPVOID pEnv = NULL;
    if (!CreateEnvironmentBlock(&pEnv, hDupToken, FALSE)) {
        Log("CreateEnvironmentBlock failed", GetLastError());
        CloseHandle(hDupToken);
        return 6;
    }
    Log("Environment block OK");

    // 7. Configure STARTUPINFO (no handle inheritance)
    STARTUPINFOW si = { sizeof(si) };
    si.lpDesktop = (LPWSTR)L"winsta0\\default";
    si.wShowWindow = SW_SHOWNORMAL;
    si.dwFlags = STARTF_USESHOWWINDOW;

    PROCESS_INFORMATION pi = { 0 };
    wchar_t cmdLine[] = L"cmd.exe";

    // 8. Launch the process (with BREAKAWAY_FROM_JOB)
    BOOL result = CreateProcessAsUserW(
        hDupToken,
        NULL,
        cmdLine,
        NULL,
        NULL,
        FALSE,  // do not inherit handles
        CREATE_UNICODE_ENVIRONMENT | CREATE_NEW_CONSOLE | CREATE_BREAKAWAY_FROM_JOB,
        pEnv,
        NULL,
        &si,
        &pi
    );

    if (!result) {
        DWORD err = GetLastError();
        Log("CreateProcessAsUser failed", err);

        // If BREAKAWAY_FROM_JOB fails, retry without the flag
        if (err == ERROR_ACCESS_DENIED) {
            Log("Retrying without BREAKAWAY_FROM_JOB");
            result = CreateProcessAsUserW(
                hDupToken, NULL, cmdLine, NULL, NULL, FALSE,
                CREATE_UNICODE_ENVIRONMENT | CREATE_NEW_CONSOLE,
                pEnv, NULL, &si, &pi
            );
            if (!result) {
                Log("Retry also failed", GetLastError());
            } else {
                Log("Retry succeeded");
            }
        }
    }

    if (result) {
        sprintf(buf, "Process started, PID: %lu", pi.dwProcessId);
        Log(buf);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }

    DestroyEnvironmentBlock(pEnv);
    CloseHandle(hDupToken);
    Log("=== End ===");
    return result ? 0 : 7;
}
