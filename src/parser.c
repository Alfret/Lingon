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

static Ast
parse_prog(Parser* parser);

static Ast
parse_fn(Parser* parser);

// ========================================================================== //
// Util
// ========================================================================== //

bool
parse_expect_kind(Parser* parser, TokKind kind)
{
  const Tok* tok = parser_peek(parser);
  if (tok->kind != kind) {
    printf("TMP: Expected token ... found ...\n");
    return false;
  }
  return true;
}

// -------------------------------------------------------------------------- //

bool
parse_expect_kw_kind(Parser* parser, TokKwKind kw_kind)
{
  const Tok* tok = parser_peek(parser);
  if (tok->kind != kTokKeyword) {
    printf("TMP: Expected keyword found ...\n");
    return false;
  }
  if (tok->data.kw_kind != kw_kind) {
    printf("TMP: Expected keyword of type ... found keyword ...\n");
    return false;
  }
  return true;
}

// ========================================================================== //
// Prog
// ========================================================================== //

static Ast
parse_prog(Parser* parser)
{
  Ast ast_prog = make_ast_prog();

  const Tok* tok;
  while ((tok = tok_iter_peek(&parser->iter)) != NULL) {
    if (tok->kind == kTokKeyword && tok->data.kw_kind == kTokKwFn) {
      Ast ast_fn = parse_fn(parser);
      ast_prog_add_fn(&ast_prog, &ast_fn);
    } else {
      panic(&LN_CSTR("Unexpected token found (%s)\n"), tok_kind_str(tok->kind));
    }
  }

  return ast_prog;
}

// ========================================================================== //
// Fn
// ========================================================================== //

static Ast
parse_fn(Parser* parser)
{
  // 'fn' keyword
  if (!parse_expect_kw_kind(parser, kTokKwFn)) {
    panic(&LN_CSTR("ICE: Trying to parse function at wrong time"));
  }
  parser_next(parser);

  // Name
  if (!parse_expect_kind(parser, kTokIdent)) {
  }
  const Tok* tok = parser_next_skip_whitespace(parser);
  LN_UNUSED(tok);

  return make_ast_fn(make_str_slice(&LN_CSTR(""), 0, 0));
}

// ========================================================================== //
// Stmt
// ========================================================================== //

// ========================================================================== //
// Expr
// ========================================================================== //

// ========================================================================== //
// Parser
// ========================================================================== //

Parser
make_parser(const TokList* toks)
{
  Parser parser;
  parser.iter = make_tok_iter(toks);
  return parser;
}

// -------------------------------------------------------------------------- //

void
release_parser(Parser* parser)
{
  LN_UNUSED(parser);
}

// -------------------------------------------------------------------------- //

const Tok*
parser_next(Parser* parser)
{
  return tok_iter_next(&parser->iter);
}

// -------------------------------------------------------------------------- //

const Tok*
parser_next_skip_whitespace(Parser* parser)
{
  const Tok* tok;
  while ((tok = parser_peek(parser)) != NULL) {
    if (tok->kind != kTokWhitespace) {
      return parser_next(parser);
    }
    parser_next(parser);
  }
  return NULL;
}

// -------------------------------------------------------------------------- //

const Tok*
parser_peek(const Parser* parser)
{
  return tok_iter_peek(&parser->iter);
}

// -------------------------------------------------------------------------- //

RangePos
parser_pos_cur(const Parser* parser)
{
  const Tok* tok = parser_peek(parser);
  return tok->pos;
}

// -------------------------------------------------------------------------- //

Ast
parser_parse(Parser* parser)
{
  return parse_prog(parser);
}