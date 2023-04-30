contract C {
    uint[] a;
    function f(uint[] calldata cd_arr) public pure {
        uncheckedArray(cd_arr) {}
    }
}
// ----
// SyntaxError 5297: (65-74):  Unchecked access for calldata array is not possible.
