// HTML Parser Module for Szymdows Web Engine
// This module is responsible for parsing HTML strings into a DOM tree structure
// This parser will be called from C++ via FFI (Foreign Function Interface)

use std::collections::HashMap;

/// Represents the different types of nodes in the DOM tree
/// Currently only supporting Element and Text nodes
#[derive(Debug, Clone)]
#[repr(C)]
pub enum NodeType {
    Element = 0,
    Text = 1,
}

/// This structure represents a single node in the DOM tree
/// Each node can have a tag name (for elements), text content, children, and attributes
/// This is designed to be passed across the FFI boundary to C++
#[derive(Debug, Clone)]
#[repr(C)]
pub struct DOMNode {
    pub node_type: NodeType,
    pub tag_name: *mut std::os::raw::c_char,  // C-compatible string pointer
    pub text_content: *mut std::os::raw::c_char,  // C-compatible string pointer
    pub children_count: usize,
    pub children: *mut DOMNode,  // Pointer to array of children
}

/// The main HTML Parser structure
/// It keeps track of the HTML content and current position while parsing
pub struct HTMLParser {
    content: String,
    position: usize,
}

impl HTMLParser {
    /// Creates a new HTMLParser instance with the given HTML content
    pub fn new(html: String) -> HTMLParser {
        HTMLParser {
            content: html,
            position: 0,
        }
    }
    
    /// Parses the HTML content and returns a vector of DOM nodes
    pub fn parse(&mut self) -> Vec<SimpleDOMNode> {
        let mut nodes: Vec<SimpleDOMNode> = Vec::new();
        
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
                    nodes.push(SimpleDOMNode {
                        tag: String::new(),
                        content: text,
                        is_text: true,
                    });
                }
            }
        }
        
        nodes
    }
    
    /// Parses an HTML element (a tag and its contents)
    fn parse_element(&mut self) -> SimpleDOMNode {
        // Move past the opening '<' character
        self.position += 1;
        
        // Parse the tag name (like "h1", "p", etc.)
        let tag_name = self.parse_tag_name();
        
        // Skip past any attributes and the closing '>' of the opening tag
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
        
        SimpleDOMNode {
            tag: tag_name,
            content: text_content,
            is_text: false,
        }
    }
    
    /// Parses a tag name from the current position
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
    fn current_char(&self) -> char {
        self.content.chars().nth(self.position).unwrap_or('\0')
    }
    
    /// Skips over whitespace characters
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

// Simpler structure for FFI that's easier to work with
// This is what we'll actually pass to C++
#[repr(C)]
pub struct SimpleDOMNode {
    pub tag: String,
    pub content: String,
    pub is_text: bool,
}

// Struct to hold array of nodes for FFI
#[repr(C)]
pub struct NodeArray {
    pub nodes: *mut SimpleDOMNode,
    pub count: usize,
}

// FFI Functions - These can be called from C++

/// Parses HTML and returns a pointer to an array of nodes
/// This is the main function C++ will call
#[no_mangle]
pub extern "C" fn parse_html_to_nodes(html_ptr: *const u8, html_len: usize) -> *mut NodeArray {
    unsafe {
        // Convert C string to Rust String
        let html_bytes = std::slice::from_raw_parts(html_ptr, html_len);
        let html = String::from_utf8_lossy(html_bytes).to_string();
        
        println!("[Rust] Parsing HTML of length: {}", html.len());
        
        // Create parser and parse
        let mut parser = HTMLParser::new(html);
        let nodes = parser.parse();
        
        println!("[Rust] Parsed {} nodes", nodes.len());
        
        // Convert Vec to array for C++
        let mut node_vec = nodes.into_boxed_slice();
        let node_ptr = node_vec.as_mut_ptr();
        let node_count = node_vec.len();
        
        // Prevent Rust from freeing the memory
        std::mem::forget(node_vec);
        
        // Create NodeArray struct
        let array = Box::new(NodeArray {
            nodes: node_ptr,
            count: node_count,
        });
        
        Box::into_raw(array)
    }
}

/// Gets the tag name from a node at a specific index
/// Helper function for C++ to read node data
#[no_mangle]
pub extern "C" fn get_node_tag(array_ptr: *const NodeArray, index: usize) -> *const u8 {
    unsafe {
        if array_ptr.is_null() {
            return std::ptr::null();
        }
        
        let array = &*array_ptr;
        if index >= array.count {
            return std::ptr::null();
        }
        
        let node = &*array.nodes.add(index);
        node.tag.as_ptr()
    }
}

/// Gets the tag name length
#[no_mangle]
pub extern "C" fn get_node_tag_len(array_ptr: *const NodeArray, index: usize) -> usize {
    unsafe {
        if array_ptr.is_null() {
            return 0;
        }
        
        let array = &*array_ptr;
        if index >= array.count {
            return 0;
        }
        
        let node = &*array.nodes.add(index);
        node.tag.len()
    }
}

/// Gets the content from a node at a specific index
#[no_mangle]
pub extern "C" fn get_node_content(array_ptr: *const NodeArray, index: usize) -> *const u8 {
    unsafe {
        if array_ptr.is_null() {
            return std::ptr::null();
        }
        
        let array = &*array_ptr;
        if index >= array.count {
            return std::ptr::null();
        }
        
        let node = &*array.nodes.add(index);
        node.content.as_ptr()
    }
}

/// Gets the content length
#[no_mangle]
pub extern "C" fn get_node_content_len(array_ptr: *const NodeArray, index: usize) -> usize {
    unsafe {
        if array_ptr.is_null() {
            return 0;
        }
        
        let array = &*array_ptr;
        if index >= array.count {
            return 0;
        }
        
        let node = &*array.nodes.add(index);
        node.content.len()
    }
}

/// Gets whether the node is a text node
#[no_mangle]
pub extern "C" fn get_node_is_text(array_ptr: *const NodeArray, index: usize) -> bool {
    unsafe {
        if array_ptr.is_null() {
            return false;
        }
        
        let array = &*array_ptr;
        if index >= array.count {
            return false;
        }
        
        let node = &*array.nodes.add(index);
        node.is_text
    }
}

/// Gets the total number of nodes
#[no_mangle]
pub extern "C" fn get_node_count(array_ptr: *const NodeArray) -> usize {
    unsafe {
        if array_ptr.is_null() {
            return 0;
        }
        
        let array = &*array_ptr;
        array.count
    }
}

/// Frees the node array memory
/// MUST be called when C++ is done with the nodes
#[no_mangle]
pub extern "C" fn free_node_array(array_ptr: *mut NodeArray) {
    unsafe {
        if array_ptr.is_null() {
            return;
        }
        
        let array = Box::from_raw(array_ptr);
        
        // Reconstruct the Vec to properly free it
        let nodes = Vec::from_raw_parts(array.nodes, array.count, array.count);
        drop(nodes);
        
        // array is automatically dropped here
        println!("[Rust] Freed node array");
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    
    #[test]
    fn test_parse_simple() {
        let html = "<h1>Hello</h1><p>World</p>".to_string();
        let mut parser = HTMLParser::new(html);
        let nodes = parser.parse();
        
        assert_eq!(nodes.len(), 2);
        assert_eq!(nodes[0].tag, "h1");
        assert_eq!(nodes[0].content, "Hello");
        assert_eq!(nodes[1].tag, "p");
        assert_eq!(nodes[1].content, "World");
    }
}