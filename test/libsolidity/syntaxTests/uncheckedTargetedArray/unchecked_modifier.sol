contract C {
    uint[] a;

    modifier m() { uncheckedArray(a) { _; } }
}
// ----
// SyntaxError 2574: (44-45): The placeholder statement "_" cannot be used inside an "uncheckedArray" block.
