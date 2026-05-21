// DOM Module
// This is the old DOM module - we might merge this with html_tree later
// For now keeping it for backward compatibility

pub mod node;

pub use node::{Node, create_element, create_text};