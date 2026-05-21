// DOM Node Implementation
// This represents a single node in the DOM tree
// Every element, text node, comment, etc. is a Node

use std::fmt;

/// The type of node this is
/// Similar to Firefox's nsIDOMNode
#[derive(Debug, Clone, PartialEq)]
pub enum NodeType {
    /// An HTML element like <div>, <p>, etc.
    Element,
    /// Plain text content
    Text,
    /// HTML comment
    Comment,
    /// The document itself
    Document,
}

/// A node in the DOM tree
/// This is the base type for all DOM objects
pub struct Node {
    /// What type of node this is
    pub node_type: NodeType,
    
    /// The node's data (tag name for elements, text for text nodes)
    pub data: String,
    
    /// Child nodes
    pub children: Vec<Node>,
    
    /// Parent node index (in the document's node list)
    /// Using Option because root has no parent
    pub parent: Option<usize>,
}

impl Node {
    /// Creates a new node
    /// This is the basic constructor for creating DOM nodes
    pub fn new(node_type: NodeType, data: String) -> Self {
        Node {
            node_type,
            data,
            children: Vec::new(),
            parent: None,
        }
    }
    
    /// Creates an element node with the given tag name
    pub fn element(tag_name: &str) -> Self {
        Node::new(NodeType::Element, tag_name.to_lowercase())
    }
    
    /// Creates a text node with the given content
    pub fn text(content: &str) -> Self {
        Node::new(NodeType::Text, content.to_string())
    }
    
    /// Adds a child node to this node
    pub fn append_child(&mut self, child: Node) {
        self.children.push(child);
    }
    
    /// Gets the tag name if this is an element node
    pub fn tag_name(&self) -> Option<&str> {
        if self.node_type == NodeType::Element {
            Some(&self.data)
        } else {
            None
        }
    }
    
    /// Gets the text content if this is a text node
    pub fn text_content(&self) -> Option<&str> {
        if self.node_type == NodeType::Text {
            Some(&self.data)
        } else {
            None
        }
    }
    
    /// Recursively gets all text from this node and descendants
    pub fn inner_text(&self) -> String {
        let mut text = String::new();
        self.collect_text(&mut text);
        text.trim().to_string()
    }
    
    /// Helper function to collect text recursively
    fn collect_text(&self, result: &mut String) {
        match self.node_type {
            NodeType::Text => {
                if !self.data.is_empty() {
                    result.push_str(&self.data);
                    result.push(' ');
                }
            }
            NodeType::Element => {
                for child in &self.children {
                    child.collect_text(result);
                }
            }
            _ => {}
        }
    }
    
    /// Checks if this node has any children
    pub fn has_children(&self) -> bool {
        !self.children.is_empty()
    }
}

impl fmt::Debug for Node {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        match self.node_type {
            NodeType::Element => {
                write!(f, "Element<{}>", self.data)?;
                if !self.children.is_empty() {
                    write!(f, " [{} children]", self.children.len())?;
                }
            }
            NodeType::Text => {
                let preview = if self.data.len() > 30 {
                    format!("{}...", &self.data[..30])
                } else {
                    self.data.clone()
                };
                write!(f, "Text(\"{}\")", preview)?;
            }
            _ => write!(f, "{:?}", self.node_type)?,
        }
        Ok(())
    }
}