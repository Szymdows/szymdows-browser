#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <gtk/gtk.h>
#include "window.h"
#include "../../engine/parser/parser_ffi.h"  // Include Rust FFI header

using namespace std;

// Browser Window Implementation with GTK
// Now uses the Rust parser via FFI instead of parsing HTML in C++!

BrowserWindow::BrowserWindow() {
    width = 800;
    height = 600;
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
    
    gtk_text_view_set_left_margin(GTK_TEXT_VIEW(text_view), 20);
    gtk_text_view_set_right_margin(GTK_TEXT_VIEW(text_view), 20);
    gtk_text_view_set_top_margin(GTK_TEXT_VIEW(text_view), 20);
    gtk_text_view_set_bottom_margin(GTK_TEXT_VIEW(text_view), 20);
    
    text_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
    
    initialize_tag_styles();
    
    for (auto& pair : tag_styles) {
        const string& tag_name = pair.first;
        const TagStyle& style = pair.second;
        create_text_tag(tag_name, style);
        cout << "Created text tag for: " << tag_name << endl;
    }
    
    gtk_container_add(GTK_CONTAINER(scrolled_window), text_view);
    gtk_container_add(GTK_CONTAINER(window), scrolled_window);
    
    GtkCssProvider* css_provider = gtk_css_provider_new();
    const gchar* css_data = "textview { background-color: white; font-family: 'Sans'; font-size: 14px; }";
    gtk_css_provider_load_from_data(css_provider, css_data, -1, NULL);
    
    GtkStyleContext* context = gtk_widget_get_style_context(text_view);
    gtk_style_context_add_provider(context,
                                   GTK_STYLE_PROVIDER(css_provider),
                                   GTK_STYLE_PROVIDER_PRIORITY_USER);
    
    cout << "UI setup complete" << endl;
}

// This now uses the Rust parser via FFI instead of parsing in C++!
void BrowserWindow::render_html(const string& html) {
    if (!text_buffer) {
        cout << "ERROR: text_buffer is NULL!" << endl;
        return;
    }
    
    cout << "[C++] render_html called with HTML length: " << html.length() << endl;
    
    gtk_text_buffer_set_text(text_buffer, "", -1);
    
    // Call the Rust parser via FFI!
    cout << "[C++] Calling Rust parser..." << endl;
    NodeArray* node_array = parse_html_to_nodes(
        reinterpret_cast<const uint8_t*>(html.c_str()),
        html.length()
    );
    
    if (node_array == nullptr) {
        cout << "[C++] ERROR: Rust parser returned NULL!" << endl;
        return;
    }
    
    // Get the number of nodes from Rust
    size_t node_count = get_node_count(node_array);
    cout << "[C++] Received " << node_count << " nodes from Rust parser" << endl;
    
    // Iterate through all the nodes
    GtkTextIter iter;
    
    for (size_t i = 0; i < node_count; i++) {
        // Get tag name from Rust
        const uint8_t* tag_ptr = get_node_tag(node_array, i);
        size_t tag_len = get_node_tag_len(node_array, i);
        string tag(reinterpret_cast<const char*>(tag_ptr), tag_len);
        
        // Get content from Rust
        const uint8_t* content_ptr = get_node_content(node_array, i);
        size_t content_len = get_node_content_len(node_array, i);
        string content(reinterpret_cast<const char*>(content_ptr), content_len);
        
        bool is_text = get_node_is_text(node_array, i);
        
        cout << "[C++] Node " << i << ": tag='" << tag << "', content='" << content 
             << "', is_text=" << is_text << endl;
        
        gtk_text_buffer_get_end_iter(text_buffer, &iter);
        
        if (!is_text && !tag.empty()) {
            // It's an element node
            auto style_it = tag_styles.find(tag);
            
            if (style_it != tag_styles.end()) {
                cout << "[C++] Rendering " << tag << " with styling" << endl;
                gtk_text_buffer_insert_with_tags_by_name(text_buffer, &iter,
                                                         content.c_str(), -1,
                                                         tag.c_str(), NULL);
            } else {
                cout << "[C++] Unknown tag '" << tag << "', rendering as plain text" << endl;
                gtk_text_buffer_insert(text_buffer, &iter, content.c_str(), -1);
            }
            
            gtk_text_buffer_get_end_iter(text_buffer, &iter);
            gtk_text_buffer_insert(text_buffer, &iter, "\n", -1);
        } else if (is_text && !content.empty()) {
            // It's a text node
            gtk_text_buffer_insert(text_buffer, &iter, content.c_str(), -1);
        }
    }
    
    // IMPORTANT: Free the memory allocated by Rust!
    cout << "[C++] Freeing Rust node array..." << endl;
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
    cout << "Now using Rust parser via FFI!" << endl;
    cout << endl;
    
    gtk_init(&argc, &argv);
    cout << "GTK initialized" << endl;
    
    string html_content = 
        "<h1>Welcome to Szymdows Browser</h1>"
        "<p>This browser now uses the Rust HTML parser via FFI (Foreign Function Interface)!</p>"
        "<h2>How it works</h2>"
        "<p>The HTML is sent to Rust, parsed there, and the results are sent back to C++ for rendering.</p>"
        "<h3>Architecture</h3>"
        "<p>Rust handles parsing (memory safe), C++ handles rendering (GTK integration).</p>"
        "<h4>This is proper browser architecture</h4>"
        "<p>Just like Firefox uses both Rust and C++!</p>";
    
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