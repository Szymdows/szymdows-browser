#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <deque>
#include <algorithm>
#include <curl/curl.h>
#include <gtk/gtk.h>
#include "window.h"
#include "../../engine/parser/parser_ffi.h"

using namespace std;

// Browser Window Implementation with actual web browsing!
// Now fetches real web pages and handles navigation

// Callback function for libcurl to write fetched data
// This gets called by curl when it receives data from the server
static size_t write_callback(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t total_size = size * nmemb;
    string* response = static_cast<string*>(userp);
    response->append(static_cast<char*>(contents), total_size);
    return total_size;
}

BrowserWindow::BrowserWindow() {
    width = 1000;
    height = 700;
    title = "Szymdows Browser";
    current_url = "";
    history_position = -1;  // No history yet
    
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

void BrowserWindow::set_window_icon() {
    GError* error = NULL;
    
    #ifdef _WIN32
        const char* icon_path = "browser/assets/branding/icon.ico";
    #else
        const char* icon_path = "browser/assets/branding/icon.png";
    #endif
    
    GdkPixbuf* icon = gdk_pixbuf_new_from_file(icon_path, &error);
    
    if (error) {
        cerr << "Failed to load window icon from " << icon_path << ": " 
             << error->message << endl;
        g_error_free(error);
    } else {
        gtk_window_set_icon(GTK_WINDOW(window), icon);
        cout << "Window icon set from: " << icon_path << endl;
        g_object_unref(icon);
    }
}

void BrowserWindow::create_toolbar() {
    toolbar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_widget_set_margin_start(toolbar, 5);
    gtk_widget_set_margin_end(toolbar, 5);
    gtk_widget_set_margin_top(toolbar, 5);
    gtk_widget_set_margin_bottom(toolbar, 5);
    
    // Back button
    back_button = gtk_button_new();
    GtkWidget* back_image = gtk_image_new_from_file("browser/assets/ui/arrow_back.svg");
    if (back_image) {
        gtk_button_set_image(GTK_BUTTON(back_button), back_image);
    } else {
        gtk_button_set_label(GTK_BUTTON(back_button), "←");
    }
    gtk_widget_set_tooltip_text(back_button, "Go back");
    gtk_widget_set_sensitive(back_button, FALSE);  // Disabled by default
    g_signal_connect(back_button, "clicked", G_CALLBACK(on_back_clicked), this);
    gtk_box_pack_start(GTK_BOX(toolbar), back_button, FALSE, FALSE, 0);
    
    // Forward button
    forward_button = gtk_button_new();
    GtkWidget* forward_image = gtk_image_new_from_file("browser/assets/ui/arrow_forward.svg");
    if (forward_image) {
        gtk_button_set_image(GTK_BUTTON(forward_button), forward_image);
    } else {
        gtk_button_set_label(GTK_BUTTON(forward_button), "→");
    }
    gtk_widget_set_tooltip_text(forward_button, "Go forward");
    gtk_widget_set_sensitive(forward_button, FALSE);  // Disabled by default
    g_signal_connect(forward_button, "clicked", G_CALLBACK(on_forward_clicked), this);
    gtk_box_pack_start(GTK_BOX(toolbar), forward_button, FALSE, FALSE, 0);
    
    // Refresh button
    refresh_button = gtk_button_new();
    GtkWidget* refresh_image = gtk_image_new_from_file("browser/assets/ui/refresh.svg");
    if (refresh_image) {
        gtk_button_set_image(GTK_BUTTON(refresh_button), refresh_image);
    } else {
        gtk_button_set_label(GTK_BUTTON(refresh_button), "⟳");
    }
    gtk_widget_set_tooltip_text(refresh_button, "Refresh");
    gtk_widget_set_sensitive(refresh_button, FALSE);  // Disabled until it has a page
    g_signal_connect(refresh_button, "clicked", G_CALLBACK(on_refresh_clicked), this);
    gtk_box_pack_start(GTK_BOX(toolbar), refresh_button, FALSE, FALSE, 0);
    
    // URL entry
    url_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(url_entry), "Enter URL or search...");
    g_signal_connect(url_entry, "activate", G_CALLBACK(on_url_activate), this);
    gtk_box_pack_start(GTK_BOX(toolbar), url_entry, TRUE, TRUE, 0);
    
    // Style the toolbar
    GtkCssProvider* toolbar_css = gtk_css_provider_new();
    const gchar* toolbar_style = 
        "box { background-color: #f0f0f0; border-bottom: 1px solid #cccccc; }";
    gtk_css_provider_load_from_data(toolbar_css, toolbar_style, -1, NULL);
    
    GtkStyleContext* toolbar_context = gtk_widget_get_style_context(toolbar);
    gtk_style_context_add_provider(toolbar_context,
                                   GTK_STYLE_PROVIDER(toolbar_css),
                                   GTK_STYLE_PROVIDER_PRIORITY_USER);
    
    cout << "Toolbar created" << endl;
}

