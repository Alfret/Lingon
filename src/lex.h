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

#ifndef LN_LEX_H
#define LN_LEX_H

#include "common.h"
#include "str.h"
#include "span.h"
#include "src.h"

// ========================================================================== //
// LexErr
// ========================================================================== //

/* Lexer errors */
typedef enum LexErr
{
  /* No error */
  kLexNoErr,
  /* Unexpected symbol */
  kLexUnexpectedSym,
  /* String not terminated */
  kLexNonTermStr,
} LexErr;

// ========================================================================== //
// TokKind
// ========================================================================== //

typedef enum TokKind
{
  kTokWhitespace,
  kTokIdent,
  kTokInt,
  kTokFloat,
  kTokStr,
  kTokKeyword,
  kTokSym,
} TokKind;

// -------------------------------------------------------------------------- //

/* Returns token kind as string */
Str
tok_kind_str(TokKind kind);

// ========================================================================== //
// TokKwKind
// ========================================================================== //

typedef enum TokKwKind
{
  kTokKwDo,
  kTokKwElif,
  kTokKwElse,
  kTokKwEnum,
  kTokKwFor,
  kTokKwFn,
  kTokKwIf,
  kTokKwImport,
  kTokKwLet,
  kTokKwMatch,
  kTokKwModule,
  kTokKwRet,
  kTokKwSelf,
  kTokKwStruct,
  kTokKwTrait,
  kTokKwType,
  kTokKwWhile,
} TokKwKind;

// -------------------------------------------------------------------------- //

bool
tok_kw_kind_get(StrSlice* slice, TokKwKind* p_kind);

// ========================================================================== //
// TokSymKind
// ========================================================================== //

typedef enum TokSymKind
{
  kTokSymAdd,
  kTokSymSub,
  kTokSymMul,
  kTokSymDiv,
  kTokSymMod,
  kTokSymAnd,
  kTokSymOr,
  kTokSymXor,
  kTokSymInvert,
  kTokSymLess,
  kTokSymGreater,
  kTokSymEqual,
  kTokSymExcl,
  kTokSymQmark,
  kTokSymLeftParen,
  kTokSymRightParen,
  kTokSymLeftBracket,
  kTokSymRightBracket,
  kTokSymLeftBrace,
  kTokSymRightBrace,
  kTokSymColon,
  kTokSymSemicolon,
  kTokSymComma,
  kTokSymApostrophe,
  kTokSymPeriod,
} TokSymKind;

// -------------------------------------------------------------------------- //

bool
tok_sym_kind_get(StrSlice* slice, TokSymKind* p_kind);

// ========================================================================== //
// Tok
// ========================================================================== //

/* Token */
typedef struct Tok
{
  /* Kind of token */
  TokKind kind;
  /* Value slice from source */
  StrSlice value;
  /* Span */
  Span span;
  /* Extra data */
  union
  {
    TokKwKind kw_kind;   // Set for kind == kTokKeyword
    TokSymKind sym_kind; // Set for kind == kTokSym
  } data;
} Tok;

// -------------------------------------------------------------------------- //

/* Make token */
Tok
make_tok(TokKind kind, StrSlice value, Span span);

// -------------------------------------------------------------------------- //

/* Check if token is of specified keyword type */
bool
tok_is_kw(const Tok* tok, TokKwKind kind);

// -------------------------------------------------------------------------- //

/* Check if token is of specified keyword type */
bool
tok_is_sym(const Tok* tok, TokSymKind kind);

// ========================================================================== //
// TokList
// ========================================================================== //

typedef struct TokList
{
  Tok* buf;
  u32 len;
  u32 cap;
} TokList;

// -------------------------------------------------------------------------- //

/* Make empty token list */
TokList
make_tok_list();

// -------------------------------------------------------------------------- //

/* Release token list */
void
release_tok_list(TokList* list);

// -------------------------------------------------------------------------- //

/* Lexical analysis */
LexErr
tok_list_lex(const Src* src, TokList* p_list);

// -------------------------------------------------------------------------- //

/* Push token at end of list */
void
tok_list_push(TokList* list, const Tok* tok);

// -------------------------------------------------------------------------- //

/* Reserve capacity */
void
tok_list_reserve(TokList* list, u32 cap);

// -------------------------------------------------------------------------- //

/* Returns token list length */
u32
tok_list_len(const TokList* list);

// -------------------------------------------------------------------------- //

/* Debug utility to dump token stream */
void
tok_list_dump(const TokList* list);

// ========================================================================== //
// TokIter
// ========================================================================== //

typedef struct TokIter
{
  const TokList* list;
  u32 idx;
} TokIter;

// -------------------------------------------------------------------------- //

TokIter
make_tok_iter(const TokList* list);

// -------------------------------------------------------------------------- //

const Tok*
tok_iter_next(TokIter* iter);

// -------------------------------------------------------------------------- //

const Tok*
tok_iter_peek(const TokIter* iter);

#endif // LN_LEX_H