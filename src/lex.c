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

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "lex.h"

// ========================================================================== //
// TokKind
// ========================================================================== //

#define TOK_STR_CASE(name)                                                     \
  case name:                                                                   \
    return make_str(#name);

// -------------------------------------------------------------------------- //

Str
tok_kind_str(TokKind kind)
{
  switch (kind) {
    TOK_STR_CASE(kTokWhitespace)
    TOK_STR_CASE(kTokIdent)
    TOK_STR_CASE(kTokInt)
    TOK_STR_CASE(kTokFloat)
    TOK_STR_CASE(kTokStr)
    TOK_STR_CASE(kTokKeyword)
    TOK_STR_CASE(kTokSym)
    default: {
      return make_str("unknown");
    }
  }
}

// ========================================================================== //
// TokKwKind
// ========================================================================== //

#define LN_TOK_KW_EQ(name, kind)                                               \
  if (str_slice_eq_str(slice, &make_str(name))) {                              \
    *p_kind = kind;                                                            \
    return true;                                                               \
  }

bool
tok_kw_kind_get(StrSlice* slice, TokKwKind* p_kind)
{
  LN_TOK_KW_EQ("do", kTokKwDo)
  LN_TOK_KW_EQ("elif", kTokKwElif)
  LN_TOK_KW_EQ("else", kTokKwElse)
  LN_TOK_KW_EQ("enum", kTokKwEnum)
  LN_TOK_KW_EQ("for", kTokKwFor)
  LN_TOK_KW_EQ("fn", kTokKwFn)
  LN_TOK_KW_EQ("if", kTokKwIf)
  LN_TOK_KW_EQ("import", kTokKwImport)
  LN_TOK_KW_EQ("let", kTokKwLet)
  LN_TOK_KW_EQ("match", kTokKwMatch)
  LN_TOK_KW_EQ("module", kTokKwModule)
  LN_TOK_KW_EQ("ret", kTokKwRet)
  LN_TOK_KW_EQ("self", kTokKwSelf)
  LN_TOK_KW_EQ("struct", kTokKwStruct)
  LN_TOK_KW_EQ("trait", kTokKwTrait)
  LN_TOK_KW_EQ("type", kTokKwType)
  LN_TOK_KW_EQ("while", kTokKwWhile)
  return false;
}

#undef LN_TOK_KW_EQ

// ========================================================================== //
// TokSymKind
// ========================================================================== //

#define LN_TOK_SYM_EQ(name, kind)                                              \
  if (str_slice_eq_str(slice, &make_str(name))) {                              \
    *p_kind = kind;                                                            \
    return true;                                                               \
  }

bool
tok_sym_kind_get(StrSlice* slice, TokSymKind* p_kind)
{
  LN_TOK_SYM_EQ("+", kTokSymAdd);
  LN_TOK_SYM_EQ("-", kTokSymSub);
  LN_TOK_SYM_EQ("*", kTokSymMul);
  LN_TOK_SYM_EQ("/", kTokSymDiv);
  LN_TOK_SYM_EQ("%", kTokSymMod);
  LN_TOK_SYM_EQ("&", kTokSymAnd);
  LN_TOK_SYM_EQ("|", kTokSymOr);
  LN_TOK_SYM_EQ("^", kTokSymXor);
  LN_TOK_SYM_EQ("~", kTokSymInvert);
  LN_TOK_SYM_EQ("<", kTokSymLess);
  LN_TOK_SYM_EQ(">", kTokSymGreater);
  LN_TOK_SYM_EQ("=", kTokSymEqual);
  LN_TOK_SYM_EQ("!", kTokSymExcl);
  LN_TOK_SYM_EQ("?", kTokSymQmark);
  LN_TOK_SYM_EQ("(", kTokSymLeftParen);
  LN_TOK_SYM_EQ(")", kTokSymRightParen);
  LN_TOK_SYM_EQ("[", kTokSymLeftBracket);
  LN_TOK_SYM_EQ("]", kTokSymRightBracket);
  LN_TOK_SYM_EQ("{", kTokSymLeftBrace);
  LN_TOK_SYM_EQ("}", kTokSymRightBrace);
  LN_TOK_SYM_EQ(":", kTokSymColon);
  LN_TOK_SYM_EQ(";", kTokSymSemicolon);
  LN_TOK_SYM_EQ(",", kTokSymComma);
  LN_TOK_SYM_EQ("'", kTokSymApostrophe);
  LN_TOK_SYM_EQ(".", kTokSymPeriod);
  return false;
}

#undef LN_TOK_SYM_EQ

// ========================================================================== //
// Tok
// ========================================================================== //

Tok
make_tok(TokKind kind, StrSlice value, Span span)
{
  return (Tok){ .kind = kind, .value = value, .span = span };
}

// -------------------------------------------------------------------------- //

bool
tok_is_kw(const Tok* tok, TokKwKind kind)
{
  return tok->kind == kTokKeyword && tok->data.kw_kind == kind;
}

// -------------------------------------------------------------------------- //

bool
tok_is_sym(const Tok* tok, TokSymKind kind)
{
  return tok->kind == kTokSym && tok->data.sym_kind == kind;
}

// ========================================================================== //
// Lex
// ========================================================================== //

/* Lexer context */
typedef struct Lex
{
  /* List to fill */
  TokList* list;
  /* Src */
  const Src* src;
  /* Src iter */
  StrIter iter;

  /* Current line */
  u32 line;
  /* Current col */
  u32 col;
} Lex;

// -------------------------------------------------------------------------- //

Lex
make_lex(TokList* list, const Src* src)
{
  return (Lex){ .list = list,
                .src = src,
                .iter = make_str_iter(&src->src),
                .line = 0,
                .col = 0 };
}

// -------------------------------------------------------------------------- //

u32
lex_next(Lex* lex)
{
  u32 code_point = str_iter_next(&lex->iter);
  if (code_point != kInvalidCodepoint) {
    lex->col++;
    if (code_point == '\n') {
      lex->col = 0;
      lex->line++;
    }
  }
  return code_point;
}

// -------------------------------------------------------------------------- //

u32
lex_peek(const Lex* lex)
{
  return str_iter_peek(&lex->iter);
}

// -------------------------------------------------------------------------- //

Pos
lex_pos_cur(const Lex* lex)
{
  return make_pos(lex->iter.off, lex->line, lex->col);
}

// -------------------------------------------------------------------------- //

StrSlice
lex_span_slice(Lex* lex, const Span* span)
{
  return span_slice(span, &lex->src->src);
}

// -------------------------------------------------------------------------- //

bool
lex_is_whitespace(u32 code_point)
{
  return code_point == ' ';
}

// -------------------------------------------------------------------------- //

bool
lex_is_num(u32 code_point)
{
  return code_point >= '0' && code_point <= '9';
}

// -------------------------------------------------------------------------- //

bool
lex_is_num_sym(u32 code_point)
{
  return lex_is_num(code_point) || (code_point >= 'a' && code_point <= 'f') ||
         (code_point >= 'A' && code_point <= 'F') || code_point == 'x' ||
         code_point == 'X' || code_point == '.' || code_point == 'o' ||
         code_point == 'O' || code_point == 'h' || code_point == 'H' ||
         code_point == 'u' || code_point == 'U';
}

// -------------------------------------------------------------------------- //

bool
lex_is_alfa(u32 code_point)
{
  return (code_point >= 'a' && code_point <= 'z') ||
         (code_point >= 'A' && code_point <= 'Z');
}

// -------------------------------------------------------------------------- //

bool
lex_is_unicode(u32 code_point)
{
  return code_point > 0x7F && code_point != kInvalidCodepoint;
}

// -------------------------------------------------------------------------- //

bool
lex_is_special(u32 code_point)
{
  return code_point == '+' || code_point == '-' || code_point == '*' ||
         code_point == '/' || code_point == '%' || code_point == '&' ||
         code_point == '|' || code_point == '~' || code_point == '<' ||
         code_point == '>' || code_point == '=' || code_point == '!' ||
         code_point == '?' || code_point == '(' || code_point == ')' ||
         code_point == '[' || code_point == ']' || code_point == '{' ||
         code_point == '}' || code_point == ':' || code_point == ';' ||
         code_point == ',' || code_point == '.';
}

// -------------------------------------------------------------------------- //

LexErr
lex_handle_whitespace(Lex* lex)
{
  Pos beg = lex_pos_cur(lex);
  u32 code_point;
  while ((code_point = lex_peek(lex)) != kInvalidCodepoint) {
    if (!lex_is_whitespace(code_point)) {
      break;
    }
    lex_next(lex);
  }
  Pos end = lex_pos_cur(lex);
  if (beg.off == end.off) {
    return kLexNoErr;
  }

  Span span = make_span(beg, end);
  StrSlice value = lex_span_slice(lex, &span);
  Tok tok = make_tok(kTokWhitespace, value, span);
  tok_list_push(lex->list, &tok);
  return kLexNoErr;
}

// -------------------------------------------------------------------------- //

LexErr
lex_handle_newline(Lex* lex)
{
  u32 code_point = lex_peek(lex);
  if (code_point == '\n') {
    lex_next(lex);
  }
  return kLexNoErr;
}

// -------------------------------------------------------------------------- //

LexErr
lex_handle_ident(Lex* lex)
{
  u32 code_point = lex_peek(lex);
  if (!lex_is_alfa(code_point) && code_point != '_' &&
      !lex_is_unicode(code_point)) {
    return kLexNoErr;
  }

  Pos beg = lex_pos_cur(lex);
  while ((code_point = lex_peek(lex)) != kInvalidCodepoint) {
    if (!lex_is_alfa(code_point) && code_point != '_' &&
        !lex_is_unicode(code_point) && !lex_is_num(code_point)) {
      break;
    }
    lex_next(lex);
  }
  Pos end = lex_pos_cur(lex);

  Span span = make_span(beg, end);
  StrSlice value = lex_span_slice(lex, &span);
  TokKwKind kw_kind;
  bool is_kw = tok_kw_kind_get(&value, &kw_kind);
  Tok tok = make_tok(is_kw ? kTokKeyword : kTokIdent, value, span);
  if (is_kw) {
    tok.data.kw_kind = kw_kind;
  }
  tok_list_push(lex->list, &tok);
  return kLexNoErr;
}

// -------------------------------------------------------------------------- //

LexErr
lex_handle_num(Lex* lex)
{
  u32 code_point = lex_peek(lex);
  if (!lex_is_num(code_point)) {
    return kLexNoErr;
  }

  Pos beg = lex_pos_cur(lex);
  while ((code_point = lex_peek(lex)) != kInvalidCodepoint) {
    if (!lex_is_num_sym(code_point)) {
      break;
    }
    lex_next(lex);
  }
  Pos end = lex_pos_cur(lex);

  // TODO(Filip Björklund): Handle float + int

  Span span = make_span(beg, end);
  StrSlice value = lex_span_slice(lex, &span);
  Tok tok = make_tok(kTokInt, value, span);
  tok_list_push(lex->list, &tok);
  return kLexNoErr;
}
// -------------------------------------------------------------------------- //

LexErr
lex_handle_str(Lex* lex)
{
  u32 code_point = lex_peek(lex);
  if (code_point != '\"') {
    return kLexNoErr;
  }
  Pos beg = lex_pos_cur(lex);
  lex_next(lex);

  bool found_end = false;
  bool escaped = false;
  while ((code_point = lex_peek(lex)) != kInvalidCodepoint) {
    lex_next(lex);
    if (code_point == '\"' && !escaped) {
      found_end = true;
      break;
    }
    escaped = code_point == '\\';
  }
  Pos end = lex_pos_cur(lex);

  if (!found_end) {
    return kLexNonTermStr;
  }

  Span span = make_span(beg, end);
  StrSlice value = lex_span_slice(lex, &span);
  Tok tok = make_tok(kTokStr, value, span);
  tok_list_push(lex->list, &tok);
  return kLexNoErr;
}

// -------------------------------------------------------------------------- //

LexErr
lex_handle_special(Lex* lex)
{
  u32 code_point = lex_peek(lex);
  if (!lex_is_special(code_point)) {
    return kLexNoErr;
  }

  Pos beg = lex_pos_cur(lex);
  lex_next(lex);
  Pos end = lex_pos_cur(lex);

  Span span = make_span(beg, end);
  StrSlice value = lex_span_slice(lex, &span);
  TokSymKind sym_kind;
  bool is_sym = tok_sym_kind_get(&value, &sym_kind);
  if (!is_sym) {
    return kLexUnexpectedSym;
  }
  Tok tok = make_tok(kTokSym, value, span);
  tok.data.sym_kind = sym_kind;
  tok_list_push(lex->list, &tok);
  return kLexNoErr;
}

// ========================================================================== //
// TokList
// ========================================================================== //

TokList
make_tok_list()
{
  u32 cap = 32;
  Tok* buf = alloc(sizeof(Tok) * cap, kLnMinAlign);
  return (TokList){ .buf = buf, .len = 0, .cap = cap };
}

// -------------------------------------------------------------------------- //

void
release_tok_list(TokList* list)
{
  release(list->buf);
}

// -------------------------------------------------------------------------- //

#define TOK_LIST_CHECK_ERR()                                                   \
  do {                                                                         \
    if (err != kLexNoErr) {                                                    \
      release_tok_list(&list);                                                 \
      return err;                                                              \
    }                                                                          \
  } while (0)

LexErr
tok_list_lex(const Src* src, TokList* p_list)
{
  *p_list = (TokList){ .buf = NULL, .len = 0, .cap = 0 };

  TokList list = make_tok_list();
  Lex lex = make_lex(&list, src);

  u32 code_point;
  while ((code_point = lex_peek(&lex)) != kInvalidCodepoint) {
    Pos beg = lex_pos_cur(&lex);

    LexErr err = lex_handle_whitespace(&lex);
    TOK_LIST_CHECK_ERR();

    err = lex_handle_newline(&lex);
    TOK_LIST_CHECK_ERR();

    err = lex_handle_ident(&lex);
    TOK_LIST_CHECK_ERR();

    err = lex_handle_num(&lex);
    TOK_LIST_CHECK_ERR();

    err = lex_handle_str(&lex);
    TOK_LIST_CHECK_ERR();

    err = lex_handle_special(&lex);
    TOK_LIST_CHECK_ERR();

    Pos end = lex_pos_cur(&lex);
    err = (beg.off == end.off) ? kLexUnexpectedSym : kLexNoErr;
    TOK_LIST_CHECK_ERR();
  }

  *p_list = list;
  return kLexNoErr;
}

// -------------------------------------------------------------------------- //

void
tok_list_push(TokList* list, const Tok* tok)
{
  if (list->len >= list->cap) {
    tok_list_reserve(list, list->cap * 2);
  }
  memcpy(list->buf + list->len++, tok, sizeof(Tok));
}

// -------------------------------------------------------------------------- //

const Tok*
tok_list_get(const TokList* list, u32 index)
{
  assrt(index < list->len, make_str("Index out of bounds"));
  return &list->buf[index];
}

// -------------------------------------------------------------------------- //

const Tok*
tok_list_last(const TokList* list)
{
  return tok_list_get(list, list->len - 1);
}

// -------------------------------------------------------------------------- //

void
tok_list_reserve(TokList* list, u32 cap)
{
  if (list->cap > cap) {
    return;
  }
  Tok* buf = alloc(sizeof(Tok) * cap, kLnMinAlign);
  memcpy(buf, list->buf, sizeof(Tok) * list->len);
  release(list->buf);
  list->buf = buf;
  list->cap = cap;
}

// -------------------------------------------------------------------------- //

u32
tok_list_len(const TokList* list)
{
  return list->len;
}

// -------------------------------------------------------------------------- //

void
tok_list_dump(const TokList* list)
{
  const Tok* tok;
  TokIter iter = make_tok_iter(list);
  printf("TokList:\n");
  while ((tok = tok_iter_next(&iter)) != NULL) {
    Str kind_name = tok_kind_str(tok->kind);
    Str value = str_slice_to_string(&tok->value);
    printf("  %s ('%s') @{%u:%u -> %u:%u}\n",
           str_cstr(&kind_name),
           str_cstr(&value),
           tok->span.beg.line + 1,
           tok->span.beg.col + 1,
           tok->span.end.line + 1,
           tok->span.end.col + 1);
    release_str(&value);
  }
}

// ========================================================================== //
// TokIter
// ========================================================================== //

TokIter
make_tok_iter(const TokList* list)
{
  return (TokIter){ .list = list, .idx = 0 };
}

// -------------------------------------------------------------------------- //

const Tok*
tok_iter_next(TokIter* iter)
{
  if (iter->idx >= iter->list->len) {
    return NULL;
  }
  return &iter->list->buf[iter->idx++];
}

// -------------------------------------------------------------------------- //

const Tok*
tok_iter_peek(const TokIter* iter)
{
  if (iter->idx >= iter->list->len) {
    return NULL;
  }
  return &iter->list->buf[iter->idx];
}