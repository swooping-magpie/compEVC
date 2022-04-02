#include <string>

#include "token.hpp"

static const std::string keywords[] = {"boolean",
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
                                       "PLACEHOLDER_SHOULD_NEVER_APPEAR"};

enum class TokenKind;

std::string Token::spell(TokenKind tk) {
  return keywords[static_cast<int>(tk)];
}

std::string Token::to_string(Token t, char const *buf, uint32_t length) {
  return std::string();
}
