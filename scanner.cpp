#include <cassert>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <string>
#include <vector>

#include "token.hpp"

template <size_t N, typename T>
constexpr size_t const_size_of(T const (&arr)[N]) {
  return N;
}

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
      : case 'x' : case 'y' : case 'z'

struct StringKeyword {
  char const *str;
  TokenKind tk;
};

static const StringKeyword keywords[] = {
    {"boolean", TokenKind::BOOLEAN},
    {"break", TokenKind::BREAK},
    {"continue", TokenKind::CONTINUE},
    {"else", TokenKind::ELSE},
    {"float", TokenKind::FLOAT},
    {"for", TokenKind::FOR},
    {"if", TokenKind::IF},
    {"int", TokenKind::INT},
    {"return", TokenKind::RETURN},
    {"void", TokenKind::VOID},
    {"while", TokenKind::WHILE},
};

static constexpr size_t size_of_keywords = const_size_of(keywords);

static_assert(size_of_keywords == 11, "can't count rip\n");

TokenKind process_identifier(char const *start, uint32_t length,
                             uint32_t start_offset, uint32_t end_offset) {
  for (size_t i = 0; i < const_size_of(keywords); ++i) {
    uint8_t broken = 0;
    for (uint32_t j = 0; j < (end_offset - start_offset); ++j) {
      if (start[start_offset + j] != keywords[i].str[j]) {
        broken = 1;
        break;
      }
      if (keywords[i].str[j] == '\0') {
        broken = 1;
        break;
      }
    }
    if (broken == 0) {
      return keywords[i].tk;
    } else {
      continue;
    }
  }
  return TokenKind::ID;
}

