
#include <fstream>
#include <string>
#include <vector>

#include "token.hpp"

enum class ScannerMode {
  midDoubleSlashComment,
  midSlashDotComment,
  freshStart,
  maxMunchingCont,
  foundOneForwardSlash,
};

struct ScannerState {
  std::string buf;
  std::string prev_munch;
  SourcePosition start_pos;
  SourcePosition curr_pos;
  ScannerMode mode;
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

std::vector<Token> do_entire_scan(std::ifstream fs) {

  auto ret = std::vector<Token>{};

  // Initialise some state!
  //
  ScannerState sState = {.buf = {},
                         .prev_munch = {},
                         .start_pos = SourcePosition{1, 1},
                         .curr_pos = SourcePosition{1, 1},
                         .mode = ScannerMode::freshStart};

  while (!fs.eof()) {
    char c = fs.get();
    advance_state(sState, c, ret);
  }

  return ret;
}

void handle_simple_case(ScannerState &sState, TokenKind tKind,
                        std::vector<Token> &v) {
  // increment the current position
  ++sState.curr_pos.col_pos;
};

void update_pos(SourcePosition &pos, char c) {
  switch (c) {}
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

  default:
    assert(!"Unreachable");
  };
};

void advance_fresh_state(ScannerState &sState, char c, std::vector<Token> &v) {
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
  default:
    break;
  };

  return;
}