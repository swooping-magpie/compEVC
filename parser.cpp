#include "parser.hpp"
#include "token.hpp"
#include <cassert>
#include <cstddef>
#include <cstdint>

#define CASE_POSTFIX TokenKind::LBRACKET : case TokenKind::LPAREN

#define CASE_PREFIX                                                            \
  TokenKind::MULT : case TokenKind::AMPERSAND : case TokenKind::NOT            \
      : case TokenKind::PLUS : case TokenKind::MINUS

#define CASE_INFIX                                                             \
  TokenKind::PLUS : case TokenKind::MINUS : case TokenKind::DIV                \
      : case TokenKind::MULT : case TokenKind::EQEQ : case TokenKind::ANDAND   \
      : case TokenKind::OROR : case TokenKind::EQ : case TokenKind::LT         \
      : case TokenKind::LTEQ : case TokenKind::GT : case TokenKind::GTEQ       \
      : case TokenKind::NOTEQ

inline uint8_t unary_prefix_binding_power(TokenKind tk) {
  switch (tk) {
  case TokenKind::PLUS:
  case TokenKind::MINUS:
  case TokenKind::NOT:
  case TokenKind::MULT:
  case TokenKind::AMPERSAND:
    return 90;
  default:
    assert(!"Ruh roh not an accepted token");
    return 0;
  }
}

inline uint8_t unary_postfix_binding_power(TokenKind tk) {
  switch (tk) {
  case TokenKind::LPAREN:
  case TokenKind::LBRACKET:
    return 100;
  default:
    assert(!"Ruh roh not an accepted token");
    return 0;
  }
}

inline uint8_t infix_left_binding_power(TokenKind tk) {
  switch (tk) {
  case TokenKind::DIV:
  case TokenKind::MULT:
    return 79;
  case TokenKind::PLUS:
  case TokenKind::MINUS:
    return 69;
  case TokenKind::GT:
  case TokenKind::LT:
    return 59;
  case TokenKind::EQEQ:
  case TokenKind::NOTEQ:
    return 49;
  case TokenKind::ANDAND:
    return 39;
  case TokenKind::OROR:
    return 29;
  case TokenKind::EQ:
    return 21;
  default:
    assert(!"Ruh roh not an accepted token");
    return 0;
  }
}

inline uint8_t infix_right_binding_power(TokenKind tk) {
  switch (tk) {
  case TokenKind::DIV:
  case TokenKind::MULT:
    return 81;
  case TokenKind::PLUS:
  case TokenKind::MINUS:
    return 71;
  case TokenKind::GT:
  case TokenKind::LT:
    return 61;
  case TokenKind::EQEQ:
  case TokenKind::NOTEQ:
    return 51;
  case TokenKind::ANDAND:
    return 41;
  case TokenKind::OROR:
    return 31;
  case TokenKind::EQ:
    return 19;
  default:
    assert(!"Ruh roh not an accepted token");
    return 0;
  }
}

#define CASE_TYPES                                                             \
  TokenKind::BOOLEAN : case TokenKind::INT : case TokenKind::FLOAT             \
      : case TokenKind::VOID

enum class Status {
  TokenNotFound,
  SyntaxError,
  Success,
};

Status parse_decl(Token const *tks, uint32_t length, uint32_t *const offset,
                  std::vector<Decl> *const ret);

struct TokenResult {
  Token tk;
  Status err;
};

static Status munch_token(Token const *tks, uint32_t length,
                          uint32_t *const offset, TokenKind expected,
                          Token *slot) {
  Token ret = tks[*offset];
  if (ret.kind != expected) {
    return Status::TokenNotFound;
  }
  ++*offset;
  *slot = ret;
  return Status::Success;
}

static Status accept_token(Token const *tks, uint32_t length,
                           uint32_t *const offset, TokenKind expected) {
  Token ret = tks[*offset];
  if (ret.kind != expected) {
    return Status::TokenNotFound;
  }
  ++*offset;
  return Status::Success;
}

static Status munch_type(Token const *tks, uint32_t length,
                         uint32_t *const offset, Token *slot) {
  switch (tks[*offset].kind) {
  case CASE_TYPES:
    return munch_token(tks, length, offset, tks[*offset].kind, slot);
  default:
    return Status::TokenNotFound;
  };
}

