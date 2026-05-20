#ifndef WINDOW_H
#define WINDOW_H

#include <string>
#include <vector>
#include <map>
#include <gtk/gtk.h>

// Structure to hold parsed HTML elements
// This stores information about each HTML tag we parse
struct HTMLElement {
    std::string tag;       // The tag name like "h1" or "p"
    std::string content;   // The text inside the tag
};

// Structure to define how each tag should be styled
struct TagStyle {
    double scale;
    int weight;
    std::string color;
    int pixels_above;
    int pixels_below;
};

class BrowserWindow {
public:
    BrowserWindow();
    ~BrowserWindow();
    
    void show();
    void set_html(const std::string& html);
    void run();
    void set_title(const std::string& title);
    void setup_ui();
    
    GtkWidget* get_window() { return window; }
    
private:
    std::string title;
    int width;
    int height;
    std::string current_url;  // Store the current URL/location
    
    // GTK widgets for the main window
    GtkWidget* window;
    GtkWidget* main_vbox;          // Vertical box to hold toolbar and content
    GtkWidget* toolbar;            // Toolbar at the top
    GtkWidget* back_button;        // Back navigation button
    GtkWidget* forward_button;     // Forward navigation button
    GtkWidget* refresh_button;     // Refresh button
    GtkWidget* url_entry;          // URL/search bar
    GtkWidget* scrolled_window;    // Scrollable area for content
    GtkWidget* text_view;          // Text display widget
    GtkTextBuffer* text_buffer;    // Buffer that holds the actual text
    
    std::map<std::string, TagStyle> tag_styles;
    
    void initialize_tag_styles();
    void render_html(const std::string& html);
    void create_text_tag(const std::string& tag_name, const TagStyle& style);
    
    // New functions for the toolbar
    void create_toolbar();
    void set_window_icon();
    
    // Static callback functions for button clicks
    // These need to be static so GTK can call them
    static void on_back_clicked(GtkWidget* widget, gpointer data);
    static void on_forward_clicked(GtkWidget* widget, gpointer data);
    static void on_refresh_clicked(GtkWidget* widget, gpointer data);
    static void on_url_activate(GtkEntry* entry, gpointer data);
};

#endif