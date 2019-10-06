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
#include "pos.h"

typedef struct AstProg AstProg;
typedef struct AstFn AstFn;
typedef struct Ast Ast;

// ========================================================================== //
// AstList
// ========================================================================== //

/* List of Ast structs */
typedef struct AstList
{
  /* buffer */
  Ast* buf;
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
ast_list_append(AstList* list, const Ast* ast);

// -------------------------------------------------------------------------- //

/* Remove from ast list */
Ast
ast_list_remove(AstList* list, u32 index);

// -------------------------------------------------------------------------- //

/* Get object from ast list */
const Ast*
ast_list_get(const AstList* list, u32 index);

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

Ast
make_ast_prog();

// -------------------------------------------------------------------------- //

void
release_ast_prog(Ast* ast);

// -------------------------------------------------------------------------- //

void
ast_prog_add_fn(Ast* ast_prog, Ast* ast_fn);

// ========================================================================== //
// AstFn
// ========================================================================== //

/* Ast function node */
typedef struct AstFn
{
  StrSlice name;
} AstFn;

// -------------------------------------------------------------------------- //

Ast
make_ast_fn(StrSlice name);

// -------------------------------------------------------------------------- //

void
release_ast_fn(Ast* ast);

// ========================================================================== //
// Ast
// ========================================================================== //

/* Ast node kind */
typedef enum AstKind
{
  /* Invalid ast type */
  kAstInvalid = 0,
  /* Program node */
  kAstProg,
  /* Function node */
  kAstFn
} AstKind;

// -------------------------------------------------------------------------- //

/* Abstract syntax tree */
typedef struct Ast
{
  /* Pos */
  RangePos pos;
  /* Kind */
  AstKind kind;
  union
  {
    /* Program node */
    AstProg prog;
    /* Function node */
    AstFn fn;
  };
} Ast;

// -------------------------------------------------------------------------- //

/* Make an invalid ast node */
Ast
make_ast_invalid();

// -------------------------------------------------------------------------- //

/* Recursively release ast */
void
release_ast(Ast* ast);

#endif // LN_AST_H