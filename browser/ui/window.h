#ifndef WINDOW_H
#define WINDOW_H

#include <string>

// Browser Window Header
// This defines the main browser window class
// Currently it's just a console-based interface but eventually
// this should create a actual graphical window

/// The main browser window class
/// This manages the browser UI and displays rendered content
class BrowserWindow {
public:
    // Constructor - Creates a new browser window
    BrowserWindow();
    
    // Destructor - Cleans up the window resources
    ~BrowserWindow();
    
    /// Displays the browser window to the user
    /// This shows the window chrome (title bar, etc.)
    void show();
    
    /// Sets the content to be displayed in the window
    /// @param content - The rendered HTML content as a string
    void set_content(const std::string& content);
    
    /// Starts the main event loop for the window
    /// This keeps the window open and responsive
    /// Currently it just waits for user input, but eventually
    /// this should handle events like mouse clicks, keyboard input, etc.
    void run();
    
    /// Sets the window title
    /// @param title - The text to display in the title bar
    void set_title(const std::string& title);
    
private:
    std::string content;      // The content being displayed
    std::string title;        // Window title
    bool is_running;          // Whether the window event loop is running
    int width;                // Window width
    int height;               // Window height
    
    /// Internal function to display the window border/chrome
    void display_chrome();
};

#endif // WINDOW_H