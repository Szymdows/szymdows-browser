#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <gtk/gtk.h>
#include "window.h"
#include "../../engine/parser/parser_ffi.h"

using namespace std;

// Browser Window Implementation with GTK
// Now includes a proper browser toolbar with navigation buttons and URL bar!

BrowserWindow::BrowserWindow() {
    width = 1000;  // Made it a bit wider for the toolbar
    height = 700;
    title = "Szymdows Browser";
    current_url = "";
    
    // Initialize all widgets to NULL
    window = NULL;
    main_vbox = NULL;
    toolbar = NULL;
    back_button = NULL;
    forward_button = NULL;
    refresh_button = NULL;
    url_entry = NULL;
    scrolled_window = NULL;
    text_view = NULL;
    text_buffer = NULL;
    
    cout << "Browser window created" << endl;
}

BrowserWindow::~BrowserWindow() {
    cout << "Browser window destroyed" << endl;
}

void BrowserWindow::initialize_tag_styles() {
    tag_styles["h1"] = {2.0, PANGO_WEIGHT_BOLD, "#000000", 15, 10};
    tag_styles["h2"] = {1.5, PANGO_WEIGHT_BOLD, "#1a1a1a", 12, 8};
    tag_styles["h3"] = {1.3, PANGO_WEIGHT_BOLD, "#1a1a1a", 10, 6};
    tag_styles["h4"] = {1.2, PANGO_WEIGHT_BOLD, "#2a2a2a", 8, 5};
    tag_styles["h5"] = {1.1, PANGO_WEIGHT_BOLD, "#2a2a2a", 6, 4};
    tag_styles["h6"] = {1.0, PANGO_WEIGHT_BOLD, "#2a2a2a", 5, 3};
    tag_styles["p"] = {1.0, PANGO_WEIGHT_NORMAL, "#333333", 0, 12};
    
    cout << "Initialized " << tag_styles.size() << " tag styles" << endl;
}

void BrowserWindow::create_text_tag(const string& tag_name, const TagStyle& style) {
    gtk_text_buffer_create_tag(text_buffer, tag_name.c_str(),
                              "weight", style.weight,
                              "scale", style.scale,
                              "foreground", style.color.c_str(),
                              "pixels-above-lines", style.pixels_above,
                              "pixels-below-lines", style.pixels_below,
                              NULL);
}

// This sets the window icon based on the platform
// Uses .ico on Windows and .png on Linux
void BrowserWindow::set_window_icon() {
    GError* error = NULL;
    
    #ifdef _WIN32
        // On Windows, use the .ico file
        const char* icon_path = "browser/assets/branding/icon.ico";
    #else
        // On Linux, use the .png file
        const char* icon_path = "browser/assets/branding/icon.png";
    #endif
    
    // Try to load the icon
    GdkPixbuf* icon = gdk_pixbuf_new_from_file(icon_path, &error);
    
    if (error) {
        // Failed to load icon
        cerr << "Failed to load window icon from " << icon_path << ": " 
             << error->message << endl;
        g_error_free(error);
    } else {
        // Successfully loaded, set it as the window icon
        gtk_window_set_icon(GTK_WINDOW(window), icon);
        cout << "Window icon set from: " << icon_path << endl;
        
        // Free the pixbuf after setting (GTK keeps its own copy)
        g_object_unref(icon);
    }
}

