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
                                       "$"};

enum class TokenKind;

std::string Token::spell(TokenKind tk) {
  return keywords[static_cast<int>(tk)];
}

std::string Token::to_string(Token t) { return std::string(); }
