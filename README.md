SessionSpawner
==============

Spawns a SYSTEM-level command prompt on the active console session's
desktop by duplicating winlogon.exe's primary token.

Usage
--
Combine with [YellowKey](https://github.com/Nightmare-Eclipse/YellowKey) to get SYSTEM level command prompt any user's desctop. (even works on restricted device such as work/school computer!)

Requirements
------------
- Windows 10 / 11 (also works on Server 2016+).
- The launching process must already run as NT AUTHORITY\SYSTEM, or
  hold SeAssignPrimaryTokenPrivilege, SeIncreaseQuotaPrivilege, and
  SeTcbPrivilege.
- A user must be logged into the console session (otherwise there is
  no active session and no winlogon to impersonate at the desktop).


Build
-----
    cl /EHsc test.cpp /link wtsapi32.lib userenv.lib advapi32.lib

Logging
-------
All steps and Win32 error codes are appended to:
    C:\Windows\Temp\utilman_log.txt


Disclaimer
----------
This tool is intended for authorized administrative recovery, security
research, CTF challenges, and educational use only. **I will not take responsibility of any issues caused by using this tool.**
