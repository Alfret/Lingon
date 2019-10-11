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

#ifndef LN_PARSER_H
#define LN_PARSER_H

#include "lex.h"
#include "ast.h"
#include "src.h"

// ========================================================================== //
// Parser
// ========================================================================== //

/* Parser errors */
typedef enum ParseErr
{
  /* No error */
  kParseNoErr
} ParseErr;

// -------------------------------------------------------------------------- //

/* Parser */
typedef struct Parser
{
  /* Source */
  const Src* src;
  /* Token iterator */
  TokIter iter;
} Parser;

// -------------------------------------------------------------------------- //

Parser
make_parser(const Src* src, const TokList* toks);

// -------------------------------------------------------------------------- //

void
release_parser(Parser* parser);

// -------------------------------------------------------------------------- //

const Tok*
parser_next(Parser* parser, bool skip_leading_whitespace);

// -------------------------------------------------------------------------- //

/* Consume whitespace tokens */
void
parser_consume_whitespace(Parser* parser);

// -------------------------------------------------------------------------- //

const Tok*
parser_peek(const Parser* parser);

// -------------------------------------------------------------------------- //

Span
parser_span_cur(const Parser* parser);

// -------------------------------------------------------------------------- //

Ast*
parser_parse(Parser* parser);

#endif // LN_PARSER_H