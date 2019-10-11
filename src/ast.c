// MIT License
//
// Copyright (c) 2019 Filip Björklund
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <memory.h>

#include "ast.h"

// ========================================================================== //
// Util
// ========================================================================== //

#define LN_AST_KIND_CHECK_PRED(predicate)                                      \
  do {                                                                         \
    assrt(predicate, make_str("Wrong ast kind"));                              \
  } while (0)

// -------------------------------------------------------------------------- //

#define LN_AST_KIND_CHECK(ast, _kind) LN_AST_KIND_CHECK_PRED(ast->kind == _kind)

// ========================================================================== //
// AstList
// ========================================================================== //

AstList
make_ast_list(u32 cap)
{
  u32 _cap = cap ? cap : 10;
  Ast** buf = alloc(sizeof(Ast*) * _cap, kLnMinAlign);
  if (!buf) {
    return (AstList){ .buf = 0, .len = 0, .cap = 0 };
  }
  return (AstList){ .buf = buf, .len = 0, .cap = _cap };
}

// -------------------------------------------------------------------------- //

void
release_ast_list(AstList* list)
{
  release(list->buf);
}

// -------------------------------------------------------------------------- //

void
ast_list_append(AstList* list, Ast* ast)
{
  if (list->len >= list->cap) {
    ast_list_reserve(list, list->cap * 2);
  }
  list->buf[list->len++] = ast;
}

// -------------------------------------------------------------------------- //

Ast*
ast_list_remove(AstList* list, u32 index)
{
  assrt(index < list->len, make_str("Index out of bounds (%u)"), index);
  Ast* ast = list->buf[index];
  memmove(list->buf + index,
          list->buf + index + 1,
          sizeof(Ast*) * (list->len - index - 1));
  return ast;
}

// -------------------------------------------------------------------------- //

Ast*
ast_list_get(AstList* list, u32 index)
{
  assrt(index < list->len, make_str("Index out of bounds (%u)"), index);
  return list->buf[index];
}

// -------------------------------------------------------------------------- //

void
ast_list_reserve(AstList* list, u32 cap)
{
  if (list->cap > cap) {
    return;
  }
  Ast** buf = alloc(sizeof(Ast*) * cap, kLnMinAlign);
  memcpy(buf, list->buf, sizeof(Ast*) * list->len);
  release(list->buf);
  list->buf = buf;
  list->cap = cap;
}

// ========================================================================== //
// AstProg
// ========================================================================== //

Ast*
make_ast_prog()
{
  Ast* ast = make_ast_invalid();
  ast->kind = kAstProg;
  ast->prog = (AstProg){ .funs = make_ast_list(8) };
  return ast;
}

// -------------------------------------------------------------------------- //

void
release_ast_prog(Ast* ast)
{
  LN_AST_KIND_CHECK(ast, kAstProg);
  release_ast_list(&ast->prog.funs);
  release(ast);
}

// -------------------------------------------------------------------------- //

void
ast_prog_add_fn(Ast* ast_prog, Ast* ast_fn)
{
  LN_AST_KIND_CHECK(ast_prog, kAstProg);
  LN_AST_KIND_CHECK(ast_fn, kAstFn);
  ast_list_append(&ast_prog->prog.funs, ast_fn);
}

// ========================================================================== //
// AstFn
// ========================================================================== //

Ast*
make_ast_fn(StrSlice name)
{
  Ast* ast = make_ast_invalid();
  ast->kind = kAstFn;
  ast->fn = (AstFn){ .name = name, .params = make_ast_list(2), .ret = NULL };
  return ast;
}

// -------------------------------------------------------------------------- //

void
release_ast_fn(Ast* ast)
{
  LN_AST_KIND_CHECK(ast, kAstFn);
  release(ast);
}

// -------------------------------------------------------------------------- //

void
ast_fn_add_param(Ast* ast_fn, Ast* ast_param)
{
  LN_AST_KIND_CHECK(ast_fn, kAstFn);
  LN_AST_KIND_CHECK(ast_param, kAstParam);
  ast_list_append(&ast_fn->fn.params, ast_param);
}

// -------------------------------------------------------------------------- //

void
ast_fn_set_ret(Ast* ast_fn, Ast* ast_ret)
{
  LN_AST_KIND_CHECK(ast_fn, kAstFn);
  LN_AST_KIND_CHECK(ast_ret, kAstType);
  ast_fn->fn.ret = ast_ret;
}

// ========================================================================== //
// AstParam
// ========================================================================== //

