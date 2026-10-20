// node server/test.js
const assert = require("node:assert");
const { add, list } = require("./server");

assert(!add("no newline"));
assert(!add("\nempty name"));
assert(add("Ann\nhi\tthere ✓"));
assert(add("Bob\nyo"));
assert.strictEqual(list(0), "1\tAnn\thi there\n2\tBob\tyo\n");
assert.strictEqual(list(1), "2\tBob\tyo\n");
assert.strictEqual(list(2), "");
console.log("ok");
