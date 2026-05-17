#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "window.h"
#include "../../engine/renderer/render.h"

using namespace std;

// Browser Window Implementation

// Constructor - This initializes the browser window with default values
BrowserWindow::BrowserWindow() {
    width = 800;
    height = 600;
    is_running = false;
    content = "";
    title = "Szymdows Browser";
    
    cout << "Browser window created" << endl;
}

// Destructor - This cleans up resources when the window is closed
BrowserWindow::~BrowserWindow() {
    cout << "Browser window destroyed" << endl;
}

// This displays the browser chrome (decorative border and title)
void BrowserWindow::display_chrome() {
    // Create a decorative border using ASCII characters
    cout << string(60, '=') << endl;
    cout << "  " << title << endl;
    cout << string(60, '=') << endl;
    cout << endl;
}

// This shows the window to the user
void BrowserWindow::show() {
    display_chrome();
}

// This sets the content that will be displayed in the window
void BrowserWindow::set_content(const string& c) {
    content = c;
}

// This sets the window title
void BrowserWindow::set_title(const string& t) {
    title = t;
}

// This is the main event loop for the browser window
// It displays the content and waits for user input
void BrowserWindow::run() {
    is_running = true;
    
    // Show the window chrome
    show();
    
    // Display the rendered content
    cout << content << endl;
    
    // Wait for user to press enter before closing
    // TODO: In a real browser this would be an actual event loop
    // handling mouse clicks, scrolling, keyboard input, etc.
    cout << endl;
    cout << "Press Enter to close the browser...";
    string dummy;
    getline(cin, dummy);
    
    is_running = false;
}

// Main function - This is the entry point for the browser application
int main(int argc, char* argv[]) {
    cout << "Starting Szymdows Browser..." << endl;
    cout << endl;
    
    // Default test HTML to display if no file is provided
    // This helps with testing during development
    string html_content = 
        "<h1>Welcome to Szymdows Browser</h1>"
        "<p>This is a simple web browser built from scratch using C++ and Rust.</p>"
        "<h1>Current Features</h1>"
        "<p>Right now the browser can render h1 and p tags. More features coming soon!</p>";
    
    // Check if the user provided an HTML file as a command line argument
    if (argc > 1) {
        string filename = argv[1];
        cout << "Loading HTML file: " << filename << endl;
        
        // Try to open and read the file
        ifstream file(filename);
        if (file.is_open()) {
            // Read the entire file into a string
            stringstream buffer;
            buffer << file.rdbuf();
            html_content = buffer.str();
            file.close();
            
            cout << "File loaded successfully" << endl;
        } else {
            // File couldn't be opened
            cerr << "Error: Could not open file '" << filename << "'" << endl;
            cerr << "Using default test content instead" << endl;
        }
        cout << endl;
    }
    
    // Create the renderer
    cout << "Initializing renderer..." << endl;
    Renderer renderer;
    
    // Render the HTML content
    cout << "Rendering HTML..." << endl;
    renderer.render(html_content);
    
    // Get the rendered output from the renderer
    string rendered_output = renderer.get_output();
    cout << "Rendering complete" << endl;
    cout << endl;
    
    // Create the browser window
    cout << "Creating browser window..." << endl;
    BrowserWindow window;
    
    // Set the rendered content in the window
    window.set_content(rendered_output);
    
    // Run the browser (this will display the window and wait for user input)
    window.run();
    
    cout << endl;
    cout << "Browser closed. Thanks for using Szymdows Browser!" << endl;
    
    return 0;
}