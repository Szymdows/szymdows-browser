// DOM Node - Legacy module
// This is kept for backward compatibility
// The new code should use html_tree instead

use std::fmt;

/// A node in the DOM tree
pub struct Node {
    pub node_type: String,
    pub tag: Option<String>,
    pub content: Option<String>,
    pub children: Vec<Node>,
}

impl Node {
    /// Creates a new node
    pub fn new(node_type: String) -> Node {
        Node {
            node_type: node_type,
            tag: None,
            content: None,
            children: Vec::new(),
        }
    }
    
    /// Adds a child node to this node
    pub fn add_child(&mut self, child: Node) {
        self.children.push(child);
    }
    
    /// Prints the node tree for debugging
    pub fn print_tree(&self, indent: usize) {
        let spacing = " ".repeat(indent);
        println!("{}Node Type: {:?}", spacing, self.node_type);
        if let Some(ref tag) = self.tag {
            println!("{}  Tag: {}", spacing, tag);
        }
        if let Some(ref content) = self.content {
            println!("{}  Content: {}", spacing, content);
        }
        
        for child in &self.children {
            child.print_tree(indent + 2);
        }
    }
    
    /// Returns the number of child nodes
    pub fn child_count(&self) -> usize {
        self.children.len()
    }
}

impl fmt::Debug for Node {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        write!(f, "Node(type={}, tag={:?})", self.node_type, self.tag)
    }
}

/// Creates a new element node with the specified tag name
pub fn create_element(tag: &str) -> Node {
    let mut node = Node::new("element".to_string());
    node.tag = Some(tag.to_string());
    node
}

/// Creates a new text node with the specified text content
pub fn create_text(text: &str) -> Node {
    let mut node = Node::new("text".to_string());
    node.content = Some(text.to_string());
    node
}

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