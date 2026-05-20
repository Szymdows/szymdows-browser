#ifndef WINDOW_H
#define WINDOW_H

#include <string>
#include <vector>
#include <map>
#include <deque>
#include <gtk/gtk.h>

// Structure to hold parsed HTML elements
struct HTMLElement {
    std::string tag;
    std::string content;
};

// Structure to define how each tag should be styled
struct TagStyle {
    double scale;
    int weight;
    std::string color;
    int pixels_above;
    int pixels_below;
};

// Structure to represent a page in navigation history
struct HistoryEntry {
    std::string url;
    std::string html_content;
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
    
    // New navigation methods
    void navigate_to(const std::string& url);
    void go_back();
    void go_forward();
    void refresh();
    
    GtkWidget* get_window() { return window; }
    
private:
    std::string title;
    int width;
    int height;
    std::string current_url;
    
    // Navigation history
    std::deque<HistoryEntry> history;  // Stores all visited pages
    int history_position;  // Current position in history (-1 means no history)
    
    // GTK widgets
    GtkWidget* window;
    GtkWidget* main_vbox;
    GtkWidget* toolbar;
    GtkWidget* back_button;
    GtkWidget* forward_button;
    GtkWidget* refresh_button;
    GtkWidget* url_entry;
    GtkWidget* scrolled_window;
    GtkWidget* text_view;
    GtkTextBuffer* text_buffer;
    
    std::map<std::string, TagStyle> tag_styles;
    
    void initialize_tag_styles();
    void render_html(const std::string& html);
    void create_text_tag(const std::string& tag_name, const TagStyle& style);
    void create_toolbar();
    void set_window_icon();
    void update_url_bar();
    void update_navigation_buttons();
    
    // HTTP fetching
    std::string fetch_url(const std::string& url);
    std::string create_search_url(const std::string& query);
    bool is_url(const std::string& text);
    
    // Static callback functions
    static void on_back_clicked(GtkWidget* widget, gpointer data);
    static void on_forward_clicked(GtkWidget* widget, gpointer data);
    static void on_refresh_clicked(GtkWidget* widget, gpointer data);
    static void on_url_activate(GtkEntry* entry, gpointer data);
};

#endif