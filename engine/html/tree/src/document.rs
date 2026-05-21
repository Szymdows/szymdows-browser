// Document Implementation
// Represents the entire HTML document
// Similar to Firefox's dom/base/Document

use crate::node::Node;

/// The HTML document
/// This is the root of the DOM tree
pub struct Document {
    /// The root element (usually <html>)
    pub root: Option<Node>,
    
    /// The document's URL
    pub url: String,
    
    /// The document title (from <title> tag)
    pub title: String,
}

impl Document {
    /// Creates a new empty document
    pub fn new() -> Self {
        Document {
            root: None,
            url: String::new(),
            title: String::new(),
        }
    }
    
    /// Creates a document with a URL
    pub fn with_url(url: &str) -> Self {
        Document {
            root: None,
            url: url.to_string(),
            title: String::new(),
        }
    }
    
    /// Sets the root node of the document
    pub fn set_root(&mut self, root: Node) {
        self.root = Some(root);
    }
    
    /// Gets a reference to the root node
    pub fn get_root(&self) -> Option<&Node> {
        self.root.as_ref()
    }
}