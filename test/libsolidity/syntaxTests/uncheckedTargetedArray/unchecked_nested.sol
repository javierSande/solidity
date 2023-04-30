contract C {
    uint[] a;
    function f() public pure {
        uint[] memory b;
        uncheckedArray(a) {
            uncheckedArray(b) {
                uint x = 2 + 3;
            }
        }
    }
}
// ----
// SyntaxError 1941: (76-133): "uncheckedArray" blocks cannot be nested.
