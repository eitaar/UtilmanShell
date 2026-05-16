# UtilmanShell

Spawns a SYSTEM-level command prompt on the active console session's desktop by duplicating winlogon.exe's primary token.

Combine with [YellowKey](https://github.com/Nightmare-Eclipse/YellowKey) to obtain a SYSTEM-level shell on any user's desktop — including restricted devices such as work or school computers.

## Requirements

- Windows 10 / 11, or Windows Server 2016 and later
- The launching process must run as `NT AUTHORITY\SYSTEM`, **or** hold all three of the following privileges:
  - `SeAssignPrimaryTokenPrivilege`
  - `SeIncreaseQuotaPrivilege`
  - `SeTcbPrivilege`
- A user must be logged into the console session (winlogon must be present on the active desktop)

---

## Build

if you can't trust the uploaded exe file, compile with Developer Command Prompt for VS 2022:

```bat
cl /EHsc UtilmanShell.cpp /link wtsapi32.lib userenv.lib advapi32.lib
```

---

## Logging

All steps and Win32 error codes are appended to:

```
C:\Windows\Temp\utilman_log.txt
```

---

## Disclaimer

This tool is intended for **authorized** administrative recovery, security research, CTF challenges, and educational use only.  
The author takes no responsibility for any damage or misuse caused by this tool.
