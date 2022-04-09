#pragma once

#include "token.hpp"

#include <cstdint>
#include <vector>

struct Decl;

struct Para {
  uint8_t indirection_counter;
  Token type;
  Token id;
};

struct Stmt;
struct Expr;
struct CmpdStmt;
struct IfStmt;
struct ForStmt;
struct WhileStmt;
struct BreakStmt;
struct ContStmt;
struct RetStmt;

struct CmpdNode {
  enum class kind {
    Decl,
    Stmt,
  };
  kind tag;
  union {
    std::vector<Decl> decl;
    Stmt *stmt;
  };
};

struct CmpdStmt {
  std::vector<CmpdNode> nodes;
};

struct ForStmt {
  Expr *e1;
  Expr *e2;
  Expr *e3;
  Stmt *for_stmt;
};

struct IfStmt {
  Expr *condition;
  Stmt *if_stmt;
  Stmt *else_stmt;
};

struct WhileStmt {
  Expr *condition;
  Stmt *while_stmt;
};

struct RetStmt {
  Expr *ret_expr;
};

struct Stmt {
  enum class kind {
    CmpdStmt,
    IfStmt,
    ForStmt,
    WhileStmt,
    BreakStmt,
    ContStmt,
    RetStmt,
    ExprStmt,
  };
  kind tag;
  union {
    CmpdStmt *compound_node;
    IfStmt *if_node;
    ForStmt *for_node;
    WhileStmt *while_node;
    RetStmt *return_node;
    Expr *expr_node;
    uint8_t nothing;
  };
};

struct AST {
  std::vector<Decl> decls;
};

enum class PrimitiveType {
  Integer,
  Boolean,
  Float,
  Void,
};

struct TypeModifier {
  enum class TypeModKind {
    PointerTo,
    ArrayOf,
    FunctionReturning,
  };
  TypeModKind tag;
  union {
    struct {
      std::vector<Para> para_list;
    };
    struct {
      Expr *array_expr;
    };
  };
};

struct TypeIdent {
  Token ident;
  Token type;
  std::vector<TypeModifier> modifiers;
};

struct UnaryExprNode {
  Token op_tk;
  Expr *expr;
};

struct BinaryExprNode {
  Token op_tk;
  Expr *left_expr;
  Expr *right_expr;
};

struct ExprList {
  std::vector<Expr *> expr_list;
};

struct CallExprNode {
  Expr *left_expr;
  ExprList *exprlist;
};

struct PlainExpr {
  Token the_tk;
};

struct Expr {
  enum class ExprKind {
    UnaryExpr,
    BinaryExpr,
    PlainExpr,
    CallExpr,
  };
  ExprKind tag;
  union {
    UnaryExprNode unary_node;
    BinaryExprNode binary_node;
    PlainExpr plain_node;
    CallExprNode call_node;
  };
};

struct InitValue {
  enum class DeclKind {
    Expr,
    ExprList,
    Body,
    Nothing,
  };
  DeclKind tag;
  union {
    Expr *expr;
    ExprList *exprlist;
    CmpdStmt *body;
    void *nothing;
  };
};

struct Decl {
  TypeIdent ti;
  InitValue init;
};

AST do_parse(Token const *tks, uint32_t length);
