#ifndef RENDER_H
#define RENDER_H

#include <string>
#include <vector>

// Renderer Header File
// This defines the interface for the rendering engine
// The renderer takes parsed HTML and converts it into displayable output

// Forward declaration of RenderNode
// This is used for the internal representation of nodes during rendering
class RenderNode;

/// The main Renderer class
/// This class is responsible for taking HTML content and rendering it
/// Currently it just renders to text, but eventually it should render graphics
class Renderer {
public:
    // Constructor - initializes the renderer with default settings
    Renderer();
    
    // Destructor - cleans up any allocated resources
    ~Renderer();
    
    /// Renders the given HTML string
    /// This is the main function that processes HTML and generates output
    /// @param html - The HTML content to render as a string
    void render(const std::string& html);
    
    /// Gets the rendered output as a string
    /// This returns whatever was rendered by the last render() call
    /// @return The rendered output
    std::string get_output();
    
    /// Sets the width of the rendering viewport
    /// @param width - The desired width in pixels (or characters for text mode)
    void set_width(int width);
    
    /// Sets the height of the rendering viewport
    /// @param height - The desired height in pixels (or characters for text mode)
    void set_height(int height);
    
private:
    // The rendered output stored as a string
    std::string output;
    
    // Viewport dimensions
    int window_width;
    int window_height;
    
    /// Internal function to render a single node
    /// This handles the actual rendering logic for each HTML element
    /// @param tag - The HTML tag name
    /// @param content - The text content inside the tag
    void render_node(const std::string& tag, const std::string& content);
    
    /// Helper function to wrap text to fit within the viewport width
    /// TODO: Implement this function
    /// @param text - The text to wrap
    /// @param max_width - Maximum width for each line
    /// @return The wrapped text
    std::string wrap_text(const std::string& text, int max_width);
};

/// Structure representing a node to be rendered
/// This is a simplified version for the rendering pipeline
/// TODO: Maybe this should be integrated with the DOM nodes from Rust?
struct RenderNode {
    std::string tag;        // HTML tag name
    std::string content;    // Text content
    std::vector<RenderNode*> children;  // Child nodes
    
    // Constructor
    RenderNode(const std::string& t, const std::string& c) 
        : tag(t), content(c) {}
    
    // Destructor - needs to clean up children
    // Note: I need to make sure this doesn't cause memory leaks
    ~RenderNode() {
        for (auto child : children) {
            delete child;
        }
    }
};

#endif // RENDER_H