// This checks if the entered text looks like a URL or a search query
bool BrowserWindow::is_url(const string& text) {
    // Simple check: if it contains a dot and no spaces, treat it as URL
    // Also check for common URL prefixes
    if (text.find("http://") == 0 || text.find("https://") == 0) {
        return true;
    }
    
    // If it has a dot and no spaces, probably a URL
    if (text.find('.') != string::npos && text.find(' ') == string::npos) {
        return true;
    }
    
    return false;
}

// This creates a Google search URL from a search query
string BrowserWindow::create_search_url(const string& query) {
    // It needs to URL-encode the query
    // For simplicity, just replace spaces with +
    string encoded_query = query;
    replace(encoded_query.begin(), encoded_query.end(), ' ', '+');
    
    return "https://www.google.com/search?q=" + encoded_query;
}

// This fetches HTML content from a URL using libcurl
string BrowserWindow::fetch_url(const string& url) {
    cout << "[Fetch] Fetching URL: " << url << endl;
    
    CURL* curl;
    CURLcode res;
    string response;
    
    // Initialize curl
    curl = curl_easy_init();
    
    if (!curl) {
        cerr << "[Fetch] ERROR: Failed to initialize curl" << endl;
        return "";
    }
    
    // Set curl options
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);  // Follow redirects
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "Szymdows Browser/0.1");  // Set user agent
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);  // Verify SSL certificates
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);  // 30 second timeout
    
    // Perform the request
    res = curl_easy_perform(curl);
    
    if (res != CURLE_OK) {
        cerr << "[Fetch] ERROR: curl_easy_perform() failed: " 
             << curl_easy_strerror(res) << endl;
        
        // Create error page HTML
        response = "<h1>Failed to Load Page</h1>"
                   "<p>Could not fetch the URL: " + url + "</p>"
                   "<p>Error: " + string(curl_easy_strerror(res)) + "</p>";
    } else {
        long http_code = 0;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
        cout << "[Fetch] HTTP response code: " << http_code << endl;
        cout << "[Fetch] Received " << response.length() << " bytes" << endl;
    }
    
    // Cleanup
    curl_easy_cleanup(curl);
    
    return response;
}

// This updates the URL bar to show the current URL
void BrowserWindow::update_url_bar() {
    if (url_entry && !current_url.empty()) {
        gtk_entry_set_text(GTK_ENTRY(url_entry), current_url.c_str());
    }
}

// This updates the enabled/disabled state of navigation buttons
void BrowserWindow::update_navigation_buttons() {
    // Enable back button if it's not at the start of history
    if (back_button) {
        gtk_widget_set_sensitive(back_button, history_position > 0);
    }
    
    // Enable forward button if it's not at the end of history
    if (forward_button) {
        gtk_widget_set_sensitive(forward_button, 
                                 history_position >= 0 && 
                                 history_position < (int)history.size() - 1);
    }
    
    // Enable refresh if it have a current page
    if (refresh_button) {
        gtk_widget_set_sensitive(refresh_button, !current_url.empty());
    }
}

// This navigates to a URL or performs a search
void BrowserWindow::navigate_to(const string& input) {
    string url;
    
    // Check if input is a URL or search query
    if (is_url(input)) {
        // It's a URL
        // Add http:// if no protocol specified
        if (input.find("://") == string::npos) {
            url = "http://" + input;
        } else {
            url = input;
        }
    } else {
        // It's a search query
        url = create_search_url(input);
    }
    
    cout << "[Navigate] Navigating to: " << url << endl;
    
    // Fetch the HTML content
    string html = fetch_url(url);
    
    if (html.empty()) {
        cout << "[Navigate] ERROR: No content fetched" << endl;
        return;
    }
    
    // Update current URL
    current_url = url;
    
    // If we navigated from the middle of history, remove forward history
    if (history_position >= 0 && history_position < (int)history.size() - 1) {
        history.erase(history.begin() + history_position + 1, history.end());
    }
    
    // Add to history
    HistoryEntry entry;
    entry.url = url;
    entry.html_content = html;
    history.push_back(entry);
    history_position = history.size() - 1;
    
    cout << "[Navigate] Added to history. Position: " << history_position 
         << ", Total: " << history.size() << endl;
    
    // Update UI
    update_url_bar();
    update_navigation_buttons();
    
    // Render the HTML
    render_html(html);
}

// Go back in history
void BrowserWindow::go_back() {
    if (history_position <= 0) {
        cout << "[Navigate] Cannot go back - at start of history" << endl;
        return;
    }
    
    history_position--;
    
    const HistoryEntry& entry = history[history_position];
    current_url = entry.url;
    
    cout << "[Navigate] Going back to: " << current_url 
         << " (position " << history_position << ")" << endl;
    
    update_url_bar();
    update_navigation_buttons();
    render_html(entry.html_content);
}

