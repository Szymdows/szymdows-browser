# Szymdows Browser

This is my attempt at creating a web browser from scratch. It's called Szymdows Browser and uses the Szymdows Web Engine which I'm also building.

## Building the Project

### To build, run the following command: `./szym build`

### To run the browser after building: `./szym run`

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

- You can install all of these dependencies with this command: `sudo apt update && sudo apt install build-essential cmake python3 pkg-config libgtk-3-dev`

## Known Issues

- Nested HTML tags don't work properly yet (I'm trying to figure out how to handle them)
- The UI is very basic right now, it just displays text in a minimal window
- Memory management might have some issues, still learning about that

## Planned Features

- Support for more HTML tags (div, span, a, etc.)
- Actual graphical rendering instead of just text
- CSS support eventually
- JavaScript support (this will be really hard)

## Notes

This is my first time building something this complex, so the code might not be perfect. I'm learning as I go and trying to understand how browser engines work by reading about Gecko and Blink.