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
#include <stdio.h>
#include <stdlib.h>

#include "ast.h"

#define kAstIndentStep 1

// ========================================================================== //
// Util
// ========================================================================== //

#define LN_AST_KIND_CHECK(predicate)                                           \
  do {                                                                         \
    assrt(predicate, make_str("Wrong ast kind"));                              \
  } while (0)

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
  for (u32 i = 0; i < list->len; i++) {
    Ast* ast_i = ast_list_get(list, i);
    release_ast(ast_i);
  }
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
  LN_AST_KIND_CHECK(ast->kind == kAstProg);
  release_ast_list(&ast->prog.funs);
  release(ast);
}

// -------------------------------------------------------------------------- //

void
ast_prog_add_fn(Ast* ast_prog, Ast* ast_fn)
{
  LN_AST_KIND_CHECK(ast_prog->kind == kAstProg);
  LN_AST_KIND_CHECK(ast_fn->kind == kAstFn);
  ast_list_append(&ast_prog->prog.funs, ast_fn);
}

// -------------------------------------------------------------------------- //

void
ast_prog_dump(Ast* ast, u32 indent)
{
  LN_AST_KIND_CHECK(ast->kind == kAstProg);
  printf("%*sprogram:\n", indent, "");
  for (u32 i = 0; i < ast->prog.funs.len; i++) {
    Ast* ast_fn = ast_list_get(&ast->prog.funs, i);
    ast_dump_aux(ast_fn, indent + kAstIndentStep);
  }
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
  LN_AST_KIND_CHECK(ast->kind == kAstFn);
  release_ast_list(&ast->fn.params);
  release_ast(ast->fn.ret);
  release_ast(ast->fn.body);
  release(ast);
}

// -------------------------------------------------------------------------- //

void
ast_fn_add_param(Ast* ast_fn, Ast* ast_param)
{
  LN_AST_KIND_CHECK(ast_fn->kind == kAstFn);
  LN_AST_KIND_CHECK(ast_param->kind == kAstParam);
  ast_list_append(&ast_fn->fn.params, ast_param);
}

// -------------------------------------------------------------------------- //

void
ast_fn_set_ret(Ast* ast_fn, Ast* ast_ret)
{
  LN_AST_KIND_CHECK(ast_fn->kind == kAstFn);
  LN_AST_KIND_CHECK(ast_ret->kind == kAstType);
  ast_fn->fn.ret = ast_ret;
}

// -------------------------------------------------------------------------- //

void
ast_fn_dump(Ast* ast, u32 indent)
{
  LN_AST_KIND_CHECK(ast->kind == kAstFn);
  printf("%*sfun '%.*s':\n", indent, "", str_slice_print(&ast->fn.name));
  for (u32 i = 0; i < ast->fn.params.len; i++) {
    Ast* ast_i = ast_list_get(&ast->fn.params, i);
    ast_dump_aux(ast_i, indent + kAstIndentStep);
  }
  printf("%*sret:\n", indent + kAstIndentStep, "");
  ast_dump_aux(ast->fn.ret, indent + (2 * kAstIndentStep));
  printf("%*sbody:\n", indent + kAstIndentStep, "");
  ast_dump_aux(ast->fn.body, indent + (2 * kAstIndentStep));
}

// ========================================================================== //
// AstParam
// ========================================================================== //

Ast*
make_ast_param()
{
  Ast* ast = make_ast_invalid();
  ast->kind = kAstParam;
  ast->param = (AstParam){ .name = str_slice_null(), .type = NULL };
  return ast;
}

// -------------------------------------------------------------------------- //

void
release_ast_param(Ast* ast)
{
  LN_AST_KIND_CHECK(ast->kind == kAstParam);
  release(ast);
}

// -------------------------------------------------------------------------- //

void
ast_param_dump(Ast* ast, u32 indent)
{
  LN_AST_KIND_CHECK(ast->kind == kAstParam);
  printf("%*sparam (%.*s):\n", indent, "", str_slice_print(&ast->param.name));
  ast_dump_aux(ast->param.type, indent + kAstIndentStep);
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
  LN_AST_KIND_CHECK(ast->kind == kAstBlock);
  release_ast_list(&ast->block.stmts);
  release(ast);
}

// -------------------------------------------------------------------------- //

void
ast_block_add_stmt(Ast* ast_block, Ast* ast_stmt)
{
  LN_AST_KIND_CHECK(ast_block->kind == kAstBlock);
  LN_AST_KIND_CHECK(ast_is_stmt(ast_stmt));
  ast_list_append(&ast_block->block.stmts, ast_stmt);
}

// -------------------------------------------------------------------------- //

void
ast_block_dump(Ast* ast, u32 indent)
{
  LN_AST_KIND_CHECK(ast->kind == kAstBlock);
  printf("%*sblock:\n", indent, "");
  for (u32 i = 0; i < ast->block.stmts.len; i++) {
    Ast* ast_i = ast_list_get(&ast->block.stmts, i);
    ast_dump_aux(ast_i, indent + kAstIndentStep);
  }
}

