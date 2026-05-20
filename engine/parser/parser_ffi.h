#ifndef PARSER_FFI_H
#define PARSER_FFI_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

// This header defines the interface between C++ and Rust
// It declares the functions that Rust exports via FFI

#ifdef __cplusplus
extern "C" {
#endif

// Opaque pointer to Rust's NodeArray structure
// We don't need to know the internals, just hold a pointer to it
typedef struct NodeArray NodeArray;

// Parses HTML string and returns a pointer to the node array
// html_ptr: pointer to HTML string data
// html_len: length of the HTML string
// Returns: pointer to NodeArray (must be freed with free_node_array)
NodeArray* parse_html_to_nodes(const uint8_t* html_ptr, size_t html_len);

// Gets the number of nodes in the array
size_t get_node_count(const NodeArray* array_ptr);

// Gets the tag name pointer for a node at index
const uint8_t* get_node_tag(const NodeArray* array_ptr, size_t index);

// Gets the tag name length for a node at index
size_t get_node_tag_len(const NodeArray* array_ptr, size_t index);

// Gets the content pointer for a node at index
const uint8_t* get_node_content(const NodeArray* array_ptr, size_t index);

// Gets the content length for a node at index
size_t get_node_content_len(const NodeArray* array_ptr, size_t index);

// Checks if a node is a text node
bool get_node_is_text(const NodeArray* array_ptr, size_t index);

// Frees the node array memory
// MUST be called when done with the nodes to prevent memory leaks
void free_node_array(NodeArray* array_ptr);

#ifdef __cplusplus
}
#endif

#endif // PARSER_FFI_H