Status parse_paralist(Token const *tks, uint32_t length, uint32_t *const offset,
                      std::vector<Para> *pl);

Status parseExpr(Token const *tks, uint32_t length, uint32_t *const offset,
                 Expr **expr);

Status parseExprList(Token const *tks, uint32_t length, uint32_t *const offset,
                     TokenKind end_token, ExprList **expr) {

  ExprList *new_list = new ExprList;

  while (tks[*offset].kind != end_token) {
    Expr *new_expr;
    Status err = parseExpr(tks, length, offset, &new_expr);
    if (err != Status::Success) {
      return err;
    }
    new_list->expr_list.push_back(new_expr);
  }

  *expr = new_list;

  return Status::Success;
}

AST do_parse(Token const *tks, uint32_t length) {
  uint32_t offset = 0;
  std::vector<Decl> ret = {};

  while (tks[offset].kind != TokenKind::EVC_EOF) {
    parse_decl(tks, length, &offset, &ret);
  }

  return AST{ret};
}
Status pratt_loop_identifier(Token const *tks, uint32_t length,
                             uint32_t *const offset, TypeIdent *ti);

// Status parseTypeIdent(Token const *tks, uint32_t length, uint32_t *const
// offset,
//                       TypeIdent *ti);

Status parseCompoundStmt(Token const *tks, uint32_t length,
                         uint32_t *const offset, CmpdStmt **cmpst);

Status parseIfStmt(Token const *tks, uint32_t length, uint32_t *const offset,
                   IfStmt **ifst);

Status parseForStmt(Token const *tks, uint32_t length, uint32_t *const offset,
                    ForStmt **forst);

Status parseWhileStmt(Token const *tks, uint32_t length, uint32_t *const offset,
                      WhileStmt **whilest);

Status parseBreakStmt(Token const *tks, uint32_t length,
                      uint32_t *const offset);

Status parseContinueStmt(Token const *tks, uint32_t length,
                         uint32_t *const offset);

Status parseReturnStmt(Token const *tks, uint32_t length,
                       uint32_t *const offset, RetStmt **retst);

Status parseStmt(Token const *tks, uint32_t length, uint32_t *const offset,
                 Stmt **stmt) {
  auto peek_token = tks[*offset];

  Stmt *new_stmt_node = (Stmt *)malloc(sizeof(Stmt));

  switch (peek_token.kind) {
  case TokenKind::LCURLY: {
    // parseCmpdStmt
    new_stmt_node->tag = Stmt::kind::CmpdStmt;
    Status err =
        parseCompoundStmt(tks, length, offset, &new_stmt_node->compound_node);
    if (err != Status::Success) {
      return err;
    }
  }
    return Status::Success;
  case TokenKind::IF: {
    // parseIfStmt
    new_stmt_node->tag = Stmt::kind::IfStmt;
    Status err = parseIfStmt(tks, length, offset, &new_stmt_node->if_node);
    if (err != Status::Success) {
      return err;
    }
  }
    return Status::Success;
    break;
  case TokenKind::FOR: {
    // parseIfStmt
    new_stmt_node->tag = Stmt::kind::ForStmt;
    Status err = parseForStmt(tks, length, offset, &new_stmt_node->for_node);
    if (err != Status::Success) {
      return err;
    }
  }
    return Status::Success;
  case TokenKind::WHILE: {
    new_stmt_node->tag = Stmt::kind::WhileStmt;
    Status err =
        parseWhileStmt(tks, length, offset, &new_stmt_node->while_node);
    if (err != Status::Success) {
      return err;
    }
  }
    return Status::Success;
  case TokenKind::BREAK: {
    new_stmt_node->tag = Stmt::kind::BreakStmt;
    new_stmt_node->nothing = 0;
    Status err = parseBreakStmt(tks, length, offset);
    if (err != Status::Success) {
      return err;
    }
  }
    return Status::Success;
  case TokenKind::CONTINUE: {
    new_stmt_node->tag = Stmt::kind::ContStmt;
    new_stmt_node->nothing = 0;
    Status err = parseContinueStmt(tks, length, offset);
    if (err != Status::Success) {
      return err;
    }
  }
    return Status::Success;
  case TokenKind::RETURN: {
    new_stmt_node->tag = Stmt::kind::RetStmt;
    Status err =
        parseReturnStmt(tks, length, offset, &new_stmt_node->return_node);
    if (err != Status::Success) {
      return err;
    }
  }
    return Status::Success;
  default: {
    new_stmt_node->tag = Stmt::kind::ExprStmt;
    Status err = parseExpr(tks, length, offset, &new_stmt_node->expr_node);
    if (err != Status::Success) {
      return err;
    }
  }
    return Status::Success;
  }
}

