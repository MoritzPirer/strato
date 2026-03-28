#include "../../../inc/Controller/Rendering/CodeHiglightInfo.hpp"

CodeHighlightInfo CodeHighlightInfo::testPython() {
    CodeHighlightInfo info;
    
    info.m_language_names = {"python", "py"};
    info.m_language_keywords = {
        "and", "as", "assert", "async", "await", "break",
        "case", "class", "continue", "def", "del",
        "elif", "else", "except",
        "False", "finally", "for", "from", "global",
        "if", "import", "in", "is", "lambda",
        "match", "None", "nonlocal", "not",
        "or", "pass", "raise", "return",
        "True", "try", "type", "while", "with", "yield",
    };

    info.m_language_builtins = {
        "abs", "aiter", "all", "anext", "any", "ascii",
        "bin", "bool", "breakpoint", "bytearray", "bytes",
        "callable", "chr", "classmethod", "compile", "complex",
        "delattr", "dict", "dir", "divmod",
        "enumerate", "eval", "exec",
        "filter", "float", "format", "frozenset",
        "getattr", "globals",
        "hasattr", "hash", "help", "hex",
        "id", "__import__", "input", "int", "isinstance", "issubclass", "iter",
        "len", "list", "locals",
        "map", "max", "memoryview", "min", "next",
        "oct", "object", "open", "ord",
        "pow", "print", "property",
        "range", "repr", "return", "reversed", "round",
        "set", "setattr", "slice", "sorted", "staticmethod", "str", "sum", "super",
        "tuple", "type", "vars", "zip"
    };

    info.m_string_indicators = {
        {"\"","\""},
        {"f\"","\""},
        {"\'", "\'"},
        {"\"\"\"","\"\"\""},
        {"\'\'\'", "\'\'\'"}
    };

    info.m_token_splitters = " {}()[]<>:,.-+*/=#%";
    info.m_token_groups = {"\"\"\"", "\'\'\'", "/*", "*/", "=>"};
    info.m_ranged_comments = {
        {"/*", "*/"}
    };
    info.m_rest_of_line_comments = {"#"};
    info.m_number_seperator = '_';

    return info;
}