// HTML Parser Module
// This is the main entry point for HTML parsing
// Similar to Firefox's parser/html

pub mod tokenizer;
pub mod tree_builder;
pub mod tag_token;

use html_tree::{Node, Document};
use tree_builder::TreeBuilder;

/// Parse HTML string into a Document
/// This is the main public API for parsing
pub fn parse_html(html: &str) -> Document {
    let mut builder = TreeBuilder::new();
    builder.parse(html);
    builder.into_document()
}

// Re-export for convenience
pub use html_tree;

// FFI exports
use std::os::raw::c_char;

/// Simple structure for passing nodes to C++
/// This flattens the tree for easier FFI
#[repr(C)]
pub struct FlatNode {
    pub tag: String,
    pub content: String,
    pub is_text: bool,
}

/// Array of flat nodes for FFI
#[repr(C)]
pub struct NodeArray {
    pub nodes: *mut FlatNode,
    pub count: usize,
}

/// Parse HTML and return flattened nodes for C++
#[no_mangle]
pub extern "C" fn parse_html_to_nodes(html_ptr: *const u8, html_len: usize) -> *mut NodeArray {
    unsafe {
        let html_bytes = std::slice::from_raw_parts(html_ptr, html_len);
        let html = String::from_utf8_lossy(html_bytes).to_string();
        
        println!("[Rust Parser] Parsing HTML of length: {}", html.len());
        
        // Parse the HTML
        let document = parse_html(&html);
        
        // Flatten the tree
        let mut flat_nodes = Vec::new();
        if let Some(root) = document.root {
            flatten_tree(&root, &mut flat_nodes);
        }
        
        println!("[Rust Parser] Extracted {} nodes", flat_nodes.len());
        
        // Debug: print first few nodes
        for (i, node) in flat_nodes.iter().enumerate().take(5) {
            let content_preview = if node.content.len() > 50 {
                format!("{}...", &node.content[..50])
            } else {
                node.content.clone()
            };
            println!("[Rust Parser]   Node {}: tag='{}', content='{}'", 
                     i, node.tag, content_preview);
        }
        
        // Convert to C-compatible array
        let mut node_vec = flat_nodes.into_boxed_slice();
        let node_ptr = node_vec.as_mut_ptr();
        let node_count = node_vec.len();
        
        std::mem::forget(node_vec);
        
        let array = Box::new(NodeArray {
            nodes: node_ptr,
            count: node_count,
        });
        
        Box::into_raw(array)
    }
}

/// Flatten the DOM tree into a list of nodes
fn flatten_tree(node: &Node, result: &mut Vec<FlatNode>) {
    use html_tree::NodeType;
    
    match node.node_type {
        NodeType::Element => {
            // Check if this is a block-level element we want to display
            if is_displayable_tag(node.tag_name().unwrap_or("")) {
                let text = node.inner_text();
                if !text.is_empty() {
                    result.push(FlatNode {
                        tag: node.data.clone(),
                        content: text,
                        is_text: false,
                    });
                }
            } else {
                // Not a displayable tag, process children
                for child in &node.children {
                    flatten_tree(child, result);
                }
            }
        }
        NodeType::Text => {
            let text = node.data.trim();
            if !text.is_empty() && !is_only_whitespace(text) {
                result.push(FlatNode {
                    tag: String::new(),
                    content: text.to_string(),
                    is_text: true,
                });
            }
        }
        _ => {
            // Skip other node types
        }
    }
}

/// Check if a tag should be displayed
fn is_displayable_tag(tag: &str) -> bool {
    matches!(tag,
        "h1" | "h2" | "h3" | "h4" | "h5" | "h6" |
        "p" | "div" | "section" | "article" | "header" | "footer" |
        "nav" | "main" | "aside" | "blockquote" | "pre" |
        "li" | "dt" | "dd" | "td" | "th" | "caption"
    )
}

/// Check if string is only whitespace
fn is_only_whitespace(s: &str) -> bool {
    s.chars().all(|c| c.is_whitespace())
}

// FFI helper functions

#[no_mangle]
pub extern "C" fn get_node_count(array_ptr: *const NodeArray) -> usize {
    unsafe {
        if array_ptr.is_null() {
            return 0;
        }
        (*array_ptr).count
    }
}

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

#[no_mangle]
pub extern "C" fn free_node_array(array_ptr: *mut NodeArray) {
    unsafe {
        if array_ptr.is_null() {
            return;
        }
        
        let array = Box::from_raw(array_ptr);
        let nodes = Vec::from_raw_parts(array.nodes, array.count, array.count);
        drop(nodes);
        
        println!("[Rust Parser] Freed node array");
    }
}