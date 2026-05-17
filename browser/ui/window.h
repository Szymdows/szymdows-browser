#ifndef WINDOW_H
#define WINDOW_H

#include <string>
#include <gtk/gtk.h>

class BrowserWindow {
public:
    BrowserWindow();
    ~BrowserWindow();
    
    void show();
    void set_content(const std::string& content);
    void run();
    void set_title(const std::string& title);
    
    GtkWidget* get_window() { return window; }
    
private:
    std::string content;
    std::string title;
    int width;
    int height;
    
    GtkWidget* window;
    GtkWidget* scrolled_window;
    GtkWidget* text_view;
    GtkTextBuffer* text_buffer;
    
    void setup_ui();
};

#endif