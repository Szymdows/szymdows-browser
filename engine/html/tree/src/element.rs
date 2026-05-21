// HTML Element Implementation
// Represents an HTML element with attributes
// Similar to Firefox's dom/base/Element

use std::collections::HashMap;

/// An HTML element with attributes
/// This extends Node with element-specific functionality
pub struct Element {
    /// The tag name (like "div", "p", "h1")
    pub tag_name: String,
    
    /// Element attributes (like id="main", class="content")
    pub attributes: HashMap<String, String>,
    
    /// The element's ID (if it has one)
    pub id: Option<String>,
    
    /// The element's classes
    pub classes: Vec<String>,
}

impl Element {
    /// Creates a new element with the given tag name
    pub fn new(tag_name: &str) -> Self {
        Element {
            tag_name: tag_name.to_lowercase(),
            attributes: HashMap::new(),
            id: None,
            classes: Vec::new(),
        }
    }
    
    /// Sets an attribute on this element
    pub fn set_attribute(&mut self, name: &str, value: &str) {
        let name_lower = name.to_lowercase();
        
        // Handle special attributes
        if name_lower == "id" {
            self.id = Some(value.to_string());
        } else if name_lower == "class" {
            self.classes = value.split_whitespace()
                               .map(|s| s.to_string())
                               .collect();
        }
        
        self.attributes.insert(name_lower, value.to_string());
    }
    
    /// Gets an attribute value
    pub fn get_attribute(&self, name: &str) -> Option<&String> {
        self.attributes.get(&name.to_lowercase())
    }
    
    /// Checks if element has an attribute
    pub fn has_attribute(&self, name: &str) -> bool {
        self.attributes.contains_key(&name.to_lowercase())
    }
    
    /// Checks if element has a specific class
    pub fn has_class(&self, class_name: &str) -> bool {
        self.classes.iter().any(|c| c == class_name)
    }
}