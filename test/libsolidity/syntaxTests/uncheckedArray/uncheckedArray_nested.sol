contract C {
    function f() public pure {
        uncheckedArray {
            uncheckedArray {
                uint x = 2 + 3;
            }
        }
    }
}
// ----
// SyntaxError 1941: (76-133): "uncheckedArray" blocks cannot be nested.