enum class ScannerMode {
  midStringLit,
  foundBackwardsSlashMidStringLit,
  midDoubleSlashComment,
  midSlashDotComment,
  threeQuartersThruSlashDotComment,
  freshStart,
  maxMunchingCont,
  foundOneForwardSlash,
  foundEquals,
  foundExclamation,
  foundLT,
  foundGT,
  foundAmp,
  foundStick,
  foundDot,
  foundSlashR,
  foundSlashRMidSlashDotComment,
  foundQuotationMark,
  foundDigit,
  foundFractionalPartAfterInt,
  foundEAfterNumber,
  foundSignAfterEAfterNumber,
  foundDigitAfterExponentNumber,
  foundLetter,
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

std::vector<Token> do_scan(char const *start, uint32_t length) {
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
      case '"':
        restart_token(&curr_token_fragment, TokenKind::STRINGLITERAL,
                      offset + 1, curr_pos);
        mode = ScannerMode::foundQuotationMark;
        break;
      case CASE_DIGIT:
        restart_token(&curr_token_fragment, TokenKind::INTLITERAL, offset,
                      curr_pos);

        mode = ScannerMode::foundDigit;
        break;
      case '_':
      case CASE_LETTER:
        restart_token(&curr_token_fragment, TokenKind::ID, offset, curr_pos);

        mode = ScannerMode::foundLetter;
        break;
      case '.':
        restart_token(&curr_token_fragment, TokenKind::ERROR, offset, curr_pos);

        mode = ScannerMode::foundDot;
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
        restart_token(&curr_token_fragment, TokenKind::AMPERSAND, offset,
                      curr_pos);
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
      case '/':
        restart_token(&curr_token_fragment, TokenKind::DIV, offset, curr_pos);
        curr_token_fragment.end_offset = offset + 1;
        curr_token_fragment.end_pos = curr_pos;

        mode = ScannerMode::foundOneForwardSlash;
        // ret.push_back(curr_token_fragment);
        break;
      }
      break;
    case ScannerMode::foundLetter:
      switch (c) {
      case CASE_LETTER:
      case CASE_DIGIT:
      case '_':
        curr_token_fragment.end_offset = offset + 1;
        curr_token_fragment.end_pos = curr_pos;
        break;
      default:
        curr_token_fragment.kind =
            process_identifier(start, length, curr_token_fragment.start_offset,
                               curr_token_fragment.end_offset);

        ret.push_back(curr_token_fragment);

        mode = ScannerMode::freshStart;
        continue;
      }
      break;
    case ScannerMode::foundDot:
      switch (c) {
      case CASE_DIGIT:
        curr_token_fragment.kind = TokenKind::FLOATLITERAL;
        curr_token_fragment.end_offset = offset + 1;
        curr_token_fragment.end_pos = curr_pos;

        mode = ScannerMode::foundFractionalPartAfterInt;
        break;
      default:
        ret.push_back(curr_token_fragment);

        mode = ScannerMode::freshStart;
        continue;
      }
      break;
    case ScannerMode::foundDigit:
      switch (c) {
      case CASE_DIGIT:
        curr_token_fragment.end_offset = offset + 1;
        curr_token_fragment.end_pos = curr_pos;
        break;
      case '.':
        curr_token_fragment.kind = TokenKind::FLOATLITERAL;
        curr_token_fragment.end_offset = offset + 1;
        curr_token_fragment.end_pos = curr_pos;

        mode = ScannerMode::foundFractionalPartAfterInt;
        break;
      case 'E':
      case 'e':
        ret.push_back(curr_token_fragment);

        restart_token(&curr_token_fragment, TokenKind::ID, offset, curr_pos);
        mode = ScannerMode::foundEAfterNumber;
        break;
      default:
        ret.push_back(curr_token_fragment);

        mode = ScannerMode::freshStart;
        continue;
      }
      break;
    case ScannerMode::foundFractionalPartAfterInt:
      switch (c) {
      case CASE_DIGIT:
        curr_token_fragment.end_offset = offset + 1;
        curr_token_fragment.end_pos = curr_pos;
        break;
      case 'E':
      case 'e':
        ret.push_back(curr_token_fragment);

        restart_token(&curr_token_fragment, TokenKind::ID, offset, curr_pos);
        mode = ScannerMode::foundEAfterNumber;
        break;
      default:
        ret.push_back(curr_token_fragment);
        mode = ScannerMode::freshStart;
        continue;
      }
      break;
    case ScannerMode::foundEAfterNumber:
      switch (c) {
      case '+':
      case '-':
        ret.push_back(curr_token_fragment);
        restart_token(&curr_token_fragment, TokenKind::PLUS, offset, curr_pos);

        mode = ScannerMode::foundSignAfterEAfterNumber;
        break;
      case CASE_DIGIT:
        // note that we have an E as current token, and a number as the last
        // element of ret
        curr_token_fragment = ret.back();
        ret.pop_back();

        curr_token_fragment.end_offset = offset + 1;
        curr_token_fragment.end_pos = curr_pos;
        curr_token_fragment.kind = TokenKind::FLOATLITERAL;

        mode = ScannerMode::foundDigitAfterExponentNumber;
        break;
      default:
        ret.push_back(curr_token_fragment);
        mode = ScannerMode::freshStart;
        continue;
      }
      break;
    case ScannerMode::foundSignAfterEAfterNumber:
      switch (c) {
      case CASE_DIGIT:
        // note that we have an + as current token, and an 'E' and a number as
        // the last 2 element of ret
        ret.pop_back();
        curr_token_fragment = ret.back();
        ret.pop_back();

        curr_token_fragment.end_offset = offset + 1;
        curr_token_fragment.end_pos = curr_pos;
        curr_token_fragment.kind = TokenKind::FLOATLITERAL;

        mode = ScannerMode::foundDigitAfterExponentNumber;
        break;
      default:
        ret.push_back(curr_token_fragment);
        mode = ScannerMode::freshStart;
        continue;
      }
      break;
    case ScannerMode::foundDigitAfterExponentNumber:
      switch (c) {
      case CASE_DIGIT:
        curr_token_fragment.end_offset = offset + 1;
        curr_token_fragment.end_pos = curr_pos;
        break;
      default:
        ret.push_back(curr_token_fragment);

        mode = ScannerMode::freshStart;
        continue;
      }
      break;
    case ScannerMode::foundQuotationMark:
      switch (c) {
      default:
        mode = ScannerMode::midStringLit;
        continue;
      }
      break;
    case ScannerMode::midStringLit:
      switch (c) {
      case '"':
        curr_token_fragment.end_pos = curr_pos;
        ret.push_back(curr_token_fragment);

        mode = ScannerMode::freshStart;
        break;
      case '\n':
        curr_token_fragment.end_pos = curr_pos;
        curr_token_fragment.kind = TokenKind::ERROR_UNTERMINATED_STRING;
        ret.push_back(curr_token_fragment);

        mode = ScannerMode::freshStart;
        break;
      case '\r':
        curr_token_fragment.end_pos = curr_pos;
        curr_token_fragment.kind = TokenKind::ERROR_UNTERMINATED_STRING;
        ret.push_back(curr_token_fragment);

        mode = ScannerMode::foundSlashR;
        break;

      case '\\':
        curr_token_fragment.end_offset = offset + 1;
        curr_token_fragment.end_pos = curr_pos;

        mode = ScannerMode::foundBackwardsSlashMidStringLit;
        break;
      default:
        curr_token_fragment.end_offset = offset + 1;
        curr_token_fragment.end_pos = curr_pos;
        break;
      }
      break;
    case ScannerMode::foundBackwardsSlashMidStringLit:
      switch (c) {
      case 'n':
      case 'b':
      case 'f':
      case 'r':
      case 't':
      case '\'':
      case '"':
      case '\\':
        curr_token_fragment.end_offset = offset + 1;
        curr_token_fragment.end_pos = curr_pos;

        mode = ScannerMode::midStringLit;
        break;
      default:
        curr_token_fragment.end_offset = offset + 1;
        curr_token_fragment.end_pos = curr_pos;
        curr_token_fragment.kind =
            TokenKind::ERROR_STRINGLIT_WITH_ILLEGAL_ESCAPE_CHAR;

        mode = ScannerMode::midStringLit;
        break;
      }
      break;
    case ScannerMode::foundOneForwardSlash:
      switch (c) {
      case '/':
        mode = ScannerMode::midDoubleSlashComment;
        break;
      case '*':
        mode = ScannerMode::midSlashDotComment;
        break;
      default:
        ret.push_back(curr_token_fragment);
        mode = ScannerMode::freshStart;
        continue;
      }
      break;
    case ScannerMode::midDoubleSlashComment:
      switch (c) {
      case '\n':
        mode = ScannerMode::freshStart;
        break;
      case '\r':
        mode = ScannerMode::foundSlashR;
        break;
      default:
        break;
      }
      break;
    case ScannerMode::midSlashDotComment:
      switch (c) {
      case '*':
        mode = ScannerMode::threeQuartersThruSlashDotComment;
        break;
      case '\r':
        mode = ScannerMode::foundSlashRMidSlashDotComment;
        break;
      default:
        break;
      }
      break;
    case ScannerMode::threeQuartersThruSlashDotComment:
      switch (c) {
      case '/':
        mode = ScannerMode::freshStart;
        break;
      case '*':
        // stay in the same state!
        break;
      case '\r':
        mode = ScannerMode::foundSlashRMidSlashDotComment;
        break;
      default:
        mode = ScannerMode::midSlashDotComment;
        break;
      }
      break;
    case ScannerMode::foundSlashRMidSlashDotComment:
      switch (c) {
      case '\n':
        ++offset;
        mode = ScannerMode::midSlashDotComment;
        continue;
      default:
        mode = ScannerMode::midSlashDotComment;
        continue;
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

  ret.push_back(curr_token_fragment);

  // final token
  restart_token(&curr_token_fragment, TokenKind::EVC_EOF, offset, curr_pos);
  curr_token_fragment.end_offset = offset;
  curr_token_fragment.end_pos = curr_pos;
  ret.push_back(curr_token_fragment);
  return ret;
}
