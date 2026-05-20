# Szymdows Browser

This is my attempt at creating a web browser from scratch. It's called Szymdows Browser and uses the Szymdows Web Engine which I'm also building.

## Building the Project

### For Linux and WSL:

#### To build, run the following command: `./szym build`

#### To run the browser after building: `./szym run`

### For Windows:

#### To build, run the following command: `szym.bat build`

#### To run the browser after building: `szym.bat run`

### outdated - Follow the Windows requirement install guide! [Windows Setup Guide](https://github.com/Szymdows/szymdows-browser#windows-setup-guide-for-szymdows-browser)

## What Works Right Now

- Parsing and rendering `<h1>` tags
- Parsing and rendering `<p>` tags
- Basic HTML parsing (though it doesn't handle nested tags yet, working on that)

## System Requirements

- Rust (I'm using version 1.70+ but other versions might work)
- C++ compiler (MSVC on Windows or GCC/Clang on Linux)
- CMake version 3.10 or higher
- Python 3 for the build script
- Pkg-config
- GTK 3 Development Libraries

- You can install all of these dependencies with this command: `sudo apt update && sudo apt install build-essential cmake python3 pkg-config libgtk-3-dev libcurl4-openssl-dev`

## Known Issues

- Nested HTML tags don't work properly yet (I'm trying to figure out how to handle them)
- The UI is very basic right now, it just displays text in a minimal window
- Memory management might have some issues, still learning about that

## Planned Features

- Support for more HTML tags (div, span, a, etc.)
- Actual graphical rendering instead of just formatted text in a minimal window
- CSS support eventually
- JavaScript support (this will be really hard)

## Notes

This is my first time building something this complex, so the code might not be perfect. I'm learning as I go and trying to understand how browser engines work by reading about Gecko and Blink.

# Windows Setup Guide for Szymdows Browser - outdated

## Warning

Setting up on Windows is more complex than Linux/WSL. If possible, use WSL2 instead.

## Prerequisites

### 1. Visual Studio

- Download Visual Studio Community from https://visualstudio.microsoft.com/
- During installation, select "Desktop development with C++"
- Restart after installation

### 2. CMake

- Download from https://cmake.org/download/
- Get the Windows installer
- During installation, select "Add CMake to system PATH"

### 3. Rust

- Go to https://rustup.rs/
- Download and run rustup-init.exe
- Follow the prompts (default options are fine)
- Restart your terminal after installation

### 4. Python 3

- Download from https://python.org/downloads/
- Run installer
- **IMPORTANT**: Check "Add Python to PATH" during installation

### 5. GTK for Windows (The Hard Part)

- Download GTK Runtime from: https://github.com/tschoonj/GTK-for-Windows-Runtime-Environment-Installer/releases
- Install to default location (usually C:\Program Files\GTK3-Runtime Win64)
- Add to PATH: `C:\Program Files\GTK3-Runtime Win64\bin`
- Add to PKG_CONFIG_PATH: `C:\Program Files\GTK3-Runtime Win64\lib\pkgconfig`

### Setting Environment Variables

1. Search for "Environment Variables" in Windows
2. Click "Environment Variables"
3. Under "System Variables", find "Path", click Edit
4. Add: `C:\Program Files\GTK3-Runtime Win64\bin`
5. Create new variable: PKG_CONFIG_PATH = `C:\Program Files\GTK3-Runtime Win64\lib\pkgconfig`

## Building

Open Command Prompt or PowerShell, and type in: `szym.bat build`

## Running

Open Command Prompt or PowerShell, and type in: `szym.bat run`

## If GTK doesn't work...

Consider using WSL2 instead. It's much simpler!

## WSL:

I really do recommend installing and using WSL since it's much easier to build and test this web engine/browser!