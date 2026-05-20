# Pong Reloaded

A classic arcade game rebuilt from the ground up using modern C++ and the Raylib library. 

---

## Credits & Attributions

### Code Base Template
* **Original Author:** educ8s (Nick Koumaris)
* **Original Repository:** [github.com/educ8s/Cpp-Pong-Game-Raylib](https://github.com/educ8s/Cpp-Pong-Game-Raylib)
* **Description:** This project utilizes core architectural patterns and Raylib integrations adapted from the original open-source template repository listed above.

### Asset Credits & Attribution
The multimedia resources (graphics, sprites, and audio files) used in this game are not my personal creations. They have been curated and adapted from the open-source community for educational development.
* **Original Project:** Moddable Pong
* **Author / Creator:** Endless OS Foundation & Endless Studios
* **Website:** [endlessos.org](https://endlessos.org)
* **Source Code Repository:** [github.com/endlessm/moddable-pong](https://github.com/endlessm/moddable-pong/)
* **Usage Notice:** These assets are integrated strictly for non-commercial, academic layout validation and development milestones. All rights, ownership, and copyrights belong entirely to the original creators at the Endless OS Foundation.

---

## Features

* **Clean Object-Oriented Architecture:** Developed using rigorous OOP principles, utilizing a base abstract `GameObject` class with virtual override structures for specialized modular entities (`Ball`, `Paddle`, and `CpuPaddle`).
* **Robust Game State Machine:** Features an integrated game loop managing distinct application states including `MainMenu`, `DifficultySelect`, `Multiplayer`, `Settings`, and `Playing`.
* **Dynamic AI Opponent:** A singleplayer CPU opponent equipped with configurable movement velocities tailored across three distinct difficulty tiers: Easy, Normal, and Hard.
* **Enhanced Visual Layout:** Fully upgraded with high-resolution texture mapping via Raylib's `DrawTexturePro`, featuring dedicated game court backdrops, localized wall segments, styled central dashed lines, and custom entity sprites.
* **Academic Context:** This work is part of a student engineering project within the Helios Additional_activity initiative at the Technical University of Cluj-Napoca (UTCN) - Faculty of Electronics, Telecommunications and Information Technology (ETTI).