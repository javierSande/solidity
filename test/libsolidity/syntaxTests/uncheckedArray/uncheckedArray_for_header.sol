contract C {
    function f() public pure {
        for (uncheckedArray { uint x = 2 }; x < 2; x ++) {

        }
    }
}
// ----
// ParserError 6933: (57-66): Expected primary expression.
