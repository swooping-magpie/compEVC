#pragma once
#include <string>

enum class TokenKind {
  BOOLEAN = 0,
  BREAK = 1,
  CONTINUE = 2,
  ELSE = 3,
  FLOAT = 4,
  FOR = 5,
  IF = 6,
  INT = 7,
  RETURN = 8,
  VOID = 9,
  WHILE = 10,

  // operators
  PLUS = 11,
  MINUS = 12,
  MULT = 13,
  DIV = 14,
  NOT = 15,
  NOTEQ = 16,
  EQ = 17,
  EQEQ = 18,
  LT = 19,
  LTEQ = 20,
  GT = 21,
  GTEQ = 22,
  ANDAND = 23,
  OROR = 24,

  // separators
  LCURLY = 25,
  RCURLY = 26,
  LPAREN = 27,
  RPAREN = 28,
  LBRACKET = 29,
  RBRACKET = 30,
  SEMICOLON = 31,
  COMMA = 32,

  // identifiers
  ID = 33,

  // literals
  INTLITERAL = 34,
  FLOATLITERAL = 35,
  BOOLEANLITERAL = 36,
  STRINGLITERAL = 37,

  // special tokens...
  ERROR = 38,
  EVC_EOF = 39,

  // EXTENDED VC tokens...
  AMPERSAND = 40,
  PLACEHOLDER = 41,
};

struct SourcePosition {
  int col_pos;
  int line_num;
};

struct Token {
  TokenKind kind;
  uint32_t start_offset;
  uint32_t end_offset;
  SourcePosition start_pos;
  SourcePosition end_pos;
  std::string spell(TokenKind tk);
  std::string to_string(Token t, char const *buf, uint32_t length);
};

inline void restart_token(Token *tk, TokenKind kind, uint32_t offset,
                          SourcePosition curr_pos) {
  tk->kind = TokenKind::PLACEHOLDER;
  tk->start_offset = offset;
  tk->end_offset = offset;
  tk->start_pos = curr_pos;
  tk->end_pos = curr_pos;
  return;
}
