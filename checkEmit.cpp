#include "AlmostJasminIR.hpp"
#include "parser.hpp"
#include "symbol_table.hpp"
#include "token.hpp"
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <memory>

int processDecl(Decl const &dcl, std::vector<AlmostJasminCmd> *ret,
                std::unique_ptr<SymbolMap> *table, char const *data,
                uint32_t length);

int doCheckEmit(AST const &the_ast, std::vector<AlmostJasminCmd> *ret,
                char const *data, uint32_t length) {

  std::unique_ptr<SymbolMap> table;

  for (auto const &dcl : the_ast.decls) {
    processDecl(dcl, ret, &table, data, length);
  }

  return 0;
}

int processDecl(Decl const &dcl, std::vector<AlmostJasminCmd> *ret,
                std::unique_ptr<SymbolMap> *table, char const *data,
                uint32_t length) {

  // 1st: basisc check for reasonable typeness

  // void
  if (dcl.ti.type.kind == TokenKind::VOID) {
    assert(dcl.ti.modifiers.size() == 1 &&
           dcl.ti.modifiers[0].tag ==
               TypeModifier::TypeModKind::FunctionReturning &&
           "Void can only be used as return of function");

  } else if (dcl.ti.modifiers.size() == 0) {

    //

    // No modifiers
  } else if (dcl.ti.modifiers[0].tag == TypeModifier::TypeModKind::PointerTo) {
    assert(dcl.ti.modifiers.size() == 1 && "Can only point to base types");
    // Just a pointer to a base time
  } else if (dcl.ti.modifiers[0].tag == TypeModifier::TypeModKind::ArrayOf) {
    // Array of plain values
    assert((dcl.ti.modifiers.size() == 1) &&
           "Array of base values, no arrays of pointers or functions");
  } else {
    // Function returning
    assert(
        (dcl.ti.modifiers.size() == 1 ||
         (dcl.ti.modifiers.size() == 2 &&
          dcl.ti.modifiers[1].tag == TypeModifier::TypeModKind::PointerTo)) &&
        "Array of base values, no arrays of pointers or functions");
  }

  return 0;
}

struct ComptimeRes {
  enum class Kind {
    Boolean,
    Float,
    Int,
    String,
  };
  Kind tag;
  union {
    bool bool_res;
    float float_res;
    int int_res;
    std::string string_res;
  };
};