// ========================================================================== //
// AstLet
// ========================================================================== //

Ast*
make_ast_let()
{
  Ast* ast = make_ast_invalid();
  ast->kind = kAstLet;
  ast->let = (AstLet){ .name = str_slice_null(), .expr = NULL };
  return ast;
}

// -------------------------------------------------------------------------- //

void
release_ast_let(Ast* ast_let)
{
  LN_AST_KIND_CHECK(ast_let->kind == kAstLet);
  release_ast(ast_let->let.expr);
  release(ast_let);
}

// -------------------------------------------------------------------------- //

void
ast_let_set_name(Ast* ast_let, StrSlice name)
{
  LN_AST_KIND_CHECK(ast_let->kind == kAstLet);
  ast_let->let.name = name;
}

// -------------------------------------------------------------------------- //

void
ast_let_set_type(Ast* ast_let, Ast* ast_type)
{
  LN_AST_KIND_CHECK(ast_let->kind == kAstLet);
  LN_AST_KIND_CHECK(ast_type->kind == kAstType);
  ast_let->let.type = ast_type;
}

// -------------------------------------------------------------------------- //

void
ast_let_set_assigned(Ast* ast_let, Ast* ast_expr)
{
  LN_AST_KIND_CHECK(ast_let->kind == kAstLet);
  LN_AST_KIND_CHECK(ast_is_expr(ast_expr));
  ast_let->let.expr = ast_expr;
}

// -------------------------------------------------------------------------- //

void
ast_let_dump(Ast* ast, u32 indent)
{
  LN_UNUSED(ast);
  LN_UNUSED(indent);
}

// ========================================================================== //
// AstRet
// ========================================================================== //

Ast*
make_ast_ret(Ast* ast_expr)
{
  LN_AST_KIND_CHECK(ast_is_expr(ast_expr));
  Ast* ast = make_ast_invalid();
  ast->kind = kAstRet;
  ast->ret = (AstRet){ .expr = ast_expr };
  return ast;
}

// -------------------------------------------------------------------------- //

void
release_ast_ret(Ast* ast_ret)
{
  LN_AST_KIND_CHECK(ast_ret->kind == kAstRet);
  release_ast(ast_ret->ret.expr);
  release(ast_ret);
}

// -------------------------------------------------------------------------- //

void
ast_ret_dump(Ast* ast, u32 indent)
{
  LN_AST_KIND_CHECK(ast->kind == kAstRet);
  printf("%*sret:\n", indent, "");
  ast_dump_aux(ast->ret.expr, indent + kAstIndentStep);
}

// ========================================================================== //
// AstBinop
// ========================================================================== //

Ast*
make_ast_binop(AstBinopKind kind)
{
  Ast* ast = make_ast_invalid();
  ast->kind = kAstBinop;
  ast->binop = (AstBinop){ .kind = kind, .lhs = NULL, .rhs = NULL };
  return ast;
}

// -------------------------------------------------------------------------- //

void
release_ast_binop(Ast* ast_binop)
{
  LN_AST_KIND_CHECK(ast_binop->kind == kAstBinop);
  release_ast(ast_binop->binop.lhs);
  release_ast(ast_binop->binop.rhs);
  release(ast_binop);
}

// -------------------------------------------------------------------------- //

void
ast_binop_set_kind(Ast* ast_binop, AstBinopKind kind)
{
  LN_AST_KIND_CHECK(ast_binop->kind == kAstBinop);
  ast_binop->binop.kind = kind;
}

// -------------------------------------------------------------------------- //

void
ast_binop_set_lhs(Ast* ast_binop, Ast* ast_lhs)
{
  LN_AST_KIND_CHECK(ast_binop->kind == kAstBinop);
  LN_AST_KIND_CHECK(ast_is_expr(ast_lhs));
  ast_binop->binop.lhs = ast_lhs;
}

// -------------------------------------------------------------------------- //

void
ast_binop_set_rhs(Ast* ast_binop, Ast* ast_rhs)
{
  LN_AST_KIND_CHECK(ast_binop->kind == kAstBinop);
  LN_AST_KIND_CHECK(ast_is_expr(ast_rhs));
  ast_binop->binop.rhs = ast_rhs;
}

// -------------------------------------------------------------------------- //

void
ast_binop_dump(Ast* ast, u32 indent)
{
  LN_AST_KIND_CHECK(ast->kind == kAstBinop);

  // Op str
  char* op_str = "";
  if (ast->binop.kind == kAstBinopAdd) {
    op_str = "+";
  } else if (ast->binop.kind == kAstBinopSub) {
    op_str = "-";
  } else if (ast->binop.kind == kAstBinopMul) {
    op_str = "*";
  } else if (ast->binop.kind == kAstBinopDiv) {
    op_str = "/";
  } else if (ast->binop.kind == kAstBinopMod) {
    op_str = "%";
  }

  // Dump binop
  printf("%*sbinop '%s':\n", indent, "", op_str);
  printf("%*slhs:\n", indent + kAstIndentStep, "");
  ast_dump_aux(ast->binop.lhs, indent + (2 * kAstIndentStep));
  printf("%*srhs:\n", indent + kAstIndentStep, "");
  ast_dump_aux(ast->binop.rhs, indent + (2 * kAstIndentStep));
}