// Go forward in history
void BrowserWindow::go_forward() {
    if (history_position >= (int)history.size() - 1) {
        cout << "[Navigate] Cannot go forward - at end of history" << endl;
        return;
    }
    
    history_position++;
    
    const HistoryEntry& entry = history[history_position];
    current_url = entry.url;
    
    cout << "[Navigate] Going forward to: " << current_url 
         << " (position " << history_position << ")" << endl;
    
    update_url_bar();
    update_navigation_buttons();
    render_html(entry.html_content);
}

// Refresh the current page
void BrowserWindow::refresh() {
    if (current_url.empty()) {
        cout << "[Navigate] Cannot refresh - no current URL" << endl;
        return;
    }
    
    cout << "[Navigate] Refreshing: " << current_url << endl;
    
    // Fetch fresh content
    string html = fetch_url(current_url);
    
    if (html.empty()) {
        cout << "[Navigate] ERROR: Failed to refresh" << endl;
        return;
    }
    
    // Update the current history entry with fresh content
    if (history_position >= 0 && history_position < (int)history.size()) {
        history[history_position].html_content = html;
    }
    
    render_html(html);
}

// Callback for back button
void BrowserWindow::on_back_clicked(GtkWidget* widget, gpointer data) {
    BrowserWindow* window = static_cast<BrowserWindow*>(data);
    window->go_back();
}

// Callback for forward button
void BrowserWindow::on_forward_clicked(GtkWidget* widget, gpointer data) {
    BrowserWindow* window = static_cast<BrowserWindow*>(data);
    window->go_forward();
}

// Callback for refresh button
void BrowserWindow::on_refresh_clicked(GtkWidget* widget, gpointer data) {
    BrowserWindow* window = static_cast<BrowserWindow*>(data);
    window->refresh();
}

// Callback when Enter is pressed in URL bar
void BrowserWindow::on_url_activate(GtkEntry* entry, gpointer data) {
    BrowserWindow* window = static_cast<BrowserWindow*>(data);
    const gchar* text = gtk_entry_get_text(entry);
    
    string input(text);
    
    if (input.empty()) {
        return;
    }
    
    window->navigate_to(input);
}

void BrowserWindow::setup_ui() {
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), title.c_str());
    gtk_window_set_default_size(GTK_WINDOW(window), width, height);
    
    set_window_icon();
    
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    
    main_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(window), main_vbox);
    
    create_toolbar();
    gtk_box_pack_start(GTK_BOX(main_vbox), toolbar, FALSE, FALSE, 0);
    
    scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
                                   GTK_POLICY_AUTOMATIC,
                                   GTK_POLICY_AUTOMATIC);
    
    gtk_box_pack_start(GTK_BOX(main_vbox), scrolled_window, TRUE, TRUE, 0);
    
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
    }
    
    gtk_container_add(GTK_CONTAINER(scrolled_window), text_view);
    
    GtkCssProvider* css_provider = gtk_css_provider_new();
    const gchar* css_data = "textview { background-color: white; font-family: 'Sans'; font-size: 14px; }";
    gtk_css_provider_load_from_data(css_provider, css_data, -1, NULL);
    
    GtkStyleContext* context = gtk_widget_get_style_context(text_view);
    gtk_style_context_add_provider(context,
                                   GTK_STYLE_PROVIDER(css_provider),
                                   GTK_STYLE_PROVIDER_PRIORITY_USER);
    
    cout << "UI setup complete" << endl;
}

void BrowserWindow::render_html(const string& html) {
    if (!text_buffer) {
        cout << "ERROR: text_buffer is NULL!" << endl;
        return;
    }
    
    cout << "[C++] render_html called with HTML length: " << html.length() << endl;
    
    gtk_text_buffer_set_text(text_buffer, "", -1);
    
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
    cout << "Now with real web browsing capabilities!" << endl;
    cout << endl;
    
    // Initialize curl globally
    curl_global_init(CURL_GLOBAL_DEFAULT);
    
    gtk_init(&argc, &argv);
    cout << "GTK initialized" << endl;
    
    // Updated welcome page with proper HTML structure
    string html_content = 
        "<html>"
        "<body>"
        "<h1>Welcome to Szymdows Browser</h1>"
        "<p>You can now browse the real web!</p>"
        "<h2>Try It Out</h2>"
        "<p>Type a URL like example.com or search for something like rust programming</p>"
        "<h2>Features</h2>"
        "<p>Back and forward buttons work with your browsing history</p>"
        "<p>Refresh button reloads the current page</p>"
        "<p>Uses our Rust parser to parse real HTML from the internet!</p>"
        "<h2>Architecture</h2>"
        "<p>The browser now has a modular structure like Firefox</p>"
        "<p>Separate modules for HTML tree, parser, tokenizer, and tree builder</p>"
        "</body>"
        "</html>";
    
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
    
    // Cleanup curl
    curl_global_cleanup();
    
    return 0;
}