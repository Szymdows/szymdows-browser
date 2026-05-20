// DOM Node Module
// This defines the structure and operations for DOM nodes
// I'm still learning about how real browser engines structure their DOM
// so this is a simplified version for now

use std::fmt;

/// Represents a node in the Document Object Model
/// This is a tree structure where each node can have children
pub struct Node {
    /// The type of node (like "element", "text", etc.)
    pub node_type: String,
    /// The HTML tag name if this is an element (like "div", "p", "h1")
    pub tag: Option<String>,
    /// Text content if this is a text node
    pub content: Option<String>,
    /// Child nodes (for nested elements)
    pub children: Vec<Node>,
}

impl Node {
    /// Creates a new Node with the specified type
    /// 
    /// # Arguments
    /// * `node_type` - A String indicating the type of node
    /// 
    /// # Returns
    /// A new Node instance with empty tag, content, and no children
    /// 
    /// # Example
    /// ```
    /// let element_node = Node::new("element".to_string());
    /// let text_node = Node::new("text".to_string());
    /// ```
    pub fn new(node_type: String) -> Node {
        Node {
            node_type: node_type,
            tag: None,
            content: None,
            children: Vec::new(),
        }
    }
    
    /// Adds a child node to this node
    /// This is used to build the tree structure of the DOM
    /// 
    /// # Arguments
    /// * `child` - The Node to add as a child
    /// 
    /// # Example
    /// ```
    /// let mut parent = Node::new("element".to_string());
    /// let child = Node::new("text".to_string());
    /// parent.add_child(child);
    /// ```
    pub fn add_child(&mut self, child: Node) {
        self.children.push(child);
    }
    
    /// Prints the node tree structure for debugging purposes
    /// This helps visualize the DOM tree structure
    /// 
    /// # Arguments
    /// * `indent` - The number of spaces to indent (for showing nesting level)
    pub fn print_tree(&self, indent: usize) {
        // Create spacing for indentation
        let spacing = " ".repeat(indent);
        
        // Print the node type
        println!("{}Node Type: {:?}", spacing, self.node_type);
        
        // Print tag if it exists
        if let Some(ref tag) = self.tag {
            println!("{}  Tag: {}", spacing, tag);
        }
        
        // Print content if it exists
        if let Some(ref content) = self.content {
            println!("{}  Content: {}", spacing, content);
        }
        
        // Recursively print all children with increased indentation
        for child in &self.children {
            child.print_tree(indent + 2);
        }
    }
    
    /// Returns the number of child nodes
    /// This is useful for various DOM operations
    /// 
    /// # Returns
    /// The count of children as a usize
    pub fn child_count(&self) -> usize {
        self.children.len()
    }
}

// Implement Debug trait for better debugging output
impl fmt::Debug for Node {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        write!(f, "Node(type={}, tag={:?})", self.node_type, self.tag)
    }
}

// Helper functions for creating specific types of nodes
// These make it easier to construct common node types

/// Creates a new element node with the specified tag name
/// This is a convenience function for creating element nodes
/// 
/// # Arguments
/// * `tag` - The HTML tag name (like "div", "p", "h1")
/// 
/// # Returns
/// A Node configured as an element with the specified tag
/// 
/// # Example
/// ```
/// let heading = create_element("h1");
/// let paragraph = create_element("p");
/// ```
pub fn create_element(tag: &str) -> Node {
    let mut node = Node::new("element".to_string());
    node.tag = Some(tag.to_string());
    node
}

/// Creates a new text node with the specified text content
/// This is a convenience function for creating text nodes
/// 
/// # Arguments
/// * `text` - The text content for the node
/// 
/// # Returns
/// A Node configured as a text node with the specified content
/// 
/// # Example
/// ```
/// let text = create_text("Hello, world!");
/// ```
pub fn create_text(text: &str) -> Node {
    let mut node = Node::new("text".to_string());
    node.content = Some(text.to_string());
    node
}

// Tests for the DOM node functionality
#[cfg(test)]
mod tests {
    use super::*;
    
    #[test]
    fn test_create_element() {
        let node = create_element("div");
        assert_eq!(node.node_type, "element");
        assert_eq!(node.tag, Some("div".to_string()));
    }
    
    #[test]
    fn test_create_text() {
        let node = create_text("test content");
        assert_eq!(node.node_type, "text");
        assert_eq!(node.content, Some("test content".to_string()));
    }
    
    #[test]
    fn test_add_child() {
        let mut parent = create_element("div");
        let child = create_text("child content");
        parent.add_child(child);
        assert_eq!(parent.child_count(), 1);
    }
}