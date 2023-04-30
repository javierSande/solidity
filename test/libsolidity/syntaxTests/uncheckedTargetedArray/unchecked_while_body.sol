contract C {
    uint[] a;
    function f() public pure {
        uint[] memory b;
        while (true) uncheckedArray(a,b) {

        }
    }
}
// ----
// ParserError 5297: (65-74): "uncheckedArray" blocks can only be used inside regular blocks.