Ast*
make_ast_param()
{
  Ast* ast = make_ast_invalid();
  ast->kind = kAstParam;
  ast->param = (AstParam){ .name = make_str_slice_null(), .type = NULL };
  return ast;
}

// -------------------------------------------------------------------------- //

void
release_ast_param(Ast* ast)
{
  LN_AST_KIND_CHECK(ast, kAstParam);
  release(ast);
}

// ========================================================================== //
// AstBlock
// ========================================================================== //

Ast*
make_ast_block()
{
  Ast* ast = make_ast_invalid();
  ast->kind = kAstBlock;
  ast->block = (AstBlock){ .stmts = make_ast_list(10), .ret_expr = NULL };
  return ast;
}

// -------------------------------------------------------------------------- //

void
release_ast_block(Ast* ast)
{
  LN_AST_KIND_CHECK(ast, kAstBlock);
  release_ast_list(&ast->block.stmts);
  release(ast);
}

// -------------------------------------------------------------------------- //

void
ast_block_add_stmt(Ast* ast_block, Ast* ast_stmt)
{
  LN_AST_KIND_CHECK(ast_block, kAstBlock);
  LN_AST_KIND_CHECK_PRED(ast_is_stmt(ast_stmt));
  ast_list_append(&ast_block->block.stmts, ast_stmt);
}

// ========================================================================== //
// AstLet
// ========================================================================== //

Ast*
make_ast_let(const StrSlice* name)
{
  Ast* ast = make_ast_invalid();
  ast->kind = kAstLet;
  ast->let = (AstLet){ .name = *name, .expr = NULL };
  return ast;
}

// -------------------------------------------------------------------------- //

void
release_ast_let(Ast* ast_let)
{
  LN_AST_KIND_CHECK(ast_let, kAstLet);
  release_ast(ast_let->let.expr);
  release(ast_let);
}

// -------------------------------------------------------------------------- //

void
ast_let_set_assigned(Ast* ast_let, Ast* ast_expr)
{
  LN_AST_KIND_CHECK_PRED(ast_is_expr(ast_expr));
  ast_let->let.expr = ast_expr;
}

// ========================================================================== //
// AstRet
// ========================================================================== //

Ast*
make_ast_ret(Ast* ast_expr)
{
  LN_AST_KIND_CHECK_PRED(ast_is_expr(ast_expr));
  Ast* ast = make_ast_invalid();
  ast->kind = kAstRet;
  ast->ret = (AstRet){ .expr = ast_expr };
  return ast;
}

// -------------------------------------------------------------------------- //

void
release_ast_ret(Ast* ast_ret)
{
  LN_AST_KIND_CHECK(ast_ret, kAstRet);
  release_ast(ast_ret->ret.expr);
  release(ast_ret);
}

// ========================================================================== //
// AstType
// ========================================================================== //

Ast*
make_ast_type(Type* type)
{
  Ast* ast = make_ast_invalid();
  ast->kind = kAstType;
  ast->type = (AstType){ .type = type };
  return ast;
}

// -------------------------------------------------------------------------- //

void
release_ast_type(Ast* ast)
{
  LN_AST_KIND_CHECK(ast, kAstType);
  release(ast);
}

// ========================================================================== //
// Ast
// ========================================================================== //

Ast*
make_ast_invalid()
{
  Ast* ast = alloc(sizeof(Ast), kLnMinAlign);
  ast->kind = kAstInvalid;
  return ast;
}

// -------------------------------------------------------------------------- //

void
release_ast(Ast* ast)
{
  switch (ast->kind) {
    case kAstInvalid: {
      panic(make_str("Cannot release invalid ast"));
    }
    case kAstProg: {
      release_ast_prog(ast);
    }
    case kAstFn: {
      release_ast_fn(ast);
    }
    case kAstParam: {
      release_ast_param(ast);
    }
    case kAstBlock: {
      release_ast_block(ast);
    }
    case kAstLet: {
      release_ast_let(ast);
    }
    case kAstRet: {
      release_ast_ret(ast);
    }
    case kAstType: {
      release_ast_type(ast);
    }
    default: {
      panic(make_str("Invalid token kind"));
    }
  }
}

// -------------------------------------------------------------------------- //

bool
ast_is_stmt(Ast* ast)
{
  return ast->kind == kAstLet || ast->kind == kAstRet;
}

// -------------------------------------------------------------------------- //

/* Check if ast is expr */
bool
ast_is_expr(Ast* ast)
{
  LN_UNUSED(ast);
  return false;
}