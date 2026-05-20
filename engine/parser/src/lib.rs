// HTML Parser Module for Szymdows Web Engine
// Now supports nested HTML elements!

use std::collections::HashMap;

/// Represents the different types of nodes in the DOM tree
#[derive(Debug, Clone)]
#[repr(C)]
pub enum NodeType {
    Element = 0,
    Text = 1,
}

/// The main HTML Parser structure
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
    /// Now handles nested elements!
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
                // Check if it's a closing tag - if so, we're done with this level
                if self.peek_ahead(1) == '/' {
                    break;
                }
                
                // It's an opening tag, parse it as an element
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
                        children: Vec::new(),
                    });
                }
            }
        }
        
        nodes
    }
    
    /// Parses an HTML element with support for nested children
    fn parse_element(&mut self) -> SimpleDOMNode {
        // Move past the opening '<' character
        self.position += 1;
        
        // Parse the tag name (like "h1", "p", "div", etc.)
        let tag_name = self.parse_tag_name();
        
        // Skip past any attributes and the closing '>' of the opening tag
        while self.position < self.content.len() && self.current_char() != '>' {
            self.position += 1;
        }
        self.position += 1; // Move past the '>'
        
        // Check for self-closing tags (like <br/>, <img/>, etc.)
        // For now we'll treat them as empty elements
        
        // Parse the content and children
        let mut text_content = String::new();
        let mut children: Vec<SimpleDOMNode> = Vec::new();
        
        // Construct what the closing tag should look like
        let closing_tag = format!("</{}>", tag_name);
        
        let content_start = self.position;
        
        // Parse children until we hit the closing tag
        while self.position < self.content.len() {
            // Check if we've found the closing tag
            if self.content[self.position..].starts_with(&closing_tag) {
                // We found the closing tag
                // If we haven't parsed any children, treat everything as text content
                if children.is_empty() {
                    text_content = self.content[content_start..self.position].to_string();
                    // Trim the text content
                    text_content = text_content.trim().to_string();
                }
                
                // Move position past the closing tag
                self.position += closing_tag.len();
                break;
            }
            
            // Check if there's a nested tag
            if self.current_char() == '<' && self.peek_ahead(1) != '/' {
                // There's a nested element - parse it recursively
                let child = self.parse_element();
                children.push(child);
            } else if self.current_char() == '<' && self.peek_ahead(1) == '/' {
                // Hit a closing tag, we're done with children
                break;
            } else {
                // It's text content
                let text = self.parse_text();
                if text.len() > 0 {
                    children.push(SimpleDOMNode {
                        tag: String::new(),
                        content: text,
                        is_text: true,
                        children: Vec::new(),
                    });
                }
            }
        }
        
        SimpleDOMNode {
            tag: tag_name,
            content: text_content,
            is_text: false,
            children: children,
        }
    }
    
    /// Parses a tag name from the current position
    fn parse_tag_name(&mut self) -> String {
        let mut name = String::new();
        
        while self.position < self.content.len() {
            let c = self.current_char();
            // Tag names end when we hit a space, >, /, or newline
            if c == '>' || c == ' ' || c == '\n' || c == '\t' || c == '/' {
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
    
    /// Peeks ahead n characters without moving position
    fn peek_ahead(&self, n: usize) -> char {
        self.content.chars().nth(self.position + n).unwrap_or('\0')
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

// Simpler structure for FFI that supports nested children
#[repr(C)]
pub struct SimpleDOMNode {
    pub tag: String,
    pub content: String,
    pub is_text: bool,
    pub children: Vec<SimpleDOMNode>,  // Now supports children!
}

// Struct to hold array of nodes for FFI
#[repr(C)]
pub struct NodeArray {
    pub nodes: *mut SimpleDOMNode,
    pub count: usize,
}

// Helper function to flatten the tree into a list for easier C++ consumption
// This does a depth-first traversal and extracts all text content
fn flatten_node(node: &SimpleDOMNode, result: &mut Vec<SimpleDOMNode>) {
    if node.is_text {
        // It's a text node, add it directly
        if !node.content.is_empty() {
            result.push(SimpleDOMNode {
                tag: String::new(),
                content: node.content.clone(),
                is_text: true,
                children: Vec::new(),
            });
        }
    } else {
        // It's an element node
        if node.children.is_empty() {
            // No children, just add the content as this tag
            if !node.content.is_empty() {
                result.push(SimpleDOMNode {
                    tag: node.tag.clone(),
                    content: node.content.clone(),
                    is_text: false,
                    children: Vec::new(),
                });
            }
        } else {
            // Has children - flatten them and combine their text
            let mut combined_text = String::new();
            
            for child in &node.children {
                if child.is_text {
                    combined_text.push_str(&child.content);
                    combined_text.push(' ');
                } else {
                    // For nested tags, just extract their text recursively
                    combined_text.push_str(&extract_text(child));
                    combined_text.push(' ');
                }
            }
            
            combined_text = combined_text.trim().to_string();
            
            if !combined_text.is_empty() {
                result.push(SimpleDOMNode {
                    tag: node.tag.clone(),
                    content: combined_text,
                    is_text: false,
                    children: Vec::new(),
                });
            }
        }
    }
}

// Helper to extract all text from a node and its children recursively
fn extract_text(node: &SimpleDOMNode) -> String {
    if node.is_text {
        return node.content.clone();
    }
    
    let mut text = String::new();
    
    if !node.content.is_empty() {
        text.push_str(&node.content);
        text.push(' ');
    }
    
    for child in &node.children {
        text.push_str(&extract_text(child));
        text.push(' ');
    }
    
    text.trim().to_string()
}

// FFI Functions

/// Parses HTML and returns a flattened array of nodes
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
        
        println!("[Rust] Parsed {} top-level nodes", nodes.len());
        
        // Flatten the tree structure into a list
        let mut flattened: Vec<SimpleDOMNode> = Vec::new();
        for node in &nodes {
            flatten_node(node, &mut flattened);
        }
        
        println!("[Rust] Flattened to {} nodes", flattened.len());
        
        // Convert Vec to array for C++
        let mut node_vec = flattened.into_boxed_slice();
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
        
        println!("[Rust] Freed node array");
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    
    #[test]
    fn test_parse_nested() {
        let html = "<div><h1>Title</h1><p>Content</p></div>".to_string();
        let mut parser = HTMLParser::new(html);
        let nodes = parser.parse();
        
        assert_eq!(nodes.len(), 1);
        assert_eq!(nodes[0].tag, "div");
        assert_eq!(nodes[0].children.len(), 2);
    }
    
    #[test]
    fn test_parse_simple() {
        let html = "<h1>Hello</h1><p>World</p>".to_string();
        let mut parser = HTMLParser::new(html);
        let nodes = parser.parse();
        
        assert_eq!(nodes.len(), 2);
        assert_eq!(nodes[0].tag, "h1");
        assert_eq!(nodes[0].content, "Hello");
    }
}