Status parseIfStmt(Token const *tks, uint32_t length, uint32_t *const offset,
                   IfStmt **ifst) {

  Status err = accept_token(tks, length, offset, TokenKind::IF);
  if (err != Status::Success) {
    return err;
  }

  err = accept_token(tks, length, offset, TokenKind::LPAREN);
  if (err != Status::Success) {
    return err;
  }

  IfStmt *new_node = (IfStmt *)malloc(sizeof(IfStmt));

  err = parseExpr(tks, length, offset, &new_node->condition);
  if (err != Status::Success) {
    return err;
  }

  err = accept_token(tks, length, offset, TokenKind::RPAREN);
  if (err != Status::Success) {
    return err;
  }

  err = parseStmt(tks, length, offset, &new_node->if_stmt);
  if (err != Status::Success) {
    return err;
  }

  auto peek_token = tks[*offset];
  if (peek_token.kind != TokenKind::ELSE) {
    new_node->else_stmt = nullptr;

    *ifst = new_node;

    return Status::Success;
  } else {
    accept_token(tks, length, offset, TokenKind::ELSE);
    err = parseStmt(tks, length, offset, &new_node->else_stmt);
    if (err != Status::Success) {
      return err;
    }

    *ifst = new_node;

    return Status::Success;
  }
}

Status parseForStmt(Token const *tks, uint32_t length, uint32_t *const offset,
                    ForStmt **forst) {

  Status err = accept_token(tks, length, offset, TokenKind::FOR);
  if (err != Status::Success) {
    return err;
  }

  err = accept_token(tks, length, offset, TokenKind::LPAREN);
  if (err != Status::Success) {
    return err;
  }

  ForStmt *new_node = (ForStmt *)malloc(sizeof(ForStmt));

  auto peek_token = tks[*offset];
  if (peek_token.kind == TokenKind::SEMICOLON) {
    accept_token(tks, length, offset, TokenKind::SEMICOLON);
  } else {
    err = parseExpr(tks, length, offset, &new_node->e1);
    if (err != Status::Success) {
      return err;
    }
  }

  peek_token = tks[*offset];
  if (peek_token.kind == TokenKind::SEMICOLON) {
    accept_token(tks, length, offset, TokenKind::SEMICOLON);
  } else {
    err = parseExpr(tks, length, offset, &new_node->e2);
    if (err != Status::Success) {
      return err;
    }
  }

  peek_token = tks[*offset];
  if (peek_token.kind == TokenKind::RPAREN) {
    ;
  } else {
    err = parseExpr(tks, length, offset, &new_node->e3);
    if (err != Status::Success) {
      return err;
    }
  }

  err = accept_token(tks, length, offset, TokenKind::RPAREN);
  if (err != Status::Success) {
    return err;
  }

  err = parseStmt(tks, length, offset, &new_node->for_stmt);
  if (err != Status::Success) {
    return err;
  }

  *forst = new_node;
  return Status::Success;
}

Status parseWhileStmt(Token const *tks, uint32_t length, uint32_t *const offset,
                      WhileStmt **whilest) {

  Status err = accept_token(tks, length, offset, TokenKind::WHILE);
  if (err != Status::Success) {
    return err;
  }

  err = accept_token(tks, length, offset, TokenKind::LPAREN);
  if (err != Status::Success) {
    return err;
  }

  WhileStmt *new_node = (WhileStmt *)malloc(sizeof(WhileStmt));
  err = parseExpr(tks, length, offset, &new_node->condition);
  if (err != Status::Success) {
    return err;
  }

  err = accept_token(tks, length, offset, TokenKind::RPAREN);
  if (err != Status::Success) {
    return err;
  }

  err = parseStmt(tks, length, offset, &new_node->while_stmt);
  if (err != Status::Success) {
    return err;
  }

  *whilest = new_node;
  return Status::Success;
};

