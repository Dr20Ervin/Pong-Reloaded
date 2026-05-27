# 🏓 Pong Reloaded

A classic arcade game rebuilt from the ground up using **Modern C++ (C++20)** and the **Raylib** library. Designed with performance, customization, and modularity in mind.

[![C++ Version](https://img.shields.io/badge/Language-C%2B%2B20-blue?logo=c%2B%2B&style=flat-square)](https://en.cppreference.com/w/cpp/compiler_support/20)
[![Raylib](https://img.shields.io/badge/Framework-Raylib--6.0-red?style=flat-square)](https://www.raylib.com/)
[![Platform](https://img.shields.io/badge/Platform-Windows-0078d7?logo=windows&style=flat-square)](https://www.microsoft.com/windows)
[![License](https://img.shields.io/badge/License-MIT-green?style=flat-square)](LICENSE.txt)

---

## 🚀 About & Motivation

**Pong Reloaded** is an educational and exploratory game development project. Built as a student engineering activity, it serves as a personal challenge to push past standard university curricula and experiment with low-level graphic structures.

The key goals of this project:
* **Master Raylib & Game Design:** Experiment with 2D rendering pipelines, sound synthesis, custom sprites, and real-time game state management.
* **Optimize for Low Resources:** Implement performance-first programming in C++, minimizing memory overhead and achieving a highly optimized execution path that runs perfectly on low-end laptops.
* **Challenge Knowledge Boundaries:** Apply advanced Object-Oriented Programming (OOP) concepts, robust state machine architectures, and direct Win32 API integrations.

> [!NOTE]
> This project is developed within the **Helios Additional Activity** initiative at the **Technical University of Cluj-Napoca (UTCN)** – *Faculty of Electronics, Telecommunications and Information Technology (ETTI)*.

---

## 🎮 Features

* **👾 Zero-Dependency Portability:** The game compiles into a single, standalone executable. Textures, audio, and icon files are embedded directly into the binary's resource section (`.rc`) using Windows PE resources, ensuring it never breaks due to missing assets.
* **⚡ Highly Optimized Engine:** Optimized game loops, zero-allocation physics updates in the hot-path, and efficient CPU sleeping patterns. Smooth frame rates on hardware with minimal system resources.
* **🤖 Dynamic AI Opponent:** A singleplayer CPU opponent powered by adaptive movement algorithms with three difficulty settings: **Easy**, **Normal**, and **Hard**.
* **👥 Local Co-op Multiplayer:** Local 1v1 couch-co-op mode featuring an interactive **Multiplayer Lobby** where both players toggle ready states before initiating the match.
* **🎨 Premium Settings & Theme Engine:**
  * **Screen Settings:** Customizable resolutions (1280x800, 1600x900, 1920x1080) and Fullscreen / Windowed modes.
  * **Framerate Controls:** Selectable 60 FPS, 144 FPS, or VSync limits.
  * **Score Limits:** Custom thresholds (5, 11, 15, or 21 points).
  * **Visual Themes:** Toggleable background space themes (Default, Galaxy, Trap), custom ball skins, and paddle textures.
* **🛠️ Developer Console:** Built-in console overlay toggled dynamically via `F7` for real-time engine tracing and logging.

---

## 💾 Quick Install & Play

No installers, DLLs, or complex dependency installations are required! 

1. Head over to the [**Latest Releases**](https://github.com/Dr20Ervin/Pong-Reloaded/releases) section.
2. Download the `pong-reloaded.exe` binary.
3. Double-click the `.exe` and start playing immediately!

---

## ⌨️ Controls Reference

| Context | Action / Key | Player 1 (Left Paddle) | Player 2 / Singleplayer (Right Paddle) |
| :--- | :--- | :--- | :--- |
| **Menu Navigation** | Up / Down | — | `Arrow Up` / `Arrow Down` |
| **Menu Confirm** | Enter | — | `Enter` |
| **Lobby Ready Toggle** | Ready State | `W` | `Arrow Up` |
| **Gameplay Movement**| Up / Down | `W` / `S` | `Arrow Up` / `Arrow Down` |
| **Gameplay Actions** | Pause / Resume | — | `P` |
| **Gameplay Actions** | Force Game Over / Reset | — | `Space` |
| **Developer Tools** | Toggle Dev Console | — | `F7` |

---

## 🛠️ Building From Source

### Prerequisites
* Windows OS
* Visual Studio 2022 (with "Desktop development with C++" workload)
* Raylib library installed/configured

### Steps
1. Clone this repository:
   ```bash
   git clone https://github.com/Dr20Ervin/Pong-Reloaded.git
   cd Pong-Reloaded
   ```
2. Open `pong-reloaded.sln` in Visual Studio.
3. Build the project under `Release` / `x64` configuration to generate the optimized, standalone executable.

---

## 📜 Credits & Attributions

### Codebase Template
* **Original Author:** educ8s (Nick Koumaris)
* **Original Repository:** [educ8s/Cpp-Pong-Game-Raylib](https://github.com/educ8s/Cpp-Pong-Game-Raylib)
* This project adapts core architectural concepts and Raylib integrations from the original repository.

### Asset Credits
The graphical and audio resources used in this game were curated from the open-source gaming community:
* **Original Project:** Moddable Pong
* **Author/Creator:** Endless OS Foundation & Endless Studios
* **Website:** [endlessos.org](https://endlessos.org)
* **Repository:** [endlessm/moddable-pong](https://github.com/endlessm/moddable-pong/)
* *Notice:* All assets are utilized strictly for academic research, non-commercial development, and validation milestones. All rights and copyrights belong to their respective owners.

---

## 📄 License

This project is licensed under the [MIT License](LICENSE.txt).
