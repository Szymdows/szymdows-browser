#ifndef WINDOW_H
#define WINDOW_H

#include <string>
#include <vector>
#include <gtk/gtk.h>

// Structure to hold parsed HTML elements
// This stores information about each HTML tag we parse
struct HTMLElement {
    std::string tag;       // The tag name like "h1" or "p"
    std::string content;   // The text inside the tag
};

// Browser Window Header
// This defines the main browser window class with GTK support
// Now it creates an actual graphical window and renders HTML with proper formatting

/// The main browser window class
/// This manages the browser UI and displays rendered content in a GTK window
class BrowserWindow {
public:
    // Constructor - Creates a new browser window
    BrowserWindow();
    
    // Destructor - Cleans up the window resources
    ~BrowserWindow();
    
    /// Creates and shows the GTK window
    /// This displays the window to the user
    void show();
    
    /// Sets the HTML content to be displayed in the window
    /// This parses the HTML and renders it with proper formatting
    /// @param html - The HTML content as a string
    void set_html(const std::string& html);
    
    /// Starts the main GTK event loop
    /// This keeps the window open and responsive to user interactions
    void run();
    
    /// Sets the window title
    /// @param title - The text to display in the title bar
    void set_title(const std::string& title);
    
    /// Internal function to setup all the GTK widgets
    /// This creates the window, text view, and applies styling
    void setup_ui();
    
    /// Gets the GTK window widget
    /// @return Pointer to the GTK window
    GtkWidget* get_window() { return window; }
    
private:
    std::string title;        // Window title text
    int width;                // Window width in pixels
    int height;               // Window height in pixels
    
    // GTK widgets - these are the graphical components
    GtkWidget* window;        // The main window
    GtkWidget* scrolled_window;  // Scrollable container for the text
    GtkWidget* text_view;     // Text display widget
    GtkTextBuffer* text_buffer;  // Buffer that holds the actual text
    
    /// This renders HTML content into the text buffer with formatting
    /// It applies different styles to different HTML tags
    /// @param html - The HTML string to render
    void render_html(const std::string& html);
    
    /// This parses HTML string into individual elements
    /// It extracts tags and their content from the HTML
    /// @param html - The HTML string to parse
    /// @return Vector of HTMLElement structures containing parsed data
    std::vector<HTMLElement> parse_html(const std::string& html);
};

#endif // WINDOW_H