Status parseBreakStmt(Token const *tks, uint32_t length,
                      uint32_t *const offset) {
  Status err = accept_token(tks, length, offset, TokenKind::CONTINUE);
  if (err != Status::Success) {
    return err;
  }
  return accept_token(tks, length, offset, TokenKind::SEMICOLON);
}

Status parseContinueStmt(Token const *tks, uint32_t length,
                         uint32_t *const offset) {
  Status err = accept_token(tks, length, offset, TokenKind::CONTINUE);
  if (err != Status::Success) {
    return err;
  }
  return accept_token(tks, length, offset, TokenKind::SEMICOLON);
}

Status parseReturnStmt(Token const *tks, uint32_t length,
                       uint32_t *const offset, RetStmt **retst) {
  Status err = accept_token(tks, length, offset, TokenKind::RETURN);
  if (err != Status::Success) {
    return err;
  }

  auto peek_token = tks[*offset];

  if (peek_token.kind == TokenKind::SEMICOLON) {
    accept_token(tks, length, offset, TokenKind::SEMICOLON);

    *retst = nullptr;
    return Status::Success;
  } else {
    RetStmt *new_node = (RetStmt *)malloc(sizeof(RetStmt));
    Status err = parseExpr(tks, length, offset, &new_node->ret_expr);
    if (err != Status::Success) {
      return err;
    }
    err = accept_token(tks, length, offset, TokenKind::SEMICOLON);
    if (err != Status::Success) {
      return err;
    }

    *retst = new_node;
    return Status::Success;
  }
}

Status parseCompoundStmt(Token const *tks, uint32_t length,
                         uint32_t *const offset, CmpdStmt **cmpst) {
  Status err = accept_token(tks, length, offset, TokenKind::LCURLY);
  if (err != Status::Success) {
    return err;
  }

  *cmpst = new CmpdStmt;

  for (auto peek_token = tks[*offset]; peek_token.kind != TokenKind::RCURLY;
       ++*offset, peek_token = tks[*offset]) {
    if (peek_token.kind == TokenKind::BOOLEAN ||
        peek_token.kind == TokenKind::INT ||
        peek_token.kind == TokenKind::FLOAT ||
        peek_token.kind == TokenKind::VOID) {
      std::vector<Decl> dcl;
      err = parse_decl(tks, length, offset, &dcl);
      if (err != Status::Success) {
        return err;
      }
      CmpdNode cnode = {.tag = CmpdNode::kind::Decl, .decl = dcl};
      (*cmpst)->nodes.push_back(cnode);
    } else {
      Stmt *stmt_node;
      err = parseStmt(tks, length, offset, &stmt_node);
      if (err != Status::Success) {
        return err;
      }
      CmpdNode cnode = {.tag = CmpdNode::kind::Stmt, .stmt = stmt_node};
      (*cmpst)->nodes.push_back(cnode);
    }
  }

  err = accept_token(tks, length, offset, TokenKind::RCURLY);
  if (err != Status::Success) {
    return err;
  }
  return Status::Success;
};

