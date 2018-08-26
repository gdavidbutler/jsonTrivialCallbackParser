## jsonTrivialCallbackParser
Trivial [JSON](https://tools.ietf.org/rfc/rfc8259.txt) callback parser in standard C.

No dependencies. No dynamic memory allocation. No recursion. No memory copying. Small and Fast.

No JSON validation! To simply "*check*" that a JSON string can be parsed, use a NULL callback and verify the return offset is the same as the source length.

Also included are some "coders", often used by JSON:

* URI encode and decode
* Base64 encode and decode

Some examples of using this:

* test/main.c: example of using the coders (./main coder) and parsing nasty JSON constructs (./main 1 test/test.json).
* test/print.c: JSON on standard input and indented JSON on standard output (./print < test/test.json).

Note: test.json is the file pass1.json from a zip file [here](http://www.json.org/JSON_checker/test.zip).

After reviewing many JSON parsers I started to doubt the claim that [JSON](http://json.org/) "...is easy for machines to parse...". However, looking at how trivial this parser is compaired to my trivial XML parser, I now believe it.

In implementing this parser, I discovered an interesting insight into the "trailing comma" issue. Come to find out, a comma has no more semantic significance than whitespace. (Assuming, of course, implied null array elements, e.g. "[,1]", are not allowed.)