// ========================================================================== //
// AstConst
// ========================================================================== //

Ast*
make_ast_const(AstConstKind kind, StrSlice value)
{
  Ast* ast = make_ast_invalid();
  ast->kind = kAstConst;
  ast->constant = (AstConst){ .kind = kind, .value = value };
  return ast;
}

// -------------------------------------------------------------------------- //

void
release_ast_const(Ast* ast_const)
{
  LN_AST_KIND_CHECK(ast_const->kind == kAstConst);
  release(ast_const);
}

// -------------------------------------------------------------------------- //

AstConstKind
ast_const_get_kind(Ast* ast_const)
{
  LN_AST_KIND_CHECK(ast_const->kind == kAstConst);
  return ast_const->constant.kind;
}

// -------------------------------------------------------------------------- //

u64
ast_const_to_u64(Ast* ast_const)
{
  // Preconditions
  LN_AST_KIND_CHECK(ast_const->kind == kAstConst);
  assrt(
    ast_const->constant.kind == kAstConstInt,
    make_str("Cannot call 'ast_const_to_u64' when const kind is not 'int'"));

  // Convert
  char* end_ptr = NULL;
  u64 value = strtoull((char*)ast_const->constant.value.ptr, &end_ptr, 10);
  assrt(end_ptr != (char*)ast_const->constant.value.ptr,
        make_str("Const int could not be converted to it's 'u64' value"));
  return value;
}

// -------------------------------------------------------------------------- //

void
ast_const_dump(Ast* ast, u32 indent)
{
  LN_AST_KIND_CHECK(ast->kind == kAstConst);
  printf(
    "%*sconst: '%.*s'\n", indent, "", str_slice_print(&ast->constant.value));
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
  LN_AST_KIND_CHECK(ast->kind == kAstType);
  release(ast);
}

// -------------------------------------------------------------------------- //

void
ast_type_dump(Ast* ast, u32 indent)
{
  LN_AST_KIND_CHECK(ast->kind == kAstType);
  Str type_str = type_to_str(ast->type.type);
  printf("%*stype: '%s'\n", indent, "", str_cstr(&type_str));
  release_str(&type_str);
}

// ========================================================================== //
// Ast
// ========================================================================== //

Ast*
make_ast_invalid()
{
  Ast* ast = alloc(sizeof(Ast), kLnMinAlign);
  assrt(ast != NULL, make_str("Allocation of AST node failed"));
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
      break;
    }
    case kAstFn: {
      release_ast_fn(ast);
      break;
    }
    case kAstParam: {
      release_ast_param(ast);
      break;
    }
    case kAstBlock: {
      release_ast_block(ast);
      break;
    }
    case kAstLet: {
      release_ast_let(ast);
      break;
    }
    case kAstRet: {
      release_ast_ret(ast);
      break;
    }
    case kAstBinop: {
      release_ast_binop(ast);
      break;
    }
    case kAstConst: {
      release_ast_const(ast);
      break;
    }
    case kAstType: {
      release_ast_type(ast);
      break;
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
  if (!ast) {
    return false;
  }
  return ast->kind == kAstLet || ast->kind == kAstRet;
}

// -------------------------------------------------------------------------- //

/* Check if ast is expr */
bool
ast_is_expr(Ast* ast)
{
  if (!ast) {
    return false;
  }
  return ast->kind == kAstBinop || ast->kind == kAstConst;
}

// -------------------------------------------------------------------------- //

void
ast_dump(Ast* ast)
{
  printf("[Ast]\n");
  ast_dump_aux(ast, 0);
}

// -------------------------------------------------------------------------- //

void
ast_dump_aux(Ast* ast, u32 indent)
{
  switch (ast->kind) {
    case kAstInvalid: {
      panic(make_str("Cannot release invalid ast"));
    }
    case kAstProg: {
      ast_prog_dump(ast, indent);
      break;
    }
    case kAstFn: {
      ast_fn_dump(ast, indent);
      break;
    }
    case kAstParam: {
      ast_param_dump(ast, indent);
      break;
    }
    case kAstBlock: {
      ast_block_dump(ast, indent);
      break;
    }
    case kAstLet: {
      ast_let_dump(ast, indent);
      break;
    }
    case kAstRet: {
      ast_ret_dump(ast, indent);
      break;
    }
    case kAstBinop: {
      ast_binop_dump(ast, indent);
      break;
    }
    case kAstConst: {
      ast_const_dump(ast, indent);
      break;
    }
    case kAstType: {
      ast_type_dump(ast, indent);
      break;
    }
    default: {
      panic(make_str("Invalid token kind"));
    }
  }
}