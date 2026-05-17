import os
import sys
import subprocess
import platform

# This script handles building and running the Szymdows Browser
# It compiles both the Rust and C++ components and links them together

def build_rust_components():
    """
    This function compiles the Rust parts of the engine.
    Returns True if successful, False if there were errors.
    """
    print("=" * 50)
    print("Building Rust components...")
    print("=" * 50)
    
    current_dir = os.getcwd()
    
    # Build the HTML parser
    print("\n[1/2] Compiling HTML parser...")
    try:
        os.chdir(os.path.join("engine", "parser"))
        result = subprocess.run(["cargo", "build", "--release"], check=True)
        print("[SUCCESS] HTML parser compiled successfully")
    except subprocess.CalledProcessError:
        print("[ERROR] Error compiling HTML parser")
        os.chdir(current_dir)
        return False
    except FileNotFoundError:
        print("[ERROR] Cargo not found. Please install Rust from https://rustup.rs/")
        os.chdir(current_dir)
        return False
    
    os.chdir(current_dir)
    
    # Build the DOM module
    print("\n[2/2] Compiling DOM module...")
    try:
        os.chdir(os.path.join("engine", "dom"))
        result = subprocess.run(["cargo", "build", "--release"], check=True)
        print("[SUCCESS] DOM module compiled successfully")
    except subprocess.CalledProcessError:
        print("[ERROR] Error compiling DOM module")
        os.chdir(current_dir)
        return False
    
    os.chdir(current_dir)
    
    print("\n[SUCCESS] All Rust components built successfully!\n")
    return True

def build_cpp_components():
    """
    This function compiles the C++ parts using CMake.
    It creates a build directory and runs CMake to generate build files.
    """
    print("=" * 50)
    print("Building C++ components...")
    print("=" * 50)
    
    # Create the build output directory if it doesn't exist
    build_dir = os.path.join("build", "output")
    if not os.path.exists(build_dir):
        os.makedirs(build_dir)
        print(f"Created build directory: {build_dir}")
    
    current_dir = os.getcwd()
    os.chdir(build_dir)
    
    # Run CMake to generate build files
    print("\nGenerating build files with CMake...")
    try:
        result = subprocess.run(["cmake", "../.."], check=True)
        print("[SUCCESS] CMake configuration successful")
    except subprocess.CalledProcessError:
        print("[ERROR] CMake configuration failed")
        os.chdir(current_dir)
        return False
    except FileNotFoundError:
        print("[ERROR] CMake not found. Please install CMake from https://cmake.org/")
        os.chdir(current_dir)
        return False
    
    # Build the project
    print("\nCompiling C++ code...")
    try:
        result = subprocess.run(["cmake", "--build", ".", "--config", "Release"], check=True)
        print("[SUUCESS] C++ components compiled successfully")
    except subprocess.CalledProcessError:
        print("[ERROR] C++ compilation failed")
        os.chdir(current_dir)
        return False
    
    os.chdir(current_dir)
    
    print("\n[SUCCESS] All C++ components built successfully!\n")
    return True

def build_all():
    """
    This is the main build function that coordinates building everything.
    It builds Rust first, then C++.
    """
    print("\n" + "=" * 50)
    print("BUILDING SZYMDOWS BROWSER")
    print("=" * 50 + "\n")
    
    # Build Rust components first
    if not build_rust_components():
        print("\n[ERROR] Build failed at Rust compilation stage")
        return False
    
    # Then build C++ components
    if not build_cpp_components():
        print("\n[ERROR] Build failed at C++ compilation stage")
        return False
    
    print("=" * 50)
    print("BUILD COMPLETED SUCCESSFULLY!")
    print("=" * 50)
    print("\nYou can now run the browser with: ./szym run")
    return True

def run_browser(html_file=None):
    """
    This function runs the compiled browser executable.
    If html_file is provided, it passes it as an argument.
    """
    print("\n" + "=" * 50)
    print("RUNNING SZYMDOWS BROWSER")
    print("=" * 50 + "\n")
    
    # Figure out the executable path based on platform
    if platform.system() == "Windows":
        executable = os.path.join("build", "output", "Release", "SzymdowsBrowser.exe")
        # Sometimes it's in Debug folder during development
        if not os.path.exists(executable):
            executable = os.path.join("build", "output", "Debug", "SzymdowsBrowser.exe")
    else:
        executable = os.path.join("build", "output", "SzymdowsBrowser")
    
    # Check if executable exists
    if not os.path.exists(executable):
        print(f"[ERROR] Browser executable not found at: {executable}")
        print("Please build the project first with: ./szym build")
        return False
    
    # Run the browser
    try:
        if html_file:
            subprocess.run([executable, html_file])
        else:
            subprocess.run([executable])
    except Exception as e:
        print(f"[ERROR] Error running browser: {e}")
        return False
    
    return True

def clean_build():
    """
    This function removes all build artifacts to start fresh.
    Useful when things get messed up during development.
    """
    print("\n" + "=" * 50)
    print("CLEANING BUILD ARTIFACTS")
    print("=" * 50 + "\n")
    
    import shutil
    
    # Clean Rust builds
    rust_dirs = [
        os.path.join("engine", "parser", "target"),
        os.path.join("engine", "dom", "target"),
    ]
    
    for dir_path in rust_dirs:
        if os.path.exists(dir_path):
            print(f"Removing {dir_path}...")
            shutil.rmtree(dir_path)
    
    # Clean C++ build
    build_output = os.path.join("build", "output")
    if os.path.exists(build_output):
        print(f"Removing {build_output}...")
        shutil.rmtree(build_output)
    
    print("\n[SUCCESS] Clean completed!")

def show_help():
    """
    This displays help informaton about available commands.
    """
    print("""
Szymdows Browser Build System (szym)

Usage: ./szym [command] [options]

Commands:
    build           - Compile all components of the browser
    run [file]      - Run the browser (optionally with an HTML file)
    clean           - Remove all build artifacts
    help            - Show this help message

Examples:
    ./szym build                    # Build the entire project
    ./szym run                      # Run with default test conten
    ./szym run test.html           # Run and load test.html
    ./szym clean                    # Clean all build files

For more information, see README.md
""")

def main():
    """
    Main entry point for the build script.
    This parses command line arguments and calls the appropriate functions.
    """
    if len(sys.argv) < 2:
        print("No command specified.")
        show_help()
        return
    
    command = sys.argv[1].lower()
    
    if command == "build":
        build_all()
    
    elif command == "run":
        # Check if HTML file was specified
        html_file = sys.argv[2] if len(sys.argv) > 2 else None
        run_browser(html_file)
    
    elif command == "clean":
        clean_build()
    
    elif command == "help" or command == "--help" or command == "-h":
        show_help()
    
    else:
        print(f"Unknown command: {command}")
        show_help()

if __name__ == "__main__":
    main()
