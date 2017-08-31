# MiSTer
Main_MiSTer rework on C++. Re-architecture and preparing for future expansion

ToDo:
1. ~~Establish modular structure~~
2. Prepare common functionality/logic for: ~~files~~, ~~logging~~, disk images
3. Transfer ~~startup~~/FPGA bitstream load functionality
4. Transfer OSD menu parts
5. Transfer I/O and FPGA interconnects
6. Modernize all file/disk image operations using unified logic
7. Transfer each core related functionality into separate modules (potentially can lead to plugin-based project structure)

Technology stack:
- Target system: Yocto Linux 32-bit (Altera maintained [SocFPGA repository](https://github.com/altera-opensource/linux-socfpga))
- Cross-compilation toolchain: Linaro 6.3.x based (official Linaro for Windows x64 and Linux, self-assembled for Mac OS X)
- Toolchain prefix: arm-linux-gnueabihf (hard-float, neon-fp16)
- LFS (Large Support) turned on

Development tools:
- Eclipse C++ package for Mac.

Doesn't really matter what host OS or IDE to use. Once you have cross-platform toolchain - everything can work.
Tried:
- JetBrains CLion - CMake based and very immature as C/C++ IDE. But best syntax analyzer and refactoring (as every IDE from JetBrains)
- Microsoft VS Code - Sublime Text on steroids. Good for text editing, bunch of messy plugins for everything else.
- Visual Studio 2017 - it's from other world, probably can work with plugins like VisualGDB, but out of the box not intended for Linux cross-platform development.
