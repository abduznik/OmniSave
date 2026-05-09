# OmniSave

OmniSave is a lightweight, portable game save synchronization tool for Windows, specifically designed to run reliably under **Wine and CrossOver**.

It acts as a "Launch Wrapper" that automates the process of fetching your saves from a portable location (like a USB drive or a cloud-synced folder), launching the game, and then backing up your progress immediately after you finish playing.

## Key Features

- **Sync-Launch-Sync Architecture**: Ensures your local environment is up-to-date before playing and your remote backup is updated after.
- **Process Polling**: Unlike standard wrappers that wait for a process handle, OmniSave polls the system memory. This allows it to stay alive even if the game launcher (like Rockstar Launcher) detaches or spawns background child processes.
- **Launch Arguments Support**: Pass arbitrary boot flags (like `-nobattleye` or `-windowed`) directly to the game.
- **Path Resolution**: Supports `~/` expansion for user profile directories and `./` for relative portable paths.
- **Mutex Locking**: Prevents multiple instances from running simultaneously to avoid save corruption.

## How It Works

1. **Pre-Sync**: Copies files from `Remote_Path` to `Local_Path`.
2. **Launch**: Executes the `Launch_Command` with any `Launch_Args`.
3. **Monitor**: Waits for the specific executable to appear in memory and then monitors it until it terminates.
4. **Post-Sync**: Copies updated files from `Local_Path` back to `Remote_Path`.

## Configuration (`omnisave.ini`)

Place `omnisave.ini` in the same directory as `OmniSave.exe`.

```ini
[OmniSave]
; The executable name to launch and monitor
Launch_Command=GTA5.exe

; Optional command line arguments
Launch_Args=-nobattleye

; The local path where the game expects saves (supports ~/)
Local_Path=~/Documents/Rockstar Games/GTA V

; The remote path for backups (supports ./)
Remote_Path=./portable_saves/GTA_V
```

## Build Instructions

Built using MinGW-w64 for cross-platform compatibility.

```bash
make clean && make
```

## License
MIT
