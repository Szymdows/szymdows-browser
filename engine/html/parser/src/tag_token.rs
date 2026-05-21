// Tag-specific token information
// Defines which tags should be skipped, which are self-closing, etc.

/// Tags that should never be displayed (self-closing/void tags)
pub fn is_void_tag(tag: &str) -> bool {
    matches!(tag,
        "area" | "base" | "br" | "col" | "embed" | "hr" | "img" | "input" |
        "link" | "meta" | "param" | "source" | "track" | "wbr"
    )
}

/// Tags whose content should be completely ignored
pub fn is_script_style_tag(tag: &str) -> bool {
    matches!(tag, "script" | "style" | "noscript")
}

/// Tags that create block-level formatting
pub fn is_block_tag(tag: &str) -> bool {
    matches!(tag,
        "address" | "article" | "aside" | "blockquote" | "details" | "dialog" |
        "dd" | "div" | "dl" | "dt" | "fieldset" | "figcaption" | "figure" |
        "footer" | "form" | "h1" | "h2" | "h3" | "h4" | "h5" | "h6" |
        "header" | "hgroup" | "hr" | "li" | "main" | "nav" | "ol" | "p" |
        "pre" | "section" | "table" | "ul"
    )
}