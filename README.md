# Dark Reflection - Refactored Version

## Project Description
The file 'BSAI24056_project_01.cpp' is my original Semester 1 programming project. 
It was written with nested loops and Windows-specific logic.

I have refactored the code into 'dark_reflection_refactored.cpp' to make it 
compatible with Linux, Windows, and Web browsers (WebAssembly). The new version 
uses a state-machine architecture to prevent crashes in the browser environment.

## Build Requirements
You must have Raylib installed on your system. 
For the Web version, you must have the Emscripten SDK installed.
Also not a build requirement but in general I hardcoded the 1920x1080 resolution since I didn't know what to do backthen so yeah.

## Build Commands

Build for Linux:
make linux

Build for Windows (using MinGW):
make windows

Build for Web (WASM):
make web

Clean build files:
make clean

## Running the Web Version
Also make sure you use fullscreen.
After running 'make web', use a local server to view index.html:
python3 -m http.server 8000