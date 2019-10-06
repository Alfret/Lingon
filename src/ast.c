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

#define LN_AST_KIND_CHECK(ast, _kind)                                          \
  do {                                                                         \
    assrt(ast->kind == _kind, &LN_CSTR("Wrong ast kind"));                     \
  } while (0)

// ========================================================================== //
// AstList
// ========================================================================== //

AstList
make_ast_list(u32 cap)
{
  u32 _cap = cap ? cap : 10;
  Ast* buf = alloc(sizeof(Ast) * _cap, kLnMinAlign);
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
ast_list_append(AstList* list, const Ast* ast)
{
  if (list->len >= list->cap) {
    ast_list_reserve(list, list->cap * 2);
  }
  memcpy(list->buf + list->len++, ast, sizeof(Ast));
}

// -------------------------------------------------------------------------- //

Ast
ast_list_remove(AstList* list, u32 index)
{
  assrt(index < list->len, &LN_CSTR("Index out of bounds (%u)"), index);
  Ast ast = list->buf[index];
  memmove(list->buf + index,
          list->buf + index + 1,
          sizeof(Ast) * (list->len - index - 1));
  return ast;
}

// -------------------------------------------------------------------------- //

const Ast*
ast_list_get(const AstList* list, u32 index)
{
  assrt(index < list->len, &LN_CSTR("Index out of bounds (%u)"), index);
  return &list->buf[index];
}

// -------------------------------------------------------------------------- //

void
ast_list_reserve(AstList* list, u32 cap)
{
  if (list->cap > cap) {
    return;
  }
  Ast* buf = alloc(sizeof(Ast) * cap, kLnMinAlign);
  memcpy(buf, list->buf, sizeof(Ast) * list->len);
  release(list->buf);
  list->buf = buf;
  list->cap = cap;
}

// ========================================================================== //
// AstProg
// ========================================================================== //

Ast
make_ast_prog()
{
  AstProg ast_prog = (AstProg){ .funs = make_ast_list(8) };
  return (Ast){ .kind = kAstProg, .prog = ast_prog };
}

// -------------------------------------------------------------------------- //

void
release_ast_prog(Ast* ast)
{
  LN_AST_KIND_CHECK(ast, kAstProg);
  release_ast_list(&ast->prog.funs);
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

Ast
make_ast_fn(StrSlice name)
{
  AstFn ast_fn = (AstFn){ .name = name };
  return (Ast){ .kind = kAstFn, .fn = ast_fn };
}

// -------------------------------------------------------------------------- //

void
release_ast_fn(Ast* ast)
{
  LN_AST_KIND_CHECK(ast, kAstFn);
}

// ========================================================================== //
// Ast
// ========================================================================== //

Ast
make_ast_invalid()
{
  return (Ast){ .kind = kAstInvalid };
}

// -------------------------------------------------------------------------- //

void
release_ast(Ast* ast)
{
  switch (ast->kind) {
    case kAstProg: {
      release_ast_prog(ast);
    }
    case kAstFn: {
      release_ast_fn(ast);
    }
    default: {
      panic(&LN_CSTR("Invalid token kind"));
    }
  }
  LN_UNUSED(ast);
}