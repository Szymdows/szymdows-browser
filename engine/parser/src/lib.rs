// HTML Parser Module for Szymdows Web Engine
// This module is responsible for parsing HTML strings into a DOM tree structure
// It's still pretty basic but I'm working on improving it

use std::collections::HashMap;

/// Represents the different types of nodes in the DOM tree
/// Currently only supporting Element and Text nodes
/// TODO: Add support for Comment nodes and other node types
#[derive(Debug, Clone)]
pub enum NodeType {
    Element,
    Text,
}

/// This structure represents a single node in the DOM tree
/// Each node can have a tag name (for elements), text content, children, and attributes
#[derive(Debug, Clone)]
pub struct DOMNode {
    pub node_type: NodeType,
    pub tag_name: Option<String>,
    pub text_content: Option<String>,
    pub children: Vec<DOMNode>,
    pub attributes: HashMap<String, String>,
}

/// The main HTML Parser structure
/// It keeps track of the HTML content and current position while parsing
pub struct HTMLParser {
    content: String,
    position: usize,
}

impl HTMLParser {
    /// Creates a new HTMLParser instance with the given HTML content
    /// 
    /// # Arguments
    /// * `html` - A String containing the HTML to parse
    /// 
    /// # Returns
    /// A new HTMLParser ready to parse the content
    pub fn new(html: String) -> HTMLParser {
        HTMLParser {
            content: html,
            position: 0,
        }
    }
    
    /// Parses the HTML content and returns a vector of DOM nodes
    /// This is the main entry point for parsing
    /// 
    /// # Returns
    /// A Vec<DOMNode> containing all the parsed nodes
    pub fn parse(&mut self) -> Vec<DOMNode> {
        let mut nodes: Vec<DOMNode> = Vec::new();
        
        // Loop through the entire HTML content
        while self.position < self.content.len() {
            // Skip any whitespace at the current position
            self.skip_whitespace();
            
            // Check if we've reached the end
            if self.position >= self.content.len() {
                break;
            }
            
            // Check if current character is the start of a tag
            if self.current_char() == '<' {
                // It's a tag, so parse it as an element
                let node = self.parse_element();
                nodes.push(node);
            } else {
                // It's not a tag, so it must be text content
                let text = self.parse_text();
                // Only add text nodes if they're not empty
                if text.len() > 0 {
                    nodes.push(DOMNode {
                        node_type: NodeType::Text,
                        tag_name: None,
                        text_content: Some(text),
                        children: Vec::new(),
                        attributes: HashMap::new(),
                    });
                }
            }
        }
        
        nodes
    }
    
    /// Parses an HTML element (a tag and its contents)
    /// This handles opening tags, content, and closing tags
    /// 
    /// # Returns
    /// A DOMNode representing the parsed element
    fn parse_element(&mut self) -> DOMNode {
        // Move past the opening '<' character
        self.position += 1;
        
        // Parse the tag name (like "h1", "p", etc.)
        let tag_name = self.parse_tag_name();
        
        // Skip past any attributes and the closing '>' of the opening tag
        // TODO: Actually parse attributes instead of just skipping them
        while self.position < self.content.len() && self.current_char() != '>' {
            self.position += 1;
        }
        self.position += 1; // Move past the '>'
        
        // Now we need to get the content between the opening and closing tags
        let mut text_content = String::new();
        let start_position = self.position;
        
        // Construct what the closing tag should look like
        let closing_tag = format!("</{}>", tag_name);
        
        // Search for the closing tag
        while self.position < self.content.len() {
            // Check if we've found the closing tag
            if self.content[self.position..].starts_with(&closing_tag) {
                // Extract the content between the tags
                text_content = self.content[start_position..self.position].to_string();
                // Move position past the closing tag
                self.position += closing_tag.len();
                break;
            }
            self.position += 1;
        }
        
        // Create and return the DOM node
        // Note: children vec is empty because we don't handle nested tags yet
        // That's something I need to implement next
        DOMNode {
            node_type: NodeType::Element,
            tag_name: Some(tag_name),
            text_content: Some(text_content),
            children: Vec::new(),
            attributes: HashMap::new(),
        }
    }
    
