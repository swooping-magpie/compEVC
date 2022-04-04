#include <string>

#include "token.hpp"

static const std::string keywords[] = {
    "boolean",
    "break",
    "continue",
    "else",
    "float",
    "for",
    "if",
    "int",
    "return",
    "void",
    "while",
    "+",
    "-",
    "*",
    "/",
    "!",
    "!=",
    "=",
    "==",
    "<",
    "<=",
    ">",
    ">=",
    "&&",
    "||",
    "{",
    "}",
    "(",
    ")",
    "[",
    "]",
    ";",
    ",",
    "<id>",
    "<int-literal>",
    "<float-literal>",
    "<boolean-literal>",
    "<string-literal>",
    "<error>",
    "$",
    "&",
    "!PLACEHOLDER_SHOULD_NEVER_APPEAR!",
    "<error - unterminated comment>",
    "<error - unterminated string>",
    "<error - illegal escape character>",
};

enum class TokenKind;

std::string spell(TokenKind tk) { return keywords[static_cast<int>(tk)]; }

static std::string pos_to_string(SourcePosition const &lhs,
                                 SourcePosition const &rhs) {
  return std::to_string(lhs.line_num) + "(" + std::to_string(lhs.col_pos) +
         ").." + std::to_string(rhs.line_num) + "(" +
         std::to_string(rhs.col_pos) + ")";
}

std::string to_string(Token t, char const *buf, uint32_t length) {
  return "Kind = " + std::to_string(static_cast<int>(t.kind)) + " [" +
         spell(t.kind) + "], spelling = \"" +
         std::string(buf + t.start_offset, buf + t.end_offset) +
         "\", position = " + pos_to_string(t.start_pos, t.end_pos);
  ;
}
