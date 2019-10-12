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

#ifndef LN_AST_H
#define LN_AST_H

#include "str.h"
#include "span.h"
#include "type.h"

typedef struct AstProg AstProg;
typedef struct AstFn AstFn;
typedef struct AstParam AstParam;
typedef struct AstBlock AstBlock;
typedef struct AstLet AstLet;
typedef struct AstRet AstRet;
typedef struct AstBinop AstBinop;
typedef struct AstConst AstConst;
typedef struct AstType AstType;
typedef struct Ast Ast;

// ========================================================================== //
// AstKind
// ========================================================================== //

/* Ast node kind */
typedef enum AstKind
{
  /* Invalid ast type */
  kAstInvalid = 0,
  /* Program node */
  kAstProg,
  /* Function node */
  kAstFn,
  /* Param node */
  kAstParam,
  /* Block node */
  kAstBlock,
  /* Let node */
  kAstLet,
  /* Ret node */
  kAstRet,
  /* Binop node */
  kAstBinop,
  /* Const node */
  kAstConst,
  /* Tupe node */
  kAstType
} AstKind;

// ========================================================================== //
// AstList
// ========================================================================== //

/* List of Ast structs */
typedef struct AstList
{
  /* buffer */
  Ast** buf;
  /* length */
  u32 len;
  /* capacity */
  u32 cap;
} AstList;

// -------------------------------------------------------------------------- //

/* Make empty ast list */
AstList
make_ast_list(u32 cap);

// -------------------------------------------------------------------------- //

/* Release ast list */
void
release_ast_list(AstList* list);

// -------------------------------------------------------------------------- //

/* Append to ast list */
void
ast_list_append(AstList* list, Ast* ast);

// -------------------------------------------------------------------------- //

/* Remove from ast list */
Ast*
ast_list_remove(AstList* list, u32 index);

// -------------------------------------------------------------------------- //

/* Get object from ast list */
Ast*
ast_list_get(AstList* list, u32 index);

// -------------------------------------------------------------------------- //

/* Reserve capacity in ast list */
void
ast_list_reserve(AstList* list, u32 cap);

// ========================================================================== //
// AstProg
// ========================================================================== //

/* Ast program node */
typedef struct AstProg
{
  /* Functions */
  AstList funs;
} AstProg;

// -------------------------------------------------------------------------- //

Ast*
make_ast_prog();

// -------------------------------------------------------------------------- //

void
release_ast_prog(Ast* ast);

// -------------------------------------------------------------------------- //

void
ast_prog_add_fn(Ast* ast_prog, Ast* ast_fn);

// -------------------------------------------------------------------------- //

/* Dump */
void
ast_prog_dump(Ast* ast, u32 indent);

// ========================================================================== //
// AstFn
// ========================================================================== //

/* Ast function node */
typedef struct AstFn
{
  /* Name */
  StrSlice name;
  /* List of parameter nodes (AstParam) */
  AstList params;
  /* Return type node (AstType) */
  Ast* ret;
  /* Body */
  Ast* body;
} AstFn;

// -------------------------------------------------------------------------- //

Ast*
make_ast_fn(StrSlice name);

// -------------------------------------------------------------------------- //

void
release_ast_fn(Ast* ast);

// -------------------------------------------------------------------------- //

/* Add parameter to function (AstParam) */
void
ast_fn_add_param(Ast* ast_fn, Ast* ast_param);

// -------------------------------------------------------------------------- //

/* Set ret type of function (AstType) */
void
ast_fn_set_ret(Ast* ast_fn, Ast* ast_ret);

// -------------------------------------------------------------------------- //

/* Dump */
void
ast_fn_dump(Ast* ast, u32 indent);

// ========================================================================== //
// AstParam
// ========================================================================== //

/* Param node */
typedef struct AstParam
{
  /* Name */
  StrSlice name;
  /* Type (AstType) */
  Ast* type;
} AstParam;

// -------------------------------------------------------------------------- //

Ast*
make_ast_param();

// -------------------------------------------------------------------------- //

void
release_ast_param(Ast* ast);

// -------------------------------------------------------------------------- //

/* Dump */
void
ast_param_dump(Ast* ast, u32 indent);

// ========================================================================== //
// AstBlock
// ========================================================================== //

typedef struct AstBlock
{
  /* Statement list (AstStmt) */
  AstList stmts;
  /* Last expr in block*/
  Ast* ret_expr;
} AstBlock;

// -------------------------------------------------------------------------- //

Ast*
make_ast_block();

// -------------------------------------------------------------------------- //

void
release_ast_block(Ast* ast);

// -------------------------------------------------------------------------- //

void
ast_block_add_stmt(Ast* ast_block, Ast* ast_stmt);

// -------------------------------------------------------------------------- //

/* Dump */
void
ast_block_dump(Ast* ast, u32 indent);

// ========================================================================== //
// AstLet
// ========================================================================== //

typedef struct AstLet
{
  /* Name */
  StrSlice name;
  /* Optional type */
  Ast* type;
  /* Assigned expr */
  Ast* expr;
} AstLet;

// -------------------------------------------------------------------------- //

