contract C {
    uint[] a;
    function f() public pure {
        uint[] memory b;
        for (uint x = 2; x < 2; uncheckedArray(a,b) { x ++; }) {

        }
    }
}
// ----
// ParserError 6933: (76-85): Expected primary expression.
