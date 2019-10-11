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
#include <stdio.h>

#include "parser.h"
#include "lex.h"
#include "con.h"
#include "err.h"

// ========================================================================== //
// Macros
// ========================================================================== //

#define LN_PARSE_TOK_ASSERT_NEXT_SYM(fn_name, tok_name)                        \
  do {                                                                         \
    assrt(parser_expect_sym(parser, tok_name, false),                          \
          make_str("'" fn_name "' must only be called when next token is "     \
                   "'" #tok_name "'"));                                        \
  } while (0)

// -------------------------------------------------------------------------- //

#define LN_PARSE_TOK_ASSERT_NEXT_KW(fn_name, tok_name)                         \
  do {                                                                         \
    assrt(parser_expect_kw(parser, tok_name, false),                           \
          make_str("'" fn_name "' must only be called when next token is "     \
                   "'" #tok_name "'"));                                        \
  } while (0)

// ========================================================================== //
// Funs
// ========================================================================== //

static Ast*
parse_prog(Parser* parser);

static Ast*
parse_fn(Parser* parser);

static Ast*
parse_block(Parser* parser);

static Ast*
parse_stmt(Parser* parser);

static Ast*
parse_type(Parser* parser);

// ========================================================================== //
// Util
// ========================================================================== //

bool
parser_expect(Parser* parser, TokKind kind, bool skip_ws)
{
  if (skip_ws) {
    parser_consume_whitespace(parser);
  }
  const Tok* tok = parser_peek(parser);
  return tok->kind == kind;
}

// -------------------------------------------------------------------------- //

bool
parser_expect_kw(Parser* parser, TokKwKind kw_kind, bool skip_whitespace)
{
  if (parser_expect(parser, kTokKeyword, skip_whitespace)) {
    const Tok* tok = parser_peek(parser);
    return tok->data.kw_kind == kw_kind;
  }
  return false;
}

// -------------------------------------------------------------------------- //

bool
parser_expect_sym(Parser* parser, TokSymKind sym_kind, bool skip_whitespace)
{
  if (parser_expect(parser, kTokSym, skip_whitespace)) {
    const Tok* tok = parser_peek(parser);
    return tok->data.sym_kind == sym_kind;
  }
  return false;
}

// -------------------------------------------------------------------------- //

LN_NORET void
parse_ice(const Str* fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  Str msg = str_format_v(*fmt, args);
  va_end(args);
  panic(make_str(con_col_ice "[ICE]" con_col_reset() "[PARSE]: %s"),
        str_cstr(&msg));
}

// -------------------------------------------------------------------------- //

/* Produce parser error at span with suggestion for solving */
void
parse_err(Parser* parser, const Span* span, const Str* expl, const Str* sugg)
{
  // Emit error
  ErrBuilder builder = make_err_builder(parser->src);
  err_builder_set_desc(&builder, expl);
  err_builder_set_msg(&builder, expl);
  err_builder_set_sugg(&builder, sugg);
  err_builder_set_span(&builder, span);
  err_builder_set_lines_after(&builder, 1);
  err_builder_set_pad_lines_before(&builder, 1);
  err_builder_set_pad_lines_after(&builder, 1);
  err_builder_set_err_num(&builder, kErrNumUnexpTok);
  err_builder_emit(&builder);

  // Store error
}

// ========================================================================== //
// Prog
// ========================================================================== //

static Ast*
parse_prog(Parser* parser)
{
  Ast* ast_prog = make_ast_prog();
  assrt(ast_prog != NULL, make_str("Failed to allocate program node"));

  const Tok* tok;
  while ((tok = tok_iter_peek(&parser->iter)) != NULL) {
    // Module
    if (tok_is_kw(tok, kTokKwModule)) {
      panic(make_str("Module declarations are not supported yet"));
    }
    // Import
    else if (tok_is_kw(tok, kTokKwImport)) {
      panic(make_str("Imports are not supported yet"));
    }
    // Type alias
    else if (tok_is_kw(tok, kTokKwType)) {
      panic(make_str("Type alias are not supported yet"));
    }
    // Function
    else if (tok_is_kw(tok, kTokKwFn)) {
      Ast* ast_fn = parse_fn(parser);
      ast_prog_add_fn(ast_prog, ast_fn);
    }
    // Enum
    else if (tok_is_kw(tok, kTokKwEnum)) {
      panic(make_str("Enums are not supported yet"));
    }
    // Struct
    else if (tok_is_kw(tok, kTokKwStruct)) {
      panic(make_str("Structs are not supported yet"));
    }
    // Trait
    else if (tok_is_kw(tok, kTokKwTrait)) {
      panic(make_str("Traits are not supported yet"));
    }
    // Unknown
    else {
      panic(make_str("Unexpected token found (%s)\n"), tok_kind_str(tok->kind));
    }
  }

  return ast_prog;
}

// ========================================================================== //
// Fn
// ========================================================================== //

static Ast*
parse_fn_ret(Parser* parser)
{
  // Check complete arrow
  LN_PARSE_TOK_ASSERT_NEXT_SYM("parse_fn_ret", kTokSymSub);
  Span span_beg = parser_span_cur(parser);
  parser_next(parser, false);

  if (!parser_expect_sym(parser, kTokSymGreater, false)) {
    parse_err(
      parser,
      &span_beg,
      &make_str("Expected arrow '->' to signify return type of function"),
      &make_str("Add an arrow before the return type"));
  }
  parser_next(parser, false);

  // Parse type
  return parse_type(parser);
}

// -------------------------------------------------------------------------- //

static Ast*
parse_fn_param(Parser* parser)
{
  LN_UNUSED(parser);
  return NULL;
}

// -------------------------------------------------------------------------- //

static Ast*
parse_fn(Parser* parser)
{
  // 'fn' keyword
  LN_PARSE_TOK_ASSERT_NEXT_KW("parse_fn", kTokKwFn);
  Span beg = parser_span_cur(parser);
  parser_next(parser, false);

  // Name
  if (!parser_expect(parser, kTokIdent, true)) {
    Span span = parser_span_cur(parser);
    parse_err(
      parser,
      &span,
      &make_str("Function name is expected after 'fn' keyword"),
      &make_str(
        "Make sure that the name of the function is a valid identifier"));
  }
  const Tok* tok = parser_next(parser, false);
  StrSlice name_slice = span_slice(&tok->span, &parser->src->src);
  Ast* ast = make_ast_fn(name_slice);

  // Expect '('
  tok = parser_peek(parser);
  if (!parser_expect_sym(parser, kTokSymLeftParen, true)) {
    parse_err(
      parser,
      &tok->span,
      &make_str(
        "Expected left parenthesis '(' at the start of the parameter list"),
      &make_str("Add a parenthesis to start the parameter list. Functions "
                "without arguments have empty parameter lists '()'"));
  }
  parser_next(parser, false);

  // Param list
  if (!parser_expect_sym(parser, kTokSymRightParen, true)) {
    do {
      Ast* ast_param = parse_fn_param(parser);
      if (ast_param) {
        ast_fn_add_param(ast, ast_param);
      }
    } while (!parser_expect_sym(parser, kTokSymComma, true));
  }

  // Expect ')'
  if (!parser_expect_sym(parser, kTokSymRightParen, true)) {
    tok = parser_peek(parser);
    parse_err(
      parser,
      &tok->span,
      &make_str(
        "Expected right parenthesis ')' at the end of the parameter list"),
      &make_str("Add a parenthesis to end the parameter list"));
  }
  parser_next(parser, false);

  // Ret
  if (parser_expect_sym(parser, kTokSymSub, true)) {
    Ast* ast_ret = parse_fn_ret(parser);
    if (ast_ret) {
      ast_fn_set_ret(ast, ast_ret);
    }
  }

  // Body
  if (!parser_expect_sym(parser, kTokSymLeftBrace, true)) {
    tok = parser_peek(parser);
    parse_err(parser,
              &tok->span,
              &make_str("Functions must be defined"),
              &make_str("Functions cannot be just declared without a body, but "
                        "must instead be defined at the same time. Add a body "
                        "to the function to solve this problem"));
  }
  Ast* ast_block = parse_block(parser);
  ast->fn.body = ast_block;

  // Done
  Span end = parser_span_cur(parser);
  ast->span = span_join(&beg, &end);
  return ast;
}

// ========================================================================== //
// Block
// ========================================================================== //

static Ast*
parse_block(Parser* parser)
{
  Ast* ast_block = make_ast_block();

  // Past '{'
  Span span_beg = parser_span_cur(parser);
  LN_PARSE_TOK_ASSERT_NEXT_SYM("parse_block", kTokSymLeftBrace);
  parser_next(parser, false);

  // Statements
  while (!parser_expect_sym(parser, kTokSymRightBrace, true) &&
         parser_peek(parser) != NULL) {
    Span span_stmt_before = parser_span_cur(parser);
    Ast* ast_stmt = parse_stmt(parser);
    if (ast_stmt) {
      ast_block_add_stmt(ast_block, ast_stmt);
    }

    // TODO(Filip Björklund): Handle invalid stmt
    Span span_stmt_after = parser_span_cur(parser);
    if (span_eq(&span_stmt_before, &span_stmt_after)) {
      panic(make_str("TMP: NO PROGRESS MADE"));
    }
  }

  // Expect '{'
  if (!parser_expect_sym(parser, kTokSymRightBrace, true)) {
  }

  Span span_end = parser_span_cur(parser);
  Span span = span_join(&span_beg, &span_end);
  LN_UNUSED(span);
  return NULL;
}

// ========================================================================== //
// Stmt
// ========================================================================== //

static Ast*
parse_stmt_let(Parser* parser)
{
  LN_PARSE_TOK_ASSERT_NEXT_KW("parse_stmt_let", kTokKwLet);

  LN_UNUSED(parser);
  return NULL;
}

// -------------------------------------------------------------------------- //

// static Ast* parse_stmt_ret(Parser* parser);

// -------------------------------------------------------------------------- //

static Ast*
parse_stmt(Parser* parser)
{
  const Tok* tok = parser_peek(parser);

  if (str_slice_eq_str(&tok->value, &make_str("let"))) {
    return parse_stmt_let(parser);
  }
  return NULL;
}

// ========================================================================== //
// Expr
// ========================================================================== //

// ========================================================================== //
// Type
// ========================================================================== //

static Type*
parse_type_array(Parser* parser)
{
  assrt(parser_expect_sym(parser, kTokSymLeftBracket, false), make_str(""));

  return NULL;
}

// -------------------------------------------------------------------------- //

static Type*
parse_type_aux(Parser* parser)
{
  // Array
  Type* type = NULL;
  if (parser_expect_sym(parser, kTokSymLeftBracket, false)) {
    type = parse_type_array(parser);
  } else { // Basic
    const Tok* tok = parser_peek(parser);
    type = get_type_from_name(&tok->value);
    parser_next(parser, false);
  }

  // Could not parse type
  if (!type) {
    return NULL;
  }

  // Add pointers
  while (parser_expect_sym(parser, kTokSymMul, true)) {
    parser_next(parser, false);
    type = get_type_ptr(type);
  }

  return get_type_void();
}

// -------------------------------------------------------------------------- //

static Ast*
parse_type(Parser* parser)
{
  parser_consume_whitespace(parser);
  Span span_beg = parser_span_cur(parser);
  Type* type = parse_type_aux(parser);
  if (!type) {
    panic(make_str("TEMP"));
  }
  Span span_end = parser_span_cur(parser);
  Ast* ast_type = make_ast_type(type);
  ast_type->span = span_join(&span_beg, &span_end);
  return ast_type;
}

// ========================================================================== //
// Parser
// ========================================================================== //

Parser
make_parser(const Src* src, const TokList* toks)
{
  return (Parser){ .src = src, .iter = make_tok_iter(toks) };
}

// -------------------------------------------------------------------------- //

void
release_parser(Parser* parser)
{
  LN_UNUSED(parser);
}

// -------------------------------------------------------------------------- //

const Tok*
parser_next(Parser* parser, bool skip_leading_whitespace)
{
  if (skip_leading_whitespace) {
    const Tok* tok;
    while ((tok = parser_peek(parser)) != NULL) {
      if (tok->kind != kTokWhitespace) {
        return parser_next(parser, false);
      }
      parser_next(parser, false);
    }
    return NULL;
  } else {
    return tok_iter_next(&parser->iter);
  }
}

// -------------------------------------------------------------------------- //

void
parser_consume_whitespace(Parser* parser)
{
  const Tok* tok;
  while ((tok = parser_peek(parser)) != NULL) {
    if (tok->kind != kTokWhitespace) {
      return;
    }
    parser_next(parser, false);
  }
}

// -------------------------------------------------------------------------- //

const Tok*
parser_peek(const Parser* parser)
{
  return tok_iter_peek(&parser->iter);
}

// -------------------------------------------------------------------------- //

Span
parser_span_cur(const Parser* parser)
{
  const Tok* tok = parser_peek(parser);
  return tok->span;
}

// -------------------------------------------------------------------------- //

Ast*
parser_parse(Parser* parser)
{
  return parse_prog(parser);
}