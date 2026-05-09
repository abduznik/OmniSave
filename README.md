# OmniSave 🛸
**The "Braindead" Portable Game Sync Engine**

OmniSave is a lightweight, zero-configuration sync engine designed to make Windows games truly portable across different machines and operating systems (macOS via CrossOver/Wine, Linux, and Windows).

It acts as a silent "Proxy" between your launcher and the game, ensuring your save data is always with you on your USB drive or external SSD, without ever needing to install software on the host machine.

## 🚀 Features
- **Zero-Install Proxy:** Simply rename your game's original `.exe` and drop OmniSave in its place.
- **Dynamic Save Discovery:** Automatically resolves complex Windows paths like `~/Documents` and `~/AppData` within Wine/CrossOver environments.
- **Silent Operation:** Runs invisibly in the background. No CMD windows, no popups.
- **Anti-Loop Protection:** Implements Windows Named Mutexes to prevent infinite spawn loops triggered by aggressive "Self-Healing" game launchers (like GTA V).
- **Auto-Sync:** Performs a high-speed pre-flight sync before the game starts and a post-flight sync after you exit.
- **Braindead Setup:** Includes a CLI wizard that configures everything for you in seconds.

## 🛠 How it Works
1. OmniSave replaces the main game executable (e.g., `PlayGtaV.exe`).
2. When launched, it checks the USB for newer save files and copies them to the host's `Documents/AppData`.
3. It launches the *real* game binary (e.g., `PlayGtaV_original.exe`).
4. It sits silently in memory, acting as a dummy launcher to satisfy anti-cheat/DRM checks.
5. Once the game closes, it syncs your new progress back to the USB.

## 📦 Project Structure
- `main.c`: Core logic, Setup Wizard, and Proxy orchestration.
- `sync_engine.c`: Recursive, high-performance directory synchronization.
- `path_utils.c`: Cross-platform path resolution (Wine/macOS/Windows).
- `config_parser.c`: Lightweight INI configuration handler.
- `process_manager.c`: Native process spawning and monitoring.

## 🚧 Current Status & Known Issues
- **Status:** Functional Beta. Currently optimized for GTA V and Rockstar Games titles.
- **Issue:** Large `Documents` folders can cause slow syncs if the user doesn't narrow down the `omnisave.ini` paths (Fix: Use specific subfolders in `.ini`).
- **Issue:** Some Wine environments may require specific Mutex permissions for global namespace access.

## 🔨 Building
```bash
# macOS Build
make

# Windows Build (requires mingw-w64)
make windows
```

---
*Created with ❤️ for the portable gaming community.*