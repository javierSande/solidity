contract C {
    function f() public pure {
        uint[] memory a;
        for (uncheckedArray(a) { uint x = 2 }; x < 2; x ++) {

        }
    }
}
// ----
// ParserError 6933: (57-66): Expected primary expression.
