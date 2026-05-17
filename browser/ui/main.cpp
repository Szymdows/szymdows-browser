#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <gtk/gtk.h>
#include "window.h"
#include "../../engine/renderer/render.h"

using namespace std;

BrowserWindow::BrowserWindow() {
    width = 800;
    height = 600;
    content = "";
    title = "Szymdows Browser";
    
    window = NULL;
    scrolled_window = NULL;
    text_view = NULL;
    text_buffer = NULL;
    
    cout << "Browser window created" << endl;
}

BrowserWindow::~BrowserWindow() {
    cout << "Browser window destroyed" << endl;
}

void BrowserWindow::setup_ui() {
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), title.c_str());
    gtk_window_set_default_size(GTK_WINDOW(window), width, height);
    
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    
    scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
                                   GTK_POLICY_AUTOMATIC,
                                   GTK_POLICY_AUTOMATIC);
    
    text_view = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(text_view), FALSE);
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(text_view), FALSE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(text_view), GTK_WRAP_WORD);
    
    gtk_text_view_set_left_margin(GTK_TEXT_VIEW(text_view), 10);
    gtk_text_view_set_right_margin(GTK_TEXT_VIEW(text_view), 10);
    gtk_text_view_set_top_margin(GTK_TEXT_VIEW(text_view), 10);
    gtk_text_view_set_bottom_margin(GTK_TEXT_VIEW(text_view), 10);
    
    text_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
    
    if (!content.empty()) {
        gtk_text_buffer_set_text(text_buffer, content.c_str(), -1);
    }
    
    gtk_container_add(GTK_CONTAINER(scrolled_window), text_view);
    gtk_container_add(GTK_CONTAINER(window), scrolled_window);
    
    GtkCssProvider* css_provider = gtk_css_provider_new();
    const gchar* css_data = "textview { background-color: white; color: black; font-family: monospace; font-size: 12pt; }";
    gtk_css_provider_load_from_data(css_provider, css_data, -1, NULL);
    
    GtkStyleContext* context = gtk_widget_get_style_context(text_view);
    gtk_style_context_add_provider(context,
                                   GTK_STYLE_PROVIDER(css_provider),
                                   GTK_STYLE_PROVIDER_PRIORITY_USER);
    
    cout << "UI setup complete" << endl;
}

void BrowserWindow::show() {
    if (window) {
        gtk_widget_show_all(window);
        cout << "Window displayed" << endl;
    }
}

void BrowserWindow::set_content(const string& c) {
    content = c;
    
    if (text_buffer) {
        gtk_text_buffer_set_text(text_buffer, content.c_str(), -1);
    }
}

void BrowserWindow::set_title(const string& t) {
    title = t;
    
    if (window) {
        gtk_window_set_title(GTK_WINDOW(window), title.c_str());
    }
}

void BrowserWindow::run() {
    cout << "Setting up user interface..." << endl;
    setup_ui();
    
    cout << "Showing window..." << endl;
    show();
    
    cout << "Starting GTK main loop..." << endl;
    cout << "Window is now open. Close the window to exit." << endl;
    
    gtk_main();
    
    cout << "GTK main loop ended" << endl;
}

int main(int argc, char* argv[]) {
    cout << "Starting Szymdows Browser..." << endl;
    cout << endl;
    
    gtk_init(&argc, &argv);
    cout << "GTK initialized" << endl;
    
    string html_content = 
        "<h1>Welcome to Szymdows Browser</h1>"
        "<p>This is a simple web browser built from scratch using C++ and Rust.</p>"
        "<h1>Current Features</h1>"
        "<p>Right now the browser can render h1 and p tags in a graphical window!</p>"
        "<h1>New in this version</h1>"
        "<p>The browser now has a real graphical user interface using GTK. You can see the rendered content in an actual window instead of just the terminal.</p>";
    
    if (argc > 1) {
        string filename = argv[1];
        cout << "Loading HTML file: " << filename << endl;
        
        ifstream file(filename);
        if (file.is_open()) {
            stringstream buffer;
            buffer << file.rdbuf();
            html_content = buffer.str();
            file.close();
            
            cout << "File loaded successfully" << endl;
        } else {
            cerr << "Error: Could not open file '" << filename << "'" << endl;
            cerr << "Using default test content instead" << endl;
        }
        cout << endl;
    }
    
    cout << "Initializing renderer..." << endl;
    Renderer renderer;
    
    cout << "Rendering HTML..." << endl;
    renderer.render(html_content);
    
    string rendered_output = renderer.get_output();
    cout << "Rendering complete" << endl;
    cout << endl;
    
    cout << "Creating browser window..." << endl;
    BrowserWindow window;
    
    window.set_content(rendered_output);
    
    window.run();
    
    cout << endl;
    cout << "Browser closed. Thanks for using Szymdows Browser!" << endl;
    
    return 0;
}