// This creates the browser toolbar with navigation buttons and URL bar
void BrowserWindow::create_toolbar() {
    // Create a horizontal box for the toolbar
    toolbar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_widget_set_margin_start(toolbar, 5);
    gtk_widget_set_margin_end(toolbar, 5);
    gtk_widget_set_margin_top(toolbar, 5);
    gtk_widget_set_margin_bottom(toolbar, 5);
    
    // Create the back button with icon
    back_button = gtk_button_new();
    GtkWidget* back_image = gtk_image_new_from_file("browser/assets/ui/arrow_back.svg");
    if (back_image) {
        gtk_button_set_image(GTK_BUTTON(back_button), back_image);
    } else {
        // Fallback to text if icon doesn't load
        gtk_button_set_label(GTK_BUTTON(back_button), "←");
    }
    gtk_widget_set_tooltip_text(back_button, "Go back");
    g_signal_connect(back_button, "clicked", G_CALLBACK(on_back_clicked), this);
    gtk_box_pack_start(GTK_BOX(toolbar), back_button, FALSE, FALSE, 0);
    
    // Create the forward button with icon
    forward_button = gtk_button_new();
    GtkWidget* forward_image = gtk_image_new_from_file("browser/assets/ui/arrow_forward.svg");
    if (forward_image) {
        gtk_button_set_image(GTK_BUTTON(forward_button), forward_image);
    } else {
        // Fallback to text if icon doesn't load
        gtk_button_set_label(GTK_BUTTON(forward_button), "→");
    }
    gtk_widget_set_tooltip_text(forward_button, "Go forward");
    g_signal_connect(forward_button, "clicked", G_CALLBACK(on_forward_clicked), this);
    gtk_box_pack_start(GTK_BOX(toolbar), forward_button, FALSE, FALSE, 0);
    
    // Create the refresh button with icon
    refresh_button = gtk_button_new();
    GtkWidget* refresh_image = gtk_image_new_from_file("browser/assets/ui/refresh.svg");
    if (refresh_image) {
        gtk_button_set_image(GTK_BUTTON(refresh_button), refresh_image);
    } else {
        // Fallback to text if icon doesn't load
        gtk_button_set_label(GTK_BUTTON(refresh_button), "⟳");
    }
    gtk_widget_set_tooltip_text(refresh_button, "Refresh");
    g_signal_connect(refresh_button, "clicked", G_CALLBACK(on_refresh_clicked), this);
    gtk_box_pack_start(GTK_BOX(toolbar), refresh_button, FALSE, FALSE, 0);
    
    // Create the URL entry (address bar / search bar)
    url_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(url_entry), "Enter URL or search...");
    g_signal_connect(url_entry, "activate", G_CALLBACK(on_url_activate), this);
    
    // Make the URL bar expand to fill remaining space
    gtk_box_pack_start(GTK_BOX(toolbar), url_entry, TRUE, TRUE, 0);
    
    // Style the toolbar with a light background
    GtkCssProvider* toolbar_css = gtk_css_provider_new();
    const gchar* toolbar_style = 
        "box { background-color: #f0f0f0; border-bottom: 1px solid #cccccc; }";
    gtk_css_provider_load_from_data(toolbar_css, toolbar_style, -1, NULL);
    
    GtkStyleContext* toolbar_context = gtk_widget_get_style_context(toolbar);
    gtk_style_context_add_provider(toolbar_context,
                                   GTK_STYLE_PROVIDER(toolbar_css),
                                   GTK_STYLE_PROVIDER_PRIORITY_USER);
    
    cout << "Toolbar created with navigation buttons and URL bar" << endl;
}

// Callback for back button click
void BrowserWindow::on_back_clicked(GtkWidget* widget, gpointer data) {
    BrowserWindow* window = static_cast<BrowserWindow*>(data);
    cout << "Back button clicked (not implemented yet)" << endl;
    // TODO: Implement navigation history
}

// Callback for forward button click
void BrowserWindow::on_forward_clicked(GtkWidget* widget, gpointer data) {
    BrowserWindow* window = static_cast<BrowserWindow*>(data);
    cout << "Forward button clicked (not implemented yet)" << endl;
    // TODO: Implement navigation history
}

// Callback for refresh button click
void BrowserWindow::on_refresh_clicked(GtkWidget* widget, gpointer data) {
    BrowserWindow* window = static_cast<BrowserWindow*>(data);
    cout << "Refresh button clicked (not implemented yet)" << endl;
    // TODO: Reload current page
}

// Callback when user presses Enter in the URL bar
void BrowserWindow::on_url_activate(GtkEntry* entry, gpointer data) {
    BrowserWindow* window = static_cast<BrowserWindow*>(data);
    
    // Get the text from the URL bar
    const gchar* url_text = gtk_entry_get_text(entry);
    window->current_url = string(url_text);
    
    cout << "URL entered: " << window->current_url << endl;
    cout << "(Navigation not implemented yet - just displaying what was typed)" << endl;
    
    // TODO: Actually load the URL or perform search
    // For now, just print it
}

void BrowserWindow::setup_ui() {
    // Create the main window
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), title.c_str());
    gtk_window_set_default_size(GTK_WINDOW(window), width, height);
    
    // Set the window icon
    set_window_icon();
    
    // Connect the destroy signal to quit the application
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    
    // Create a vertical box to hold toolbar and content area
    main_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(window), main_vbox);
    
    // Create and add the toolbar
    create_toolbar();
    gtk_box_pack_start(GTK_BOX(main_vbox), toolbar, FALSE, FALSE, 0);
    
    // Create a scrolled window for the content
    scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
                                   GTK_POLICY_AUTOMATIC,
                                   GTK_POLICY_AUTOMATIC);
    
    // Add the scrolled window to the vbox (it will expand to fill space)
    gtk_box_pack_start(GTK_BOX(main_vbox), scrolled_window, TRUE, TRUE, 0);
    
    // Create a text view widget to display the rendered content
    text_view = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(text_view), FALSE);
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(text_view), FALSE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(text_view), GTK_WRAP_WORD);
    
    // Set margins for the text view
    gtk_text_view_set_left_margin(GTK_TEXT_VIEW(text_view), 20);
    gtk_text_view_set_right_margin(GTK_TEXT_VIEW(text_view), 20);
    gtk_text_view_set_top_margin(GTK_TEXT_VIEW(text_view), 20);
    gtk_text_view_set_bottom_margin(GTK_TEXT_VIEW(text_view), 20);
    
    // Get the text buffer
    text_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
    
    // Initialize tag styles
    initialize_tag_styles();
    
    // Create GTK text tags for styling
    for (auto& pair : tag_styles) {
        const string& tag_name = pair.first;
        const TagStyle& style = pair.second;
        create_text_tag(tag_name, style);
        cout << "Created text tag for: " << tag_name << endl;
    }
    
    // Add the text view to the scrolled window
    gtk_container_add(GTK_CONTAINER(scrolled_window), text_view);
    
    // Style the text view
    GtkCssProvider* css_provider = gtk_css_provider_new();
    const gchar* css_data = "textview { background-color: white; font-family: 'Sans'; font-size: 14px; }";
    gtk_css_provider_load_from_data(css_provider, css_data, -1, NULL);
    
    GtkStyleContext* context = gtk_widget_get_style_context(text_view);
    gtk_style_context_add_provider(context,
                                   GTK_STYLE_PROVIDER(css_provider),
                                   GTK_STYLE_PROVIDER_PRIORITY_USER);
    
    cout << "UI setup complete with toolbar" << endl;
}

