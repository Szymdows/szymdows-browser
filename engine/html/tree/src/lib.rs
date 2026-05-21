// HTML Tree Module
// This module defines the DOM tree structure
// Similar to Firefox's dom/base

pub mod node;
pub mod element;
pub mod document;

pub use node::{Node, NodeType};
pub use element::Element;
pub use document::Document;