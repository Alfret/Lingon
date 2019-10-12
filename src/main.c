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

#include "common.h"
#include "str.h"
#include "lex.h"
#include "file.h"
#include "parser.h"
#include "lsp.h"
#include "type.h"
#include "args.h"
#include "con.h"
#include "src.h"
#include "target.h"
#include "llvm_util.h"

// ========================================================================== //
// Main
// ========================================================================== //

void
print_help()
{
  printf(
    "--help, -h                 | Print this help message\n"
    "--output, -o <path>        | Specify the output file\n"
    "--target, -t <arch>        | Specify target architecture for\n"
    "                           | compilation. Only specify this if you are\n"
    "                           | doing cross-compilation\n"
    "--verbose, -v              | Verbose output\n"
    "--lsp <type> <host> <port> | Start the compiler in LSP server mode. This\n"
    "                           | will let the compiler start serving request\n"
    "                           | from an LSP client\n"
    "--dbg-dump-tok             | Dump the tokens after lexical analysis\n"
    "--dbg-dump-ast             | Dump ast after syntax analysis\n"
    "--dbg-dump-ir              | Dump IR after conversion to first stage IR,\n"
    "                           | 'MIR' (Mid-level IR).\n"
    "--dbg-dump-ll              | Dump LLVM IR after conversion from the\n"
    "\n");
}

// -------------------------------------------------------------------------- //

int
main_lsp(const Args* args)
{
  Lsp lsp = make_lsp();
  lsp_connect(
    &lsp, args->lsp_data.type, args->lsp_data.host, args->lsp_data.port);
  LspErr err = lsp_run(&lsp);
  Str err_str = lsp_err_str(err);
  if (err != kLspNoErr) {
    printf("LSP error (%s)\n", str_cstr(&err_str));
    return -1;
  }
  lsp_disconnect(&lsp);
  return 0;
}

// -------------------------------------------------------------------------- //

void
main_init()
{
  llvm_init();
  types_init();
}

// -------------------------------------------------------------------------- //

void
main_cleanup()
{
  types_cleanup();
  llvm_cleanup();
  LN_CHECK_LEAK();
}

// -------------------------------------------------------------------------- //

int
main_compile_files(const Args* args)
{
  // Compile each file
  for (u32 i = 0; i < args->input.len; i++) {
    const Str* in = str_list_get(&args->input, i);
    printf(con_col256(105) "Compiling:" con_col_reset " %s\n", str_cstr(in));

    // Create target machine
    Target target;
    TargetErr target_err = make_target(&args->target, &target);
    if (target_err != kTargetNoErr) {
      printf("Fatal: Failed to create target machine\n");
      exit(-1);
    }

    // Load source
    Src src;
    SrcErr src_err = make_src(in, &src);
    if (src_err != kSrcNoErr) {
      printf("Fatal: Failed to create source '%s'\n", str_cstr(in));
      exit(-1);
    }

    // Lexical analysis
    TokList tokens;
    LexErr lex_err = tok_list_lex(&src, &tokens);
    if (lex_err != kLexNoErr) {
      printf("Lexical analysis failed\n");
      return -1;
    }
    if (args->dbg_dump_tokens) {
      tok_list_dump(&tokens);
    }

    // Syntax analysis
    Parser parser = make_parser(&src, &tokens);
    Ast* ast = parser_parse(&parser);
    if (args->dbg_dump_ast) {
      LN_UNUSED(ast);
    }
    if (args->dbg_dump_ast) {
      ast_dump(ast);
    }

    // MIR gen

    // LLVM IR gen

    // Release
    release_ast(ast);
    release_tok_list(&tokens);
    release_src(&src);
  }

  return 0;
}

// -------------------------------------------------------------------------- //

int
main(int argc, char** argv)
{
  // Args
  if (argc < 2) {
    printf("Too few arguments to 'lnc'\n");
    print_help();
    return 0;
  }
  Args args;
  make_args(argc, argv, &args);
  if (args.help) {
    print_help();
    exit(0);
  }

  // Init
  main_init();

  // LSP
  if (args.lsp) {
    printf("Starting the compiler in LSP mode");
    int res = main_lsp(&args);
    release_args(&args);
    return res;
  }

  // Compile files
  main_compile_files(&args);

  // Cleanup
  release_args(&args);
  main_cleanup();
  return 0;
}