/* Make 'let' node */
Ast*
make_ast_let();

// -------------------------------------------------------------------------- //

/* Release 'let' node */
void
release_ast_let(Ast* ast_let);

// -------------------------------------------------------------------------- //

/* Set let name */
void
ast_let_set_name(Ast* ast_let, StrSlice name);

// -------------------------------------------------------------------------- //

/* Set type */
void
ast_let_set_type(Ast* ast_let, Ast* ast_type);

// -------------------------------------------------------------------------- //

/* Set expr that is being assigned */
void
ast_let_set_assigned(Ast* ast_let, Ast* ast_expr);

// -------------------------------------------------------------------------- //

/* Dump */
void
ast_let_dump(Ast* ast, u32 indent);

// ========================================================================== //
// AstRet
// ========================================================================== //

typedef struct AstRet
{
  /* Ret expr */
  Ast* expr;
} AstRet;

// -------------------------------------------------------------------------- //

Ast*
make_ast_ret(Ast* ast_expr);

// -------------------------------------------------------------------------- //

void
release_ast_ret(Ast* ast_ret);

// -------------------------------------------------------------------------- //

/* Dump */
void
ast_ret_dump(Ast* ast, u32 indent);

// ========================================================================== //
// AstBinop
// ========================================================================== //

typedef enum AstBinopKind
{
  kAstBinopAdd,
  kAstBinopSub,
  kAstBinopMul,
  kAstBinopDiv,
  kAstBinopMod,
} AstBinopKind;

// -------------------------------------------------------------------------- //

typedef struct AstBinop
{
  /* Kind */
  AstBinopKind kind;
  /* Left-hand side */
  Ast* lhs;
  /* Right-hand side */
  Ast* rhs;
} AstBinop;

// -------------------------------------------------------------------------- //

Ast*
make_ast_binop(AstBinopKind kind);

// -------------------------------------------------------------------------- //

void
release_ast_binop(Ast* ast_binop);

// -------------------------------------------------------------------------- //

void
ast_binop_set_kind(Ast* ast_binop, AstBinopKind kind);

// -------------------------------------------------------------------------- //

void
ast_binop_set_lhs(Ast* ast_binop, Ast* ast_lhs);

// -------------------------------------------------------------------------- //

void
ast_binop_set_rhs(Ast* ast_binop, Ast* ast_rhs);

// -------------------------------------------------------------------------- //

/* Dump */
void
ast_binop_dump(Ast* ast, u32 indent);

// ========================================================================== //
// AstConst
// ========================================================================== //

typedef enum AstConstKind
{
  kAstConstInt,
  kAstConstFloat,
  kAstConstStr,
} AstConstKind;

// -------------------------------------------------------------------------- //

typedef struct AstConst
{
  /* Const kind */
  AstConstKind kind;
  /* Value */
  StrSlice value;
} AstConst;

// -------------------------------------------------------------------------- //

Ast*
make_ast_const(AstConstKind kind, StrSlice value);

// -------------------------------------------------------------------------- //

void
release_ast_const(Ast* ast_const);

// -------------------------------------------------------------------------- //

AstConstKind
ast_const_get_kind(Ast* ast_const);

// -------------------------------------------------------------------------- //

u64
ast_const_to_u64(Ast* ast_const);

// -------------------------------------------------------------------------- //

/* Dump */
void
ast_const_dump(Ast* ast, u32 indent);

// ========================================================================== //
// AstType
// ========================================================================== //

/* Type node */
typedef struct AstType
{
  /* Type */
  Type* type;
} AstType;

// -------------------------------------------------------------------------- //

Ast*
make_ast_type(Type* type);

// -------------------------------------------------------------------------- //

void
release_ast_type(Ast* ast);

// -------------------------------------------------------------------------- //

/* Dump */
void
ast_type_dump(Ast* ast, u32 indent);

// ========================================================================== //
// Ast
// ========================================================================== //

/* Abstract syntax tree */
typedef struct Ast
{
  /* Span */
  Span span;
  /* Kind */
  AstKind kind;
  union
  {
    /* Program */
    AstProg prog;
    /* Function */
    AstFn fn;
    /* Param */
    AstParam param;
    /* Block */
    AstBlock block;
    /* Let */
    AstLet let;
    /* Ret */
    AstRet ret;
    /* Binop */
    AstBinop binop;
    /* Constant */
    AstConst constant;
    /* Type */
    AstType type;
  };
} Ast;

// -------------------------------------------------------------------------- //

Ast*
make_ast_invalid();

// -------------------------------------------------------------------------- //

/* Recursively release ast */
void
release_ast(Ast* ast);

// -------------------------------------------------------------------------- //

/* Check if ast is stmt */
bool
ast_is_stmt(Ast* ast);

// -------------------------------------------------------------------------- //

/* Check if ast is expr */
bool
ast_is_expr(Ast* ast);

// -------------------------------------------------------------------------- //

/* Dump ast */
void
ast_dump(Ast* ast);

// -------------------------------------------------------------------------- //

/* Dump ast at indentatin */
void
ast_dump_aux(Ast* ast, u32 indent);

#endif // LN_AST_H