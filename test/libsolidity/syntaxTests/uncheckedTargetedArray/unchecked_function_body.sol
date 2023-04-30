function f() pure returns (uint) uncheckedArray() {}
// ----
// ParserError 5297: (33-42): "uncheckedArray" blocks can only be used inside regular blocks.
