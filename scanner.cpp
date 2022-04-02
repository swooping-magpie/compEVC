
#include <cassert>
#include <cstdint>
#include <fstream>
#include <string>
#include <vector>

#include "token.hpp"

#define CASE_DIGIT                                                             \
  '0' : case '1' : case '2' : case '3' : case '4' : case '5' : case '6'        \
      : case '7' : case '8' : case '9'

#define CASE_LETTER                                                            \
  'A' : case 'B' : case 'C' : case 'D' : case 'E' : case 'F' : case 'G'        \
      : case 'H' : case 'I' : case 'J' : case 'K' : case 'L' : case 'M'        \
      : case 'N' : case 'O' : case 'P' : case 'Q' : case 'R' : case 'S'        \
      : case 'T' : case 'U' : case 'V' : case 'W' : case 'X' : case 'Y'        \
      : case 'Z' : case 'a' : case 'b' : case 'c' : case 'd' : case 'e'        \
      : case 'f' : case 'g' : case 'h' : case 'i' : case 'j' : case 'k'        \
      : case 'l' : case 'm' : case 'n' : case 'o' : case 'p' : case 'q'        \
      : case 'r' : case 's' : case 't' : case 'u' : case 'v' : case 'w'        \
      : case 'x' : case 'y' : case 'z':

enum class ScannerMode {
  midDoubleSlashComment,
  midSlashDotComment,
  freshStart,
  maxMunchingCont,
  foundOneForwardSlash,
  foundEquals,
  foundExclamation,
  foundLT,
  foundGT,
  foundAmp,
  foundStick,
  foundPeriod,
};

struct MaybeToken {
  Token tk;
  bool is_token;
};

static inline bool isLetter(char c) {
  return (('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z'));
}

static inline bool isNumber(char c) { return ('0' <= c && c <= '9'); }

static inline bool isLetterOrUndersocre(char c) {
  return isLetter(c) || (c == '_');
}

static inline bool isNumberOrLetterOrUnderscore(char c) {
  return isLetter(c) || isNumber(c) || (c == '_');
}

void advance_state(ScannerState &sState, char c, std::vector<Token> &v);

std::vector<Token> do_entire_scan(char const *start, uint32_t length) {
  auto ret = std::vector<Token>{};

  // Initialise some state!
  //
  uint32_t offset = 0;
  ScannerMode mode = ScannerMode::freshStart;
  SourcePosition curr_pos = SourcePosition{1, 1};

  auto curr_token_fragment = Token{
      .kind = TokenKind::PLACEHOLDER,
      .start_offset = offset,
      .end_offset = offset,
      .start_pos = curr_pos,
      .end_pos = curr_pos,
  };

  char c = start[offset];
  while (offset < length) {
    assert(c == start[offset]);
    switch (mode) {
    case ScannerMode::freshStart:
      switch (c) {}
    default:
      assert(!"Unreachable");
    };
  }
}
return ret;
}

void handle_simple_case(ScannerState &sState, TokenKind tKind,
                        std::vector<Token> &v) {
  // increment the current position
  ++sState.curr_pos.col_pos;
};

void update_pos(SourcePosition &pos, char c) {
  switch (c) {
  case '\n':
    pos.col_pos = 1;
    ++pos.line_num;
    break;
  case '\t':
    do {
      ++pos.col_pos;
    } while (pos.line_num % 8 != 1);
    break;
  default:
    ++pos.col_pos;
    break;
  }
}

void advance_fresh_state(ScannerState &sState, char c, std::vector<Token> &v);
void advance_max_munch(ScannerState &sState, char c, std::vector<Token> &v);
void advance_one_slash(ScannerState &sState, char c, std::vector<Token> &v);
void advance_middledouble_slash(ScannerState &sState, char c,
                                std::vector<Token> &v);
void advance_middleslashdot(ScannerState &sState, char c,
                            std::vector<Token> &v);

void advance_state(ScannerState &sState, char c, std::vector<Token> &v) {

  update_pos(sState.curr_pos, c);

  switch (sState.mode) {
  case ScannerMode::freshStart:
    advance_fresh_state(sState, c, v);
    return;

  case ScannerMode::maxMunchingCont:
    advance_max_munch(sState, c, v);
    return;

  case ScannerMode::foundOneForwardSlash:
    advance_one_slash(sState, c, v);
    return;

  case ScannerMode::midDoubleSlashComment:
    advance_middledouble_slash(sState, c, v);
    return;

  case ScannerMode::midSlashDotComment:
    advance_middleslashdot(sState, c, v);
    return;

  case ScannerMode::foundEquals:
    if (c == '=') {
    }
    return

        default : assert(!"Unreachable");
  };
};

static inline void advance_fresh_state(char c, ) {
  switch (c) {
  case '(':
    handle_simple_case(sState, TokenKind::LPAREN, v);
    return;
  case ')':
    handle_simple_case(sState, TokenKind::RPAREN, v);
    return;
  case '{':
    handle_simple_case(sState, TokenKind::LCURLY, v);
    return;
  case '}':
    handle_simple_case(sState, TokenKind::RCURLY, v);
    return;
  case '[':
    handle_simple_case(sState, TokenKind::LBRACKET, v);
    return;
  case ']':
    handle_simple_case(sState, TokenKind::RBRACKET, v);
    return;
  case ';':
    handle_simple_case(sState, TokenKind::SEMICOLON, v);
    return;
  case ',':
    handle_simple_case(sState, TokenKind::COMMA, v);
    return;
  case '+':
    handle_simple_case(sState, TokenKind::PLUS, v);
    return;
  case '-':
    handle_simple_case(sState, TokenKind::MINUS, v);
    return;
  case ' ':
    // handle blank space

  default:
    break;
  };

  return;
}