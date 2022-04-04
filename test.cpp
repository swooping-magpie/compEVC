#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "scanner.hpp"
#include "token.hpp"

#include <string>

template <size_t N, typename T>
constexpr size_t const_size_of(T const (&arr)[N]) {
  return N;
}

int factorial(int number) {
  return number <= 1 ? number : factorial(number - 1) * number;
}

TEST_CASE("testing the factorial function") {
  CHECK(factorial(1) == 1);
  CHECK(factorial(2) == 2);
  CHECK(factorial(3) == 6);
  CHECK(factorial(10) == 3628800);
}

TEST_CASE("basic parsing test") {
  char const src_file[] = R"(1.2e+ 2)";
  std::vector<Token> const v1 = do_scan(src_file, const_size_of(src_file));
  std::vector<Token> const v2 = std::vector<Token>{
      Token{.kind = TokenKind::FLOATLITERAL,
            .start_offset = 0,
            .end_offset = 3,
            .start_pos = SourcePosition{1, 1},
            .end_pos = SourcePosition{1, 3}},
      Token{.kind = TokenKind::ID,
            .start_offset = 3,
            .end_offset = 4,
            .start_pos = SourcePosition{1, 4},
            .end_pos = SourcePosition{1, 4}},
      Token{.kind = TokenKind::PLUS,
            .start_offset = 4,
            .end_offset = 5,
            .start_pos = SourcePosition{1, 51},
            .end_pos = SourcePosition{1, 5}},
      Token{.kind = TokenKind::INTLITERAL,
            .start_offset = 6,
            .end_offset = 7,
            .start_pos = SourcePosition{1, 6},
            .end_pos = SourcePosition{1, 6}},
      Token{.kind = TokenKind::EVC_EOF,
            .start_offset = 7,
            .end_offset = 7,
            .start_pos = SourcePosition{2, 1},
            .end_pos = SourcePosition{2, 1}},
  };
  CHECK(v1 == v2);
}