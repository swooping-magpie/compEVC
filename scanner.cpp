
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
  foundSlashR,
};

void move_up_tab(SourcePosition *pos) {
  do {
    ++pos->col_pos;
  } while (pos->line_num % 8 != 1);
  return;
};
void move_up_newline(SourcePosition *pos) {
  pos->col_pos = 1;
  ++pos->line_num;
  return;
};

void move_up_space(SourcePosition *pos) { ++pos->col_pos; }

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

  while (offset < length) {
    char c = start[offset];

    switch (mode) {
    case ScannerMode::freshStart:
      switch (c) {
      case '\t':
      case ' ':
      case '\n':
        break;
      case '\r':
        mode = ScannerMode::foundSlashR;
        break;
      case '(':
        restart_token(&curr_token_fragment, TokenKind::LPAREN, offset,
                      curr_pos);
        curr_token_fragment.end_offset = offset + 1;
        curr_token_fragment.end_pos = curr_pos;
        ret.push_back(curr_token_fragment);
        break;
      case ')':
        restart_token(&curr_token_fragment, TokenKind::RPAREN, offset,
                      curr_pos);
        curr_token_fragment.end_offset = offset + 1;
        curr_token_fragment.end_pos = curr_pos;
        ret.push_back(curr_token_fragment);
        break;
      case '{':
        restart_token(&curr_token_fragment, TokenKind::LCURLY, offset,
                      curr_pos);
        curr_token_fragment.end_offset = offset + 1;
        curr_token_fragment.end_pos = curr_pos;
        ret.push_back(curr_token_fragment);
        break;
      case '}':
        restart_token(&curr_token_fragment, TokenKind::RCURLY, offset,
                      curr_pos);
        curr_token_fragment.end_offset = offset + 1;
        curr_token_fragment.end_pos = curr_pos;
        ret.push_back(curr_token_fragment);
        break;
      case '[':
        restart_token(&curr_token_fragment, TokenKind::LBRACKET, offset,
                      curr_pos);
        curr_token_fragment.end_offset = offset + 1;
        curr_token_fragment.end_pos = curr_pos;
        ret.push_back(curr_token_fragment);
        break;
      case ']':
        restart_token(&curr_token_fragment, TokenKind::RBRACKET, offset,
                      curr_pos);
        curr_token_fragment.end_offset = offset + 1;
        curr_token_fragment.end_pos = curr_pos;
        ret.push_back(curr_token_fragment);
        break;
      case ';':
        restart_token(&curr_token_fragment, TokenKind::SEMICOLON, offset,
                      curr_pos);
        curr_token_fragment.end_offset = offset + 1;
        curr_token_fragment.end_pos = curr_pos;
        ret.push_back(curr_token_fragment);
        break;
      case ',':
        restart_token(&curr_token_fragment, TokenKind::COMMA, offset, curr_pos);
        curr_token_fragment.end_offset = offset + 1;
        curr_token_fragment.end_pos = curr_pos;
        ret.push_back(curr_token_fragment);
        break;
      case '+':
        restart_token(&curr_token_fragment, TokenKind::PLUS, offset, curr_pos);
        curr_token_fragment.end_offset = offset + 1;
        curr_token_fragment.end_pos = curr_pos;
        ret.push_back(curr_token_fragment);
        break;
      case '-':
        restart_token(&curr_token_fragment, TokenKind::MINUS, offset, curr_pos);
        curr_token_fragment.end_offset = offset + 1;
        curr_token_fragment.end_pos = curr_pos;
        ret.push_back(curr_token_fragment);
        break;
      case '*':
        restart_token(&curr_token_fragment, TokenKind::MULT, offset, curr_pos);
        curr_token_fragment.end_offset = offset + 1;
        curr_token_fragment.end_pos = curr_pos;
        ret.push_back(curr_token_fragment);
        break;
      case '/':
        restart_token(&curr_token_fragment, TokenKind::DIV, offset, curr_pos);
        curr_token_fragment.end_offset = offset + 1;
        curr_token_fragment.end_pos = curr_pos;
        ret.push_back(curr_token_fragment);
        break;
      case '!':
        restart_token(&curr_token_fragment, TokenKind::NOT, offset, curr_pos);
        curr_token_fragment.end_offset = offset + 1;
        curr_token_fragment.end_pos = curr_pos;

        mode = ScannerMode::foundExclamation;
        break;
      case '=':
        restart_token(&curr_token_fragment, TokenKind::EQ, offset, curr_pos);
        curr_token_fragment.end_offset = offset + 1;
        curr_token_fragment.end_pos = curr_pos;

        mode = ScannerMode::foundEquals;
        break;

      case '<':
        restart_token(&curr_token_fragment, TokenKind::LT, offset, curr_pos);
        curr_token_fragment.end_offset = offset + 1;
        curr_token_fragment.end_pos = curr_pos;

        mode = ScannerMode::foundLT;
        break;

      case '>':
        restart_token(&curr_token_fragment, TokenKind::GT, offset, curr_pos);
        curr_token_fragment.end_offset = offset + 1;
        curr_token_fragment.end_pos = curr_pos;

        mode = ScannerMode::foundGT;
        break;

      case '&':
        restart_token(&curr_token_fragment, TokenKind::ERROR, offset, curr_pos);
        curr_token_fragment.end_offset = offset + 1;
        curr_token_fragment.end_pos = curr_pos;

        mode = ScannerMode::foundAmp;
        break;

      case '|':
        restart_token(&curr_token_fragment, TokenKind::ERROR, offset, curr_pos);
        curr_token_fragment.end_offset = offset + 1;
        curr_token_fragment.end_pos = curr_pos;

        mode = ScannerMode::foundStick;
        break;
      }
      break;
    case ScannerMode::foundSlashR:
      switch (c) {
      case '\n':
        ++offset;

        mode = ScannerMode::freshStart;
        continue;
      default:
        mode = ScannerMode::freshStart;
        continue;
      }
      break;
    case ScannerMode::foundExclamation:
      switch (c) {
      case '=':
        curr_token_fragment.kind = TokenKind::NOTEQ;
        curr_token_fragment.end_offset = offset + 1;
        curr_token_fragment.end_pos = curr_pos;
        ret.push_back(curr_token_fragment);

        mode = ScannerMode::freshStart;
        break;
      default:
        // push back what is currently there
        ret.push_back(curr_token_fragment);
        mode = ScannerMode::freshStart;
        continue;
      }
      break;
    case ScannerMode::foundEquals:
      switch (c) {
      case '=':
        curr_token_fragment.kind = TokenKind::EQEQ;
        curr_token_fragment.end_offset = offset + 1;
        curr_token_fragment.end_pos = curr_pos;
        ret.push_back(curr_token_fragment);

        mode = ScannerMode::freshStart;
        break;
      default:
        // push back what is currently there
        ret.push_back(curr_token_fragment);
        mode = ScannerMode::freshStart;
        continue;
      }
      break;

    case ScannerMode::foundLT:
      switch (c) {
      case '=':
        curr_token_fragment.kind = TokenKind::LTEQ;
        curr_token_fragment.end_offset = offset + 1;
        curr_token_fragment.end_pos = curr_pos;
        ret.push_back(curr_token_fragment);

        mode = ScannerMode::freshStart;
        break;
      default:
        // push back what is currently there
        ret.push_back(curr_token_fragment);
        mode = ScannerMode::freshStart;
        continue;
      }
      break;

    case ScannerMode::foundGT:
      switch (c) {
      case '=':
        curr_token_fragment.kind = TokenKind::GTEQ;
        curr_token_fragment.end_offset = offset + 1;
        curr_token_fragment.end_pos = curr_pos;
        ret.push_back(curr_token_fragment);

        mode = ScannerMode::freshStart;
        break;
      default:
        // push back what is currently there
        ret.push_back(curr_token_fragment);
        mode = ScannerMode::freshStart;
        continue;
      }
      break;

    case ScannerMode::foundAmp:
      switch (c) {
      case '&':
        curr_token_fragment.kind = TokenKind::ANDAND;
        curr_token_fragment.end_offset = offset + 1;
        curr_token_fragment.end_pos = curr_pos;
        ret.push_back(curr_token_fragment);

        mode = ScannerMode::freshStart;
        break;
      default:
        // push back what is currently there
        // this is an error token!
        ret.push_back(curr_token_fragment);
        mode = ScannerMode::freshStart;
        continue;
      }
      break;

    case ScannerMode::foundStick:
      switch (c) {
      case '|':
        curr_token_fragment.kind = TokenKind::OROR;
        curr_token_fragment.end_offset = offset + 1;
        curr_token_fragment.end_pos = curr_pos;
        ret.push_back(curr_token_fragment);

        mode = ScannerMode::freshStart;
        break;
      default:
        // push back what is currently there
        // this is an error token!
        ret.push_back(curr_token_fragment);
        mode = ScannerMode::freshStart;
        continue;
      }
      break;

    default:
      assert(!"Unreachable");
    };

    ++offset;

    switch (c) {
    case '\t':
      move_up_tab(&curr_pos);
      break;
    case '\n':
      move_up_newline(&curr_pos);
      break;
    case '\r':
      move_up_newline(&curr_pos);
      break;
    default:
      move_up_space(&curr_pos);
      break;
    }
  }

  return ret;
}

void handle_simple_case(ScannerState &sState, TokenKind tKind,
                        std::vector<Token> &v) {
  // increment the current position
  ++sState.curr_pos.col_pos;
};

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