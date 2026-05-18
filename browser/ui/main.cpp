#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <gtk/gtk.h>
#include "window.h"

using namespace std;

// Browser Window Implementation with GTK
// This file contains all the code for creating and managing the browser window

// Constructor - This initializes the browser window with default values
BrowserWindow::BrowserWindow() {
    width = 800;
    height = 600;
    title = "Szymdows Browser";
    
    // Initialize GTK widgets to NULL
    // They'll be created later in setup_ui()
    window = NULL;
    scrolled_window = NULL;
    text_view = NULL;
    text_buffer = NULL;
    
    cout << "Browser window created" << endl;
}

// Destructor - This cleans up resources when the window is closed
BrowserWindow::~BrowserWindow() {
    cout << "Browser window destroyed" << endl;
    // GTK handles cleanup of widgets automatically so we don't need to do much here
}

// This sets up all the GTK user interface widgets
// It creates the window, text view, scrolling, and applies styling
void BrowserWindow::setup_ui() {
    // Create the main window
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), title.c_str());
    gtk_window_set_default_size(GTK_WINDOW(window), width, height);
    
    // Connect the destroy signal to quit the application
    // This makes the X button work to close the window
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    
    // Create a scrolled window so we can scroll if content is long
    // This is important for pages with lots of content
    scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
                                   GTK_POLICY_AUTOMATIC,
                                   GTK_POLICY_AUTOMATIC);
    
    // Create a text view widget to display the rendered content
    text_view = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(text_view), FALSE);  // Make it read-only
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(text_view), FALSE);  // Hide cursor
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(text_view), GTK_WRAP_WORD);  // Wrap long lines
    
    // Set some padding so text doesn't touch the edges
    // This makes it look nicer and easier to read
    gtk_text_view_set_left_margin(GTK_TEXT_VIEW(text_view), 20);
    gtk_text_view_set_right_margin(GTK_TEXT_VIEW(text_view), 20);
    gtk_text_view_set_top_margin(GTK_TEXT_VIEW(text_view), 20);
    gtk_text_view_set_bottom_margin(GTK_TEXT_VIEW(text_view), 20);
    
    // Get the text buffer from the text view
    // The buffer is where we actually store and format the text
    text_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
    
    // Create text tags for different HTML elements
    // These tags define how different elements should look
    
    // h1 tag - large bold text for headings
    // Using scale 2.0 means it's twice as big as normal text
    gtk_text_buffer_create_tag(text_buffer, "h1",
                              "weight", PANGO_WEIGHT_BOLD,  // Make it bold
                              "scale", 2.0,  // 2x normal size
                              "foreground", "#000000",  // Black color
                              "pixels-above-lines", 15,  // Space above
                              "pixels-below-lines", 10,  // Space below
                              NULL);
    
    // p tag - normal paragraph text
    // This is for regular content
    gtk_text_buffer_create_tag(text_buffer, "p",
                              "foreground", "#333333",  // Dark gray color
                              "pixels-below-lines", 12,  // Space below paragraph
                              NULL);
    
    // Add the text view to the scrolled window
    gtk_container_add(GTK_CONTAINER(scrolled_window), text_view);
    
    // Add the scrolled window to the main window
    gtk_container_add(GTK_CONTAINER(window), scrolled_window);
    
    // Style the text view with CSS to make it look nice
    // This sets the background to white and chooses a good font
    GtkCssProvider* css_provider = gtk_css_provider_new();
    const gchar* css_data = "textview { background-color: white; font-family: 'Sans'; font-size: 14px; }";
    gtk_css_provider_load_from_data(css_provider, css_data, -1, NULL);
    
    GtkStyleContext* context = gtk_widget_get_style_context(text_view);
    gtk_style_context_add_provider(context,
                                   GTK_STYLE_PROVIDER(css_provider),
                                   GTK_STYLE_PROVIDER_PRIORITY_USER);
    
    cout << "UI setup complete" << endl;
}

// This parses simple HTML into elements
// It's a basic parser that finds tags and their content
// TODO: This should probably use the Rust parser eventually but this works for now
vector<HTMLElement> BrowserWindow::parse_html(const string& html) {
    vector<HTMLElement> elements;
    
    size_t pos = 0;
    
    // Loop through the entire HTML string
    while (pos < html.length()) {
        // Find the next opening tag (starts with <)
        size_t tag_start = html.find('<', pos);
        if (tag_start == string::npos) {
            // No more tags found
            break;
        }
        
        // Find the closing bracket of the tag
        size_t tag_end = html.find('>', tag_start);
        if (tag_end == string::npos) {
            // Malformed HTML - missing closing bracket
            break;
        }
        
        // Extract the tag name (everything between < and >)
        string tag = html.substr(tag_start + 1, tag_end - tag_start - 1);
        
        // Build the closing tag string we need to find
        string closing_tag = "</" + tag + ">";
        
        // Find where the closing tag is
        size_t content_end = html.find(closing_tag, tag_end);
        
        if (content_end != string::npos) {
            // Extract the content between opening and closing tags
            string content = html.substr(tag_end + 1, content_end - tag_end - 1);
            
            // Create an HTMLElement structure to store this
            HTMLElement elem;
            elem.tag = tag;
            elem.content = content;
            elements.push_back(elem);
            
            // Move position past the closing tag so we can find the next one
            pos = content_end + closing_tag.length();
        } else {
            // No closing tag found, skip past this tag
            pos = tag_end + 1;
        }
    }
    
    return elements;
}

