// HTML Tokenizer
// Breaks HTML into tokens (tags, text, etc.)
// Similar to Firefox's nsHtml5Tokenizer

#[derive(Debug, Clone, PartialEq)]
pub enum TokenType {
    StartTag,
    EndTag,
    Text,
    Comment,
    Doctype,
}

#[derive(Debug, Clone)]
pub struct Token {
    pub token_type: TokenType,
    pub data: String,
    pub tag_name: Option<String>,
}

pub struct Tokenizer {
    input: Vec<char>,
    position: usize,
}

impl Tokenizer {
    pub fn new(input: &str) -> Self {
        Tokenizer {
            input: input.chars().collect(),
            position: 0,
        }
    }
    
    pub fn next_token(&mut self) -> Option<Token> {
        // Skip whitespace only at the very beginning
        if self.position == 0 {
            self.skip_whitespace_at_start();
        }
        
        if self.position >= self.input.len() {
            return None;
        }
        
        if self.current() == '<' {
            self.parse_tag()
        } else {
            self.parse_text()
        }
    }
    
    fn parse_tag(&mut self) -> Option<Token> {
        self.position += 1; // Skip '<'
        
        if self.position >= self.input.len() {
            return None;
        }
        
        // Check for comment
        if self.peek_str("!--") {
            return self.parse_comment();
        }
        
        // Check for doctype
        if self.peek_str("!") || self.peek_str("!DOCTYPE") || self.peek_str("!doctype") {
            return self.skip_doctype();
        }
        
        // Check for end tag
        if self.current() == '/' {
            self.position += 1;
            let tag_name = self.read_tag_name();
            self.skip_until('>');
            if self.position < self.input.len() {
                self.position += 1; // Skip '>'
            }
            
            return Some(Token {
                token_type: TokenType::EndTag,
                data: String::new(),
                tag_name: Some(tag_name.to_lowercase()),
            });
        }
        
        // Start tag
        let tag_name = self.read_tag_name();
        self.skip_until('>');
        if self.position < self.input.len() {
            self.position += 1; // Skip '>'
        }
        
        Some(Token {
            token_type: TokenType::StartTag,
            data: String::new(),
            tag_name: Some(tag_name.to_lowercase()),
        })
    }
    
    fn parse_text(&mut self) -> Option<Token> {
        let mut text = String::new();
        
        while self.position < self.input.len() && self.current() != '<' {
            text.push(self.current());
            self.position += 1;
        }
        
        Some(Token {
            token_type: TokenType::Text,
            data: text,
            tag_name: None,
        })
    }
    
    fn parse_comment(&mut self) -> Option<Token> {
        self.position += 3; // Skip "!--"
        let start = self.position;
        
        // Find end of comment
        while self.position < self.input.len() - 2 {
            if self.peek_str("-->") {
                let comment = self.input[start..self.position].iter().collect();
                self.position += 3;
                return Some(Token {
                    token_type: TokenType::Comment,
                    data: comment,
                    tag_name: None,
                });
            }
            self.position += 1;
        }
        
        None
    }
    
    fn skip_doctype(&mut self) -> Option<Token> {
        // Skip until we find '>'
        self.skip_until('>');
        if self.position < self.input.len() {
            self.position += 1; // Skip '>'
        }
        
        Some(Token {
            token_type: TokenType::Doctype,
            data: String::new(),
            tag_name: None,
        })
    }
    
    fn read_tag_name(&mut self) -> String {
        let mut name = String::new();
        
        while self.position < self.input.len() {
            let c = self.current();
            if c == '>' || c == ' ' || c == '\n' || c == '\t' || c == '/' {
                break;
            }
            name.push(c);
            self.position += 1;
        }
        
        name
    }
    
    fn skip_until(&mut self, ch: char) {
        while self.position < self.input.len() && self.current() != ch {
            self.position += 1;
        }
    }
    
    fn skip_whitespace_at_start(&mut self) {
        while self.position < self.input.len() {
            let c = self.current();
            if c != ' ' && c != '\n' && c != '\r' && c != '\t' {
                break;
            }
            self.position += 1;
        }
    }
    
    fn current(&self) -> char {
        if self.position < self.input.len() {
            self.input[self.position]
        } else {
            '\0'
        }
    }
    
    fn peek_str(&self, s: &str) -> bool {
        if self.position + s.len() > self.input.len() {
            return false;
        }
        
        let slice: String = self.input[self.position..self.position + s.len()].iter().collect();
        slice == s || slice.to_lowercase() == s.to_lowercase()
    }
}