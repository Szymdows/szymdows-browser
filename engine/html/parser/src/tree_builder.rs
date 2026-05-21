// Tree Builder
// Constructs the DOM tree from tokens
// Similar to Firefox's nsHtml5TreeBuilder

use html_tree::{Node, NodeType, Document};
use crate::tokenizer::{Tokenizer, Token, TokenType};
use crate::tag_token::{is_script_style_tag, is_void_tag};

pub struct TreeBuilder {
    document: Document,
    stack: Vec<Node>,  // Stack of open elements
}

impl TreeBuilder {
    pub fn new() -> Self {
        TreeBuilder {
            document: Document::new(),
            stack: Vec::new(),
        }
    }
    
    pub fn parse(&mut self, html: &str) {
        let mut tokenizer = Tokenizer::new(html);
        
        while let Some(token) = tokenizer.next_token() {
            self.process_token(token);
        }
        
        // Close any remaining open tags
        while !self.stack.is_empty() {
            self.pop_element();
        }
    }
    
    fn process_token(&mut self, token: Token) {
        match token.token_type {
            TokenType::StartTag => {
                if let Some(tag_name) = &token.tag_name {
                    // Skip script and style tags entirely
                    if is_script_style_tag(tag_name) {
                        // Don't add to tree
                        return;
                    }
                    
                    let element = Node::element(tag_name);
                    
                    // Void elements don't have children
                    if is_void_tag(tag_name) {
                        self.append_to_current(element);
                    } else {
                        self.push_element(element);
                    }
                }
            }
            TokenType::EndTag => {
                if let Some(tag_name) = &token.tag_name {
                    self.close_tag(tag_name);
                }
            }
            TokenType::Text => {
                let text = token.data.trim();
                if !text.is_empty() {
                    let text_node = Node::text(text);
                    self.append_to_current(text_node);
                }
            }
            TokenType::Comment => {
                // Skip comments for now
            }
            TokenType::Doctype => {
                // Skip doctype
            }
        }
    }
    
    fn push_element(&mut self, element: Node) {
        self.stack.push(element);
    }
    
    fn pop_element(&mut self) {
        if let Some(element) = self.stack.pop() {
            if self.stack.is_empty() {
                // This is the root element
                self.document.set_root(element);
            } else {
                // Add to parent
                if let Some(parent) = self.stack.last_mut() {
                    parent.append_child(element);
                }
            }
        }
    }
    
    fn close_tag(&mut self, tag_name: &str) {
        // Find matching opening tag
        for i in (0..self.stack.len()).rev() {
            if let Some(current_tag) = self.stack[i].tag_name() {
                if current_tag == tag_name {
                    // Pop everything up to and including this tag
                    while self.stack.len() > i {
                        self.pop_element();
                    }
                    return;
                }
            }
        }
    }
    
    fn append_to_current(&mut self, node: Node) {
        if let Some(current) = self.stack.last_mut() {
            current.append_child(node);
        } else {
            // No open elements, this becomes the root
            self.document.set_root(node);
        }
    }
    
    pub fn into_document(self) -> Document {
        self.document
    }
}