Status parse_decl(Token const *tks, uint32_t length, uint32_t *const offset,
                  std::vector<Decl> *const ret) {
  Token type_tk;
  Status err = munch_type(tks, length, offset, &type_tk);
  if (err != Status::Success) {
    return err;
  }
  return Status::Success;

  while (1) {
    Decl dcl;
    dcl.ti.type = type_tk;
    Status res = pratt_loop_identifier(tks, length, offset, &dcl.ti);
    if (res != Status::Success) {
      return res;
    }

    Token peek_token = tks[*offset];
    switch (peek_token.kind) {
    case TokenKind::COMMA: {
      dcl.init.tag = InitValue::DeclKind::Nothing;
      dcl.init.nothing = nullptr;
      ret->emplace_back(dcl);
      accept_token(tks, length, offset, TokenKind::COMMA);
    }
      continue;
    case TokenKind::SEMICOLON: {
      dcl.init.tag = InitValue::DeclKind::Nothing;
      dcl.init.nothing = nullptr;
      ret->emplace_back(dcl);
      accept_token(tks, length, offset, TokenKind::SEMICOLON);
    }
      return Status::Success;
    case TokenKind::LCURLY: {
      dcl.init.tag = InitValue::DeclKind::Body;
      err = parseCompoundStmt(tks, length, offset, &dcl.init.body);
      ret->emplace_back(dcl);
    }
      return Status::Success;
    case TokenKind::EQ: {
      accept_token(tks, length, offset, TokenKind::EQ);
      if (tks[*offset].kind == TokenKind::LCURLY) {
        // can be an expression list!
        dcl.init.tag = InitValue::DeclKind::ExprList;
        Status err = accept_token(tks, length, offset, TokenKind::LCURLY);
        if (err != Status::Success) {
          return err;
        }

        err = parseExprList(tks, length, offset, TokenKind::RCURLY,
                            &dcl.init.exprlist);

        err = accept_token(tks, length, offset, TokenKind::RCURLY);
        if (err != Status::Success) {
          return err;
        }
        ret->emplace_back(dcl);
      } else {
        // just a plain expression!
        dcl.init.tag = InitValue::DeclKind::Expr;
        err = parseExpr(tks, length, offset, &dcl.init.expr);
        ret->emplace_back(dcl);
      }
      if (tks[*offset].kind == TokenKind::SEMICOLON) {
        accept_token(tks, length, offset, TokenKind::SEMICOLON);
        return Status::Success;
      } else if (tks[*offset].kind == TokenKind::COMMA) {
        accept_token(tks, length, offset, TokenKind::COMMA);
        continue;
      } else {
        return Status::SyntaxError;
      }
    }
    default:
      return Status::TokenNotFound;
    }
  }
}

Status pratt_loop_identifier(Token const *tks, uint32_t length,
                             uint32_t *const offset, TypeIdent *ti) {

  auto curr_token = tks[*offset];

  switch (curr_token.kind) {
  case TokenKind::LPAREN: {
    pratt_loop_identifier(tks, length, offset, ti);
    if (tks[*offset].kind != TokenKind::RPAREN) {
      return Status::TokenNotFound;
    }
    ++*offset;
  } break;
  case TokenKind::MULT: {
    pratt_loop_identifier(tks, length, offset, ti);
  } break;
  case TokenKind::ID: {
    ti->ident = curr_token;
  } break;
  default:
    return Status::TokenNotFound;
  }

  while (1) {
    if (*offset >= length) {
      goto END_CON;
    }
    auto peek_token = tks[*offset];
    switch (peek_token.kind) {
    case TokenKind::LPAREN: {
      ++*offset;
      std::vector<Para> paralist = {};
      Status ret = parse_paralist(tks, length, offset, &paralist);
      if (ret != Status::Success) {
        return ret;
      }

      if (tks[*offset].kind != TokenKind::RPAREN) {
        return Status::TokenNotFound;
      }
      ++*offset;

      ti->modifiers.push_back(
          TypeModifier{.tag = TypeModifier::TypeModKind::FunctionReturning,
                       .para_list = std::move(paralist)});
    }
      continue;
    case TokenKind::LBRACKET: {
      ++*offset;
      Expr *expr;
      Status ret = parseExpr(tks, length, offset, &expr);
      if (ret != Status::Success) {
        return ret;
      }

      if (tks[*offset].kind != TokenKind::RBRACKET) {
        return Status::TokenNotFound;
      }
      ++*offset;

      ti->modifiers.push_back(TypeModifier{
          .tag = TypeModifier::TypeModKind::ArrayOf, .array_expr = expr});
    }
      continue;
    default:
      return Status::TokenNotFound;
    }
    break;
  }
END_CON:

  if (curr_token.kind == TokenKind::MULT) {
    TypeModifier tm = {.tag = TypeModifier::TypeModKind::PointerTo,
                       .array_expr = nullptr};
    ti->modifiers.push_back(tm);
  }

  return Status::Success;
}

