contract Base {
    uint public m_x;
    bytes m_s;
    constructor(uint x, bytes memory s) {
        m_x = x;
        m_s = s;
    }
    function part(uint i) public returns (bytes1) {
        return m_s[i];
    }
}
contract Main is Base {
    constructor(bytes memory s, uint x) Base(x, f(s)) {}
    function f(bytes memory s) public returns (bytes memory) {
        return s;
    }
}
contract Creator {
    function f(uint x, bytes memory s) public returns (uint r, bytes1 ch) {
        Main c = new Main(s, x);
        r = c.m_x();
        ch = c.part(x);
    }
}
// ----
// f(uint256,bytes): 7, 0x40, 78, "abcdefghijklmnopqrstuvwxyzabcdef", "ghijklmnopqrstuvwxyzabcdefghijkl", "mnopqrstuvwxyz" -> 7, "h"
// gas irOptimized: 169902
// gas irOptimized code: 105200
// gas legacy: 173430
// gas legacy code: 245000
// gas legacyOptimized: 170363
// gas legacyOptimized code: 122200