    /// Parses a tag name from the current position
    /// Continues until it hits a space or '>' character
    /// 
    /// # Returns
    /// A String containing the tag name
    fn parse_tag_name(&mut self) -> String {
        let mut name = String::new();
        
        while self.position < self.content.len() {
            let c = self.current_char();
            // Tag names end when we hit a space or '>'
            if c == '>' || c == ' ' || c == '\n' || c == '\t' {
                break;
            }
            name.push(c);
            self.position += 1;
        }
        
        name
    }
    
    /// Parses text content (anything that's not a tag)
    /// Continues until it encounters a '<' character
    /// 
    /// # Returns
    /// A String containing the text content, trimmed of whitespace
    fn parse_text(&mut self) -> String {
        let mut text = String::new();
        
        while self.position < self.content.len() && self.current_char() != '<' {
            text.push(self.current_char());
            self.position += 1;
        }
        
        // Trim whitespace from the text
        text.trim().to_string()
    }
    
    /// Gets the character at the current position
    /// Returns null character '\0' if position is out of bounds
    /// 
    /// # Returns
    /// The character at the current position
    fn current_char(&self) -> char {
        self.content.chars().nth(self.position).unwrap_or('\0')
    }
    
    /// Skips over whitespace characters (spaces, newlines, tabs, etc.)
    /// Advances the position until a non-whitespace character is found
    fn skip_whitespace(&mut self) {
        while self.position < self.content.len() {
            let c = self.current_char();
            if c != ' ' && c != '\n' && c != '\r' && c != '\t' {
                break;
            }
            self.position += 1;
        }
    }
}

// Unit tests for the parser
// These help make sure the parsing logic works correctly
#[cfg(test)]
mod tests {
    use super::*;
    
    #[test]
    fn test_parse_simple_h1() {
        let html = "<h1>Hello World</h1>".to_string();
        let mut parser = HTMLParser::new(html);
        let nodes = parser.parse();
        
        assert_eq!(nodes.len(), 1);
        // TODO: Add more detailed assertions here
    }
    
    #[test]
    fn test_parse_multiple_tags() {
        let html = "<h1>Title</h1><p>Paragraph</p>".to_string();
        let mut parser = HTMLParser::new(html);
        let nodes = parser.parse();
        
        assert_eq!(nodes.len(), 2);
    }
}

// Foreign Function Interface (FFI) functions
// These allow the C++ code to call into this Rust code
// I'm not actually using these yet but they're here for when I integrate everything

/// Parses HTML from a C-style string pointer
/// This is called from C++ code
/// 
/// # Safety
/// This function is unsafe because it deals with raw pointers from C++
/// The caller must ensure the pointer is valid and the length is correct
#[no_mangle]
pub extern "C" fn parse_html(html_ptr: *const u8, len: usize) -> *mut Vec<DOMNode> {
    unsafe {
        // Convert the C pointer to a Rust slice
        let html_bytes = std::slice::from_raw_parts(html_ptr, len);
        // Convert bytes to a String
        let html = String::from_utf8_lossy(html_bytes).to_string();
        
        // Create parser and parse the HTML
        let mut parser = HTMLParser::new(html);
        let nodes = parser.parse();
        
        // Return a pointer to the nodes that C++ can use
        Box::into_raw(Box::new(nodes))
    }
}

/// Frees the memory allocated for nodes
/// This must be called from C++ when done with the nodes to prevent memory leaks
/// 
/// # Safety
/// This function is unsafe because it deals with raw pointers
/// The caller must ensure the pointer was created by parse_html and hasn't been freed already
#[no_mangle]
pub extern "C" fn free_nodes(nodes_ptr: *mut Vec<DOMNode>) {
    unsafe {
        if !nodes_ptr.is_null() {
            // Convert the pointer back to a Box and drop it
            // This frees the memory
            Box::from_raw(nodes_ptr);
        }
    }
}