#include "render.h"
#include <iostream>
#include <sstream>
#include <algorithm>

// Renderer Implementation
// This file contains the actual rendering logic

// Constructor - This initializes the renderer with default values
Renderer::Renderer() {
    window_width = 800;
    window_height = 600;
    output = "";
    
    // Print initialization message for debugging
    // TODO: Remove this debug output later
    std::cout << "Renderer initialized with dimensions: " 
              << window_width << "x" << window_height << std::endl;
}

// Destructor - This cleans up when the renderer is destroyed
Renderer::~Renderer() {
    // Currently nothing to clean up
    // If we allocate any dynamic memory in the future, it should be freed here
}

// This is the main rendering function
// It takes HTML as input and generates formatted text output
void Renderer::render(const std::string& html) {
    // Clear any previous output
    output.clear();
    
    // This is a very simplified HTML parser built into the renderer
    // Eventually this should use the proper Rust parser instead
    // But for now this works for basic testing
    
    size_t position = 0;
    
    // Loop through the entire HTML string
    while (position < html.length()) {
        // Find the next opening tag
        size_t tag_start = html.find('<', position);
        
        // If no more tags found, we're done
        if (tag_start == std::string::npos) {
            break;
        }
        
        // Find the closing bracket of the opening tag
        size_t tag_end = html.find('>', tag_start);
        if (tag_end == std::string::npos) {
            // Malformed HTML - missing closing bracket
            // Just break out to avoid infinite loop
            break;
        }
        
        // Extract the tag name (everything between < and >)
        std::string tag = html.substr(tag_start + 1, tag_end - tag_start - 1);
        
        // Build the closing tag string
        std::string closing_tag = "</" + tag + ">";
        
        // Find where the closing tag is
        size_t content_end = html.find(closing_tag, tag_end);
        
        if (content_end != std::string::npos) {
            // Extract the content between the opening and closing tags
            std::string content = html.substr(tag_end + 1, content_end - tag_end - 1);
            
            // Render this node
            render_node(tag, content);
            
            // Move position past the closing tag
            position = content_end + closing_tag.length();
        } else {
            // No closing tag found
            // This might be a self-closing tag or malformed HTML
            // For now just skip past it
            position = tag_end + 1;
        }
    }
}

// This function renders individual HTML elements
// Different tags get different formatting
void Renderer::render_node(const std::string& tag, const std::string& content) {
    if (tag == "h1") {
        // Render h1 as a large heading with decorative borders
        // Using equals signs to make it stand out
        output += "\n";
        output += "================================\n";
        output += "  " + content + "\n";
        output += "================================\n";
        output += "\n";
    } 
    else if (tag == "p") {
        // Render paragraph as plain text with spacing
        output += content + "\n";
        output += "\n";
    }
    else {
        // Unknown tag - just render the content with a label
        // This helps with debugging when new tags are added
        output += "[" + tag + "] " + content + "\n";
    }
}

// This returns the rendered output
std::string Renderer::get_output() {
    return output;
}

// This sets the viewport width
void Renderer::set_width(int width) {
    if (width > 0) {
        window_width = width;
    }
}

// This sets the viewport height  
void Renderer::set_height(int height) {
    if (height > 0) {
        window_height = height;
    }
}

// This function should wrap text to fit within a certain width
// TODO: Actually implement this properly
// Right now it's not used but it will be needed when we have proper text rendering
std::string Renderer::wrap_text(const std::string& text, int max_width) {
    // Placeholder implementation
    // This should split the text into lines that fit within max_width
    // For now just return the text as-is
    return text;
}