// This now uses the Rust parser via FFI
void BrowserWindow::render_html(const string& html) {
    if (!text_buffer) {
        cout << "ERROR: text_buffer is NULL!" << endl;
        return;
    }
    
    cout << "[C++] render_html called with HTML length: " << html.length() << endl;
    
    gtk_text_buffer_set_text(text_buffer, "", -1);
    
    // Call the Rust parser via FFI
    cout << "[C++] Calling Rust parser..." << endl;
    NodeArray* node_array = parse_html_to_nodes(
        reinterpret_cast<const uint8_t*>(html.c_str()),
        html.length()
    );
    
    if (node_array == nullptr) {
        cout << "[C++] ERROR: Rust parser returned NULL!" << endl;
        return;
    }
    
    size_t node_count = get_node_count(node_array);
    cout << "[C++] Received " << node_count << " nodes from Rust parser" << endl;
    
    GtkTextIter iter;
    
    for (size_t i = 0; i < node_count; i++) {
        const uint8_t* tag_ptr = get_node_tag(node_array, i);
        size_t tag_len = get_node_tag_len(node_array, i);
        string tag(reinterpret_cast<const char*>(tag_ptr), tag_len);
        
        const uint8_t* content_ptr = get_node_content(node_array, i);
        size_t content_len = get_node_content_len(node_array, i);
        string content(reinterpret_cast<const char*>(content_ptr), content_len);
        
        bool is_text = get_node_is_text(node_array, i);
        
        gtk_text_buffer_get_end_iter(text_buffer, &iter);
        
        if (!is_text && !tag.empty()) {
            auto style_it = tag_styles.find(tag);
            
            if (style_it != tag_styles.end()) {
                gtk_text_buffer_insert_with_tags_by_name(text_buffer, &iter,
                                                         content.c_str(), -1,
                                                         tag.c_str(), NULL);
            } else {
                gtk_text_buffer_insert(text_buffer, &iter, content.c_str(), -1);
            }
            
            gtk_text_buffer_get_end_iter(text_buffer, &iter);
            gtk_text_buffer_insert(text_buffer, &iter, "\n", -1);
        } else if (is_text && !content.empty()) {
            gtk_text_buffer_insert(text_buffer, &iter, content.c_str(), -1);
        }
    }
    
    // Free the Rust memory
    free_node_array(node_array);
    
    cout << "[C++] Finished rendering HTML" << endl;
}

void BrowserWindow::show() {
    if (window) {
        gtk_widget_show_all(window);
        cout << "Window displayed" << endl;
    }
}

void BrowserWindow::set_html(const string& html) {
    render_html(html);
}

void BrowserWindow::set_title(const string& t) {
    title = t;
    
    if (window) {
        gtk_window_set_title(GTK_WINDOW(window), title.c_str());
    }
}

void BrowserWindow::run() {
    cout << "Starting GTK main loop..." << endl;
    cout << "Window is now open. Close the window to exit." << endl;
    
    gtk_main();
    
    cout << "GTK main loop ended" << endl;
}

int main(int argc, char* argv[]) {
    cout << "Starting Szymdows Browser..." << endl;
    cout << "Now with a proper browser toolbar!" << endl;
    cout << endl;
    
    gtk_init(&argc, &argv);
    cout << "GTK initialized" << endl;
    
    string html_content = 
        "<h1>Welcome to Szymdows Browser</h1>"
        "<p>This browser now has a proper toolbar with navigation buttons and a URL bar!</p>"
        "<h2>New Features</h2>"
        "<p>Look at the top of the window - you'll see back, forward, and refresh buttons, plus a URL/search bar.</p>"
        "<h3>Try It Out</h3>"
        "<p>Type something in the URL bar and press Enter. For now it just prints to the console, but I'll make it work soon!</p>"
        "<h2>Architecture</h2>"
        "<p>Still using Rust for parsing and C++ for rendering. Now with a real browser interface!</p>";
    
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
    
    cout << "Creating browser window..." << endl;
    BrowserWindow window;
    
    cout << "Setting up user interface..." << endl;
    window.setup_ui();
    
    cout << "Parsing and rendering HTML (using Rust parser)..." << endl;
    window.set_html(html_content);
    
    cout << "Showing window..." << endl;
    window.show();
    
    window.run();
    
    cout << endl;
    cout << "Browser closed. Thanks for using Szymdows Browser!" << endl;
    
    return 0;
}