contract C {
    modifier m() { uncheckedArray { _; } }
}
// ----
// SyntaxError 2574: (44-45): The placeholder statement "_" cannot be used inside an "uncheckedArray" block.