// This renders HTML by inserting text with proper formatting tags
// It parses the HTML and applies different styles to different elements
void BrowserWindow::render_html(const string& html) {
    // Make sure we have a text buffer to work with
    if (!text_buffer) {
        cout << "ERROR: text_buffer is NULL!" << endl;
        return;
    }
    
    cout << "render_html called with HTML length: " << html.length() << endl;
    
    // Clear any existing content in the buffer
    gtk_text_buffer_set_text(text_buffer, "", -1);
    
    // Parse the HTML into individual elements
    vector<HTMLElement> elements = parse_html(html);
    
    cout << "Parsed " << elements.size() << " HTML elements" << endl;
    
    // Debug: print what we parsed
    for (size_t i = 0; i < elements.size(); i++) {
        cout << "Element " << i << ": tag=" << elements[i].tag 
             << ", content=" << elements[i].content << endl;
    }
    
    // Iterator for inserting text into the buffer
    GtkTextIter iter;
    
    // Insert each element with appropriate formatting
    for (size_t i = 0; i < elements.size(); i++) {
        const HTMLElement& elem = elements[i];
        
        cout << "Rendering element: " << elem.tag << endl;
        
        // Get the end iterator (where we'll insert the next text)
        gtk_text_buffer_get_end_iter(text_buffer, &iter);
        
        // Insert the text with the appropriate formatting tag
        if (elem.tag == "h1") {
            cout << "Inserting h1: " << elem.content << endl;
            // Insert as h1 - big bold heading
            gtk_text_buffer_insert_with_tags_by_name(text_buffer, &iter,
                                                     elem.content.c_str(), -1,
                                                     "h1", NULL);
            // Add newline after heading for spacing
            gtk_text_buffer_get_end_iter(text_buffer, &iter);
            gtk_text_buffer_insert(text_buffer, &iter, "\n", -1);
        } else if (elem.tag == "p") {
            cout << "Inserting p: " << elem.content << endl;
            // Insert as p - normal paragraph text
            gtk_text_buffer_insert_with_tags_by_name(text_buffer, &iter,
                                                     elem.content.c_str(), -1,
                                                     "p", NULL);
            // Add newline after paragraph for spacing
            gtk_text_buffer_get_end_iter(text_buffer, &iter);
            gtk_text_buffer_insert(text_buffer, &iter, "\n", -1);
        } else {
            cout << "Inserting unknown tag: " << elem.content << endl;
            // Unknown tag, just insert as plain text
            // This way we don't crash if there's a tag we don't recognize
            gtk_text_buffer_insert(text_buffer, &iter, elem.content.c_str(), -1);
            gtk_text_buffer_get_end_iter(text_buffer, &iter);
            gtk_text_buffer_insert(text_buffer, &iter, "\n", -1);
        }
    }
    
    cout << "Finished rendering HTML" << endl;
}

// This shows the window to the user
void BrowserWindow::show() {
    if (window) {
        // Show all widgets in the window
        gtk_widget_show_all(window);
        cout << "Window displayed" << endl;
    }
}

// This sets the HTML content and renders it
void BrowserWindow::set_html(const string& html) {
    // Call the render function to parse and display the HTML
    render_html(html);
}

// This sets the window title
void BrowserWindow::set_title(const string& t) {
    title = t;
    
    // If window already exists, update its title
    if (window) {
        gtk_window_set_title(GTK_WINDOW(window), title.c_str());
    }
}

// This is the main event loop for the browser window
// It just starts the GTK main loop
void BrowserWindow::run() {
    cout << "Starting GTK main loop..." << endl;
    cout << "Window is now open. Close the window to exit." << endl;
    
    // Start the GTK main loop
    // This function blocks until the window is closed
    // It handles all the events like mouse clicks, keyboard input, etc.
    gtk_main();
    
    cout << "GTK main loop ended" << endl;
}

int main(int argc, char* argv[]) {
    cout << "Starting Szymdows Browser..." << endl;
    cout << endl;
    
    // Initialize GTK
    // This must be called before using any GTK functions
    gtk_init(&argc, &argv);
    cout << "GTK initialized" << endl;
    
    // Default test HTML to display if no file is provided
    // This helps with testing during development
    string html_content = 
        "<h1>Welcome to Szymdows Browser</h1>"
        "<p>This is a simple web browser built from scratch using C++ and Rust.</p>"
        "<h1>Current Features</h1>"
        "<p>The browser now renders HTML with proper text formatting! Headings are large and bold, paragraphs are normal text.</p>"
        "<h1>Technical Details</h1>"
        "<p>This browser uses GTK for the user interface and custom HTML parsing to render content with different text sizes and styles.</p>";
    
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
    
    // Create the browser window
    cout << "Creating browser window..." << endl;
    BrowserWindow window;
    
    // Set up the UI (this creates all the GTK widgets)
    cout << "Setting up user interface..." << endl;
    window.setup_ui();
    
    // Parse and render the HTML content (do this AFTER UI setup)
    cout << "Parsing and rendering HTML..." << endl;
    window.set_html(html_content);
    
    // Show the window
    cout << "Showing window..." << endl;
    window.show();
    
    // Run the browser event loop
    window.run();
    
    cout << endl;
    cout << "Browser closed. Thanks for using Szymdows Browser!" << endl;
    
    return 0;
}