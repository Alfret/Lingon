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
    assrt(parser_accept_sym(parser, tok_name, false),                          \
          make_str("'" fn_name "' must only be called when next token is "     \
                   "'" #tok_name "'"));                                        \
  } while (0)

// -------------------------------------------------------------------------- //

#define LN_PARSE_TOK_ASSERT_NEXT_KW(fn_name, tok_name)                         \
  do {                                                                         \
    assrt(parser_accept_kw(parser, tok_name, false),                           \
          make_str("'" fn_name "' must only be called when next token is "     \
                   "'" #tok_name "'"));                                        \
  } while (0)

// -------------------------------------------------------------------------- //

#define LN_PARSE_TOK_ASSERT_NEXT(fn_name, tok_name)                            \
  do {                                                                         \
    assrt(parser_accept(parser, tok_name, false),                              \
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
parse_expr(Parser* parser);

static Ast*
parse_type(Parser* parser);

// ========================================================================== //
// Util
// ========================================================================== //

bool
parser_accept(Parser* parser, TokKind kind, bool skip_ws)
{
  if (skip_ws) {
    parser_consume_whitespace(parser);
  }
  const Tok* tok = parser_peek(parser);
  return tok->kind == kind;
}

// -------------------------------------------------------------------------- //

bool
parser_accept_kw(Parser* parser, TokKwKind kw_kind, bool skip_whitespace)
{
  if (parser_accept(parser, kTokKeyword, skip_whitespace)) {
    const Tok* tok = parser_peek(parser);
    return tok->data.kw_kind == kw_kind;
  }
  return false;
}

// -------------------------------------------------------------------------- //

bool
parser_accept_sym(Parser* parser, TokSymKind sym_kind, bool skip_whitespace)
{
  if (parser_accept(parser, kTokSym, skip_whitespace)) {
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
  panic(make_str(con_col_ice "[ICE]" con_col_reset "[PARSE]: %s"),
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
      if (ast_fn) {
        ast_prog_add_fn(ast_prog, ast_fn);
      }
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
    // Whitespace
    else if (tok->kind == kTokWhitespace) {
      parser_consume_whitespace(parser);
    }
    // Unknown
    else {
      Span span_cur = parser_span_cur(parser);
      parse_err(
        parser,
        &span_cur,
        &make_str("Unexpected token at top-level scope"),
        &make_str(
          "Only 'module', 'import', 'fn', 'enum', 'struct', 'trait' and 'type' "
          "constructs are allowed to reside in the top-level program scope"));
      parser_next(parser, false);
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

  if (!parser_accept_sym(parser, kTokSymGreater, false)) {
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
  if (!parser_accept(parser, kTokIdent, true)) {
    Span span = parser_span_cur(parser);
    parse_err(
      parser,
      &span,
      &make_str("Function name is expected after 'fn' keyword"),
      &make_str(
        "Make sure that the name of the function is a valid identifier"));
    return NULL;
  }
  const Tok* tok = parser_next(parser, false);
  StrSlice name_slice = span_slice(&tok->span, &parser->src->src);
  Ast* ast = make_ast_fn(name_slice);

  // Expect '('
  tok = parser_peek(parser);
  if (!parser_accept_sym(parser, kTokSymLeftParen, true)) {
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
  if (!parser_accept_sym(parser, kTokSymRightParen, true)) {
    do {
      Ast* ast_param = parse_fn_param(parser);
      if (ast_param) {
        ast_fn_add_param(ast, ast_param);
      }
    } while (!parser_accept_sym(parser, kTokSymComma, true));
  }

  // Expect ')'
  if (!parser_accept_sym(parser, kTokSymRightParen, true)) {
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
  if (parser_accept_sym(parser, kTokSymSub, true)) {
    Ast* ast_ret = parse_fn_ret(parser);
    if (ast_ret) {
      ast_fn_set_ret(ast, ast_ret);
    }
  }

  // Body
  if (!parser_accept_sym(parser, kTokSymLeftBrace, true)) {
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
  while (!parser_accept_sym(parser, kTokSymRightBrace, true) &&
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

  // Expect '}'
  if (!parser_accept_sym(parser, kTokSymRightBrace, true)) {
    Span span_cur = parser_span_cur(parser);
    parse_err(parser,
              &span_cur,
              &make_str("Expected a right brace at the end of a block"),
              &make_str("Blocks are terminated with right brace to balance the "
                        "left brace that starts it"));
  }
  parser_next(parser, false);

  // End
  Span span_end = parser_span_cur(parser);
  ast_block->span = span_join(&span_beg, &span_end);
  return ast_block;
}

// ========================================================================== //
// Stmt
// ========================================================================== //

static Ast*
parse_stmt_let(Parser* parser)
{
  LN_PARSE_TOK_ASSERT_NEXT_KW("parse_stmt_let", kTokKwLet);

  Ast* ast_let = make_ast_let();

  // 'let'
  Span span_beg = parser_span_cur(parser);
  parser_next(parser, false);

  // Name
  if (!parser_accept(parser, kTokIdent, true)) {
    const Tok* tok = parser_peek(parser);
    parse_err(parser,
              &tok->span,
              &make_str("Expected identifier for let statement"),
              &make_str("Name the variable"));
  }
  const Tok* tok = parser_next(parser, false);
  ast_let_set_name(ast_let, tok->value);

  // Optional type ': <type>'
  if (parser_accept_sym(parser, kTokSymColon, true)) {
    parser_next(parser, false);

    Ast* ast_type = parse_type(parser);
    ast_let_set_type(ast_let, ast_type);
  }

  // Any assigned value?
  if (parser_accept_sym(parser, kTokSymSemicolon, true)) {
    // ';'
    parser_next(parser, false);
  } else {
    // '='
    if (!parser_accept_sym(parser, kTokSymEqual, true)) {
      tok = parser_peek(parser);
      parse_err(
        parser,
        &tok->span,
        &make_str("Expected assignment operator in let-statement"),
        &make_str("If the variable is not supposed to have a default value "
                  "then end the statement with a semicolon instead"));
    }
    parser_next(parser, false);

    // Expr
    Ast* ast_expr = parse_expr(parser);
    ast_let_set_assigned(ast_let, ast_expr);
  }

  Span span_end = parser_span_cur(parser);
  ast_let->span = span_join(&span_beg, &span_end);
  return ast_let;
}

// -------------------------------------------------------------------------- //

static Ast*
parse_stmt_ret(Parser* parser)
{
  // Past 'ret'
  LN_PARSE_TOK_ASSERT_NEXT_KW("parse_stmt_ret", kTokKwRet);
  Span span_beg = parser_span_cur(parser);
  parser_next(parser, false);

  // Expr
  Ast* ast_expr = parse_expr(parser);
  Ast* ast_ret = make_ast_ret(ast_expr);

  // ';'
  if (!parser_accept_sym(parser, kTokSymSemicolon, true)) {
    Span span_cur = parser_span_cur(parser);
    parse_err(parser,
              &span_cur,
              &make_str("Expected semicolon at the end of a return statement"),
              &make_str("Return statements are not expressions and must "
                        "therefore be succeeded by a semicolon"));
  }
  parser_next(parser, false);
  Span span_end = parser_span_cur(parser);
  ast_ret->span = span_join(&span_beg, &span_end);
  return ast_ret;
}

// -------------------------------------------------------------------------- //

static Ast*
parse_stmt(Parser* parser)
{
  parser_consume_whitespace(parser);

  // Stmt kinds
  const Tok* tok = parser_peek(parser);
  if (tok_is_kw(tok, kTokKwLet)) {
    return parse_stmt_let(parser);
  } else if (tok_is_kw(tok, kTokKwRet)) {
    return parse_stmt_ret(parser);
  } else {
    Ast* ast_expr = parse_expr(parser);
    LN_UNUSED(ast_expr);
    LN_UNREACHABLE();
  }
}

// ========================================================================== //
// Expr
// ========================================================================== //

static Ast*
parse_expr_paren(Parser* parser)
{
  LN_UNUSED(parser);
  return NULL;
}

// -------------------------------------------------------------------------- //

static Ast*
parse_expr_var(Parser* parser)
{
  LN_UNUSED(parser);
  return NULL;
}

// -------------------------------------------------------------------------- //

static Ast*
parse_expr_const(Parser* parser)
{
  // Precondition
  const Tok* tok = parser_peek(parser);
  Span span_beg = tok->span;
  if (tok->kind != kTokInt && tok->kind != kTokFloat && tok->kind != kTokStr) {
    panic(make_str("'parse_expr_const' can only be called when next token is "
                   "'kTokInt', 'kTokFloat' or 'kTokStr'"));
  }

  // Check type
  AstConstKind kind;
  if (tok->kind == kTokInt) {
    kind = kAstConstInt;
  } else if (tok->kind == kTokFloat) {
    kind = kAstConstFloat;
  } else if (tok->kind == kTokFloat) {
    kind = kAstConstStr;
  } else {
    LN_UNREACHABLE();
  }

  // Make const
  parser_next(parser, false);
  Span span_end = parser_span_cur(parser);
  Ast* ast = make_ast_const(kind, tok->value);
  ast->span = span_join(&span_beg, &span_end);
  return ast;
}

// -------------------------------------------------------------------------- //

static Ast*
parse_expr_bottom(Parser* parser)
{
  parser_consume_whitespace(parser);

  const Tok* tok = parser_peek(parser);
  if (tok->kind == kTokInt || tok->kind == kTokFloat || tok->kind == kTokStr) {
    return parse_expr_const(parser);
  } else if (tok->kind == kTokIdent) {
    return parse_expr_var(parser);
  } else if (tok_is_sym(tok, kTokSymLeftParen)) {
    return parse_expr_paren(parser);
  } else {
    parse_err(
      parser,
      &tok->span,
      &make_str("expected identifier, literal or parenthesized expression"),
      &make_str("TMP"));
  }

  return NULL;
}

// -------------------------------------------------------------------------- //

static Ast*
parse_expr_scope_op(Parser* parser)
{
  // TODO(Filip Björklund): Implement
  return parse_expr_bottom(parser);
}

// -------------------------------------------------------------------------- //

static Ast*
parse_expr_postfix(Parser* parser)
{
  // TODO(Filip Björklund): Implement

  // Parse expr before postfix
  Ast* ast = parse_expr_scope_op(parser);

  // Parse prefix

  return ast;
}

// -------------------------------------------------------------------------- //

static Ast*
parse_expr_prefix(Parser* parser)
{
  parser_consume_whitespace(parser);
  const Tok* tok = parser_peek(parser);

  // Any prefix?
  Ast* ast_prefix = NULL;
  if (tok_is_sym(tok, kTokSymAdd)) {           // '+x' and '++x'
  } else if (tok_is_sym(tok, kTokSymSub)) {    // '-x' and '--x'
  } else if (tok_is_sym(tok, kTokSymExcl)) {   // '!x'
  } else if (tok_is_sym(tok, kTokSymInvert)) { // '~x'
  } else if (tok_is_sym(tok, kTokSymMul)) {    // '*x'
  } else if (tok_is_sym(tok, kTokSymAnd)) {    // '&x'
  }

  // Parse expr after prefix
  Ast* ast = parse_expr_postfix(parser);
  if (ast_prefix != NULL) {
    // // Store ast in prefix...
    // ast_prefix_set_expr(ast_prefix, ast);
    // ast = ast_prefix;
  }
  return ast;
}

// -------------------------------------------------------------------------- //

static Ast*
parse_expr_factor(Parser* parser)
{
  // Parse 'lhs'
  Ast* ast_lhs = parse_expr_prefix(parser);
  if (!ast_lhs) {
    return NULL;
  }

  // Terms while '*' or '/'
  while (parser_accept_sym(parser, kTokSymMul, true) ||
         parser_accept_sym(parser, kTokSymDiv, true)) {
    const Tok* tok = parser_next(parser, false);

    // Parse 'rhs'
    Ast* ast_rhs = parse_expr_prefix(parser);
    if (!ast_rhs) {
      release_ast(ast_lhs);
      return NULL;
    }

    // Create binop
    if (tok_is_sym(tok, kTokSymMul)) {
      Ast* ast_binop = make_ast_binop(kAstBinopMul);
      ast_binop_set_lhs(ast_binop, ast_lhs);
      ast_binop_set_rhs(ast_binop, ast_rhs);
      ast_lhs = ast_binop;
    } else if (tok_is_sym(tok, kTokSymDiv)) {
      Ast* ast_binop = make_ast_binop(kAstBinopDiv);
      ast_binop_set_lhs(ast_binop, ast_lhs);
      ast_binop_set_rhs(ast_binop, ast_rhs);
      ast_lhs = ast_binop;
    } else {
      panic(make_str("Unreachable"));
    }
  }

  return ast_lhs;
}

// -------------------------------------------------------------------------- //

static Ast*
parse_expr_term(Parser* parser)
{
  // Parse 'lhs'
  Ast* ast_lhs = parse_expr_factor(parser);
  if (!ast_lhs) {
    return NULL;
  }

  // Terms while '+' or '-'
  while (parser_accept_sym(parser, kTokSymAdd, true) ||
         parser_accept_sym(parser, kTokSymSub, true)) {
    const Tok* tok = parser_next(parser, false);

    // Parse 'rhs'
    Ast* ast_rhs = parse_expr_factor(parser);
    if (!ast_rhs) {
      release_ast(ast_lhs);
      return NULL;
    }

    // Create binop
    if (tok_is_sym(tok, kTokSymAdd)) {
      Ast* ast_binop = make_ast_binop(kAstBinopAdd);
      ast_binop_set_lhs(ast_binop, ast_lhs);
      ast_binop_set_rhs(ast_binop, ast_rhs);
      ast_lhs = ast_binop;
    } else if (tok_is_sym(tok, kTokSymSub)) {
      Ast* ast_binop = make_ast_binop(kAstBinopSub);
      ast_binop_set_lhs(ast_binop, ast_lhs);
      ast_binop_set_rhs(ast_binop, ast_rhs);
      ast_lhs = ast_binop;
    } else {
      panic(make_str("Unreachable"));
    }
  }

  return ast_lhs;
}

// -------------------------------------------------------------------------- //

static Ast*
parse_expr_match(Parser* parser)
{
  panic(make_str("'parse_expr_match' is not implemented"));
  LN_UNUSED(parser);
  return NULL;
}

// -------------------------------------------------------------------------- //

static Ast*
parse_expr_if(Parser* parser)
{
  panic(make_str("'parse_expr_if' is not implemented"));
  LN_UNUSED(parser);
  return NULL;
}

// -------------------------------------------------------------------------- //

static Ast*
parse_expr(Parser* parser)
{
  parser_consume_whitespace(parser);
  const Tok* tok = parser_peek(parser);

  // Match expr type
  if (str_slice_eq_str(&tok->value, &make_str("if"))) {
    return parse_expr_if(parser);
  } else if (str_slice_eq_str(&tok->value, &make_str("match"))) {
    return parse_expr_match(parser);
  } else if (tok_is_sym(tok, kTokSymLeftBrace)) {
    return parse_block(parser);
  } else {
    return parse_expr_term(parser);
  }
}

// ========================================================================== //
// Type
// ========================================================================== //

static Type*
parse_type_aux(Parser* parser);

// -------------------------------------------------------------------------- //

static Type*
parse_type_array(Parser* parser)
{
  // '['
  LN_PARSE_TOK_ASSERT_NEXT_SYM("parse_type_array", kTokSymLeftBracket);
  parser_next(parser, false);

  // Elem type
  Type* elem_type = parse_type_aux(parser);

  // ';'
  u64 len = kTypeArrayUnknownLen;
  if (parser_accept_sym(parser, kTokSymSemicolon, true)) {
    parser_next(parser, false);

    // Len must be integer
    if (!parser_accept(parser, kTokInt, true)) {
      return NULL;
    }

    // Parse len constant
    Ast* ast_num = parse_expr_const(parser);
    assrt(ast_num && ast_num->kind == kAstConst &&
            ast_num->constant.kind == kAstConstInt,
          make_str("Failed to parse array length"));

    len = ast_const_to_u64(ast_num);

    // Throw away constant
    release_ast(ast_num);
  }

  // ']'
  if (!parser_accept_sym(parser, kTokSymRightBracket, true)) {
    Span span_cur = parser_span_cur(parser);
    parse_err(parser,
              &span_cur,
              &make_str("Expected right bracket to end array type"),
              &make_str("Array types are enclosed in a matching '[' and ']' "
                        "pair. Make sure both are present"));
  }
  parser_next(parser, false);

  // Type
  return get_type_array(elem_type, len);
}

// -------------------------------------------------------------------------- //

static Type*
parse_type_aux(Parser* parser)
{
  // Array
  Type* type = NULL;
  if (parser_accept_sym(parser, kTokSymLeftBracket, false)) {
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
  while (parser_accept_sym(parser, kTokSymMul, true)) {
    parser_next(parser, false);
    type = get_type_ptr(type);
  }

  return type;
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
  if (!tok) { // Special case for last token
    const Tok* last_tok = tok_list_last(parser->iter.list);
    return make_span(last_tok->span.end, last_tok->span.end);
  }
  return tok->span;
}

// -------------------------------------------------------------------------- //

Ast*
parser_parse(Parser* parser)
{
  return parse_prog(parser);
}