Status pratt_loop_expr(Token const *tks, uint32_t length,
                       uint32_t *const offset, uint8_t bp_level, Expr **expr) {

  auto curr_token = tks[*offset];
  ++*offset;
  uint8_t right_bp;
  Status err;
  Expr *new_left_node;

  switch (curr_token.kind) {
  case TokenKind::LPAREN: {
    err = pratt_loop_expr(tks, length, offset, 0, &new_left_node);
    if (err != Status::Success) {
      return err;
    }
    if (tks[*offset].kind != TokenKind::RPAREN) {
      return Status::TokenNotFound;
    }
    ++*offset;
  } break;
  case CASE_PREFIX: {
    right_bp = unary_prefix_binding_power(curr_token.kind);
    new_left_node = (Expr *)malloc(sizeof(Expr));
    new_left_node->tag = Expr::ExprKind::UnaryExpr;
    new_left_node->unary_node.op_tk = curr_token;
    err = pratt_loop_expr(tks, length, offset, right_bp,
                          &new_left_node->unary_node.expr);
    if (err != Status::Success) {
      return err;
    }
  } break;
  case TokenKind::ID: {
    new_left_node = (Expr *)malloc(sizeof(Expr));
    new_left_node->tag = Expr::ExprKind::PlainExpr;
    new_left_node->plain_node.the_tk = curr_token;
  } break;
  default:
    return Status::TokenNotFound;
  }

  Token curr_op;

  while (1) {
    if (*offset >= length) {
      if (*offset > length) {
        return Status::SyntaxError;
      }
      goto END_CON;
    }
    auto peek_token = tks[*offset];
    switch (peek_token.kind) {
    case CASE_POSTFIX: {
      uint8_t left_bp = unary_postfix_binding_power(peek_token.kind);
      if (left_bp < bp_level) {
        goto END_CON;
      }
      ++*offset;
      if (peek_token.kind == TokenKind::LBRACKET) {
        Expr *temp_node = (Expr *)malloc(sizeof(Expr));
        temp_node->tag = Expr::ExprKind::BinaryExpr;
        temp_node->binary_node.op_tk = peek_token;
        temp_node->binary_node.left_expr = new_left_node;
        err = pratt_loop_expr(tks, length, offset, 0,
                              &temp_node->binary_node.right_expr);
        if (err != Status::Success) {
          return err;
        }
        new_left_node = temp_node;
      } else {
        assert(peek_token.kind == TokenKind::LPAREN);
        Expr *temp_node = (Expr *)malloc(sizeof(Expr));
        temp_node->tag = Expr::ExprKind::CallExpr;
        temp_node->call_node.left_expr = new_left_node;
        err = parseExprList(tks, length, offset, TokenKind::RPAREN,
                            &temp_node->call_node.exprlist);
        if (err != Status::Success) {
          return err;
        }
        new_left_node = temp_node;
      }
    }
      continue;
    case CASE_INFIX: {
      uint8_t left_bp = infix_left_binding_power(peek_token.kind);
      if (left_bp < bp_level) {
        goto END_CON;
      }
      ++*offset;
      Expr *temp_node = (Expr *)malloc(sizeof(Expr));
      temp_node->tag = Expr::ExprKind::BinaryExpr;
      temp_node->binary_node.op_tk = peek_token;
      temp_node->binary_node.left_expr = new_left_node;
      uint8_t right_bp = infix_right_binding_power(peek_token.kind);
      err = pratt_loop_expr(tks, length, offset, right_bp,
                            &temp_node->binary_node.right_expr);
      if (err != Status::Success) {
        return err;
      }
      new_left_node = temp_node;
    }
      continue;
    default:
      return Status::TokenNotFound;
    }
    break;
  }
END_CON:
  *expr = new_left_node;
  return Status::Success;
}

Status parseExpr(Token const *tks, uint32_t length, uint32_t *const offset,
                 Expr **expr) {
  Status err = pratt_loop_expr(tks, length, offset, 0, expr);
  if (err != Status::Success) {
    return err;
  }
  return Status::Success;
};

Status parse_paralist(Token const *tks, uint32_t length, uint32_t *const offset,
                      std::vector<Para> *pl) {}
