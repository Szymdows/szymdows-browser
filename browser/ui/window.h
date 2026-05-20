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
    
    GtkWidget* window;
    GtkWidget* scrolled_window;
    GtkWidget* text_view;
    GtkTextBuffer* text_buffer;
    
    std::map<std::string, TagStyle> tag_styles;
    
    void initialize_tag_styles();
    void render_html(const std::string& html);
    
    void create_text_tag(const std::string& tag_name, const TagStyle& style);
};

#endif