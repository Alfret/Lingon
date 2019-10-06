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

// ========================================================================== //
// ArgParse
// ========================================================================== //

typedef struct ArgParse
{
  /* Output file name */
  Str output;
  Str input; // Only temporary
  /* Show help */
  bool help;
  /* Verbose output */
  bool verbose;
  /* Debug: Dump tokens */
  bool dbg_dump_tokens;
  /* Debug: Dump ast */
  bool dbg_dump_ast;
} ArgParse;

// -------------------------------------------------------------------------- //

ArgParse
make_arg_parse(int argc, char** argv)
{
  ArgParse args = {};
  for (int i = 1; i < argc; i++) {
    if (cstr_eq(argv[i], "--help") || cstr_eq(argv[i], "-h")) {
      args.help = true;
    } else if (cstr_eq(argv[i], "--output") || cstr_eq(argv[i], "-o")) {
      if (argc < i + 2) {
        printf(
          "Missing arguments to '%s'. Please specify an output file path\n",
          argv[i]);
        quick_exit(-1);
      }
      args.output = make_str(argv[++i]);
    } else if (cstr_eq(argv[i], "--verbose") || cstr_eq(argv[i], "-v")) {
      args.verbose = true;
    } else if (cstr_eq(argv[i], "--dbg-dump-tok")) {
      args.dbg_dump_tokens = true;
    } else if (cstr_eq(argv[i], "--dbg-dump-ast")) {
      args.dbg_dump_ast = true;
    } else {
      args.input = make_str(argv[i]);
    }
  }
  return args;
}

// -------------------------------------------------------------------------- //

void
release_arg_parse(ArgParse* args)
{
  release_str(&args->output);
  release_str(&args->input);
}

// ========================================================================== //
// Main
// ========================================================================== //

void
print_help()
{
  printf("--help, -h     | Print this help message\n"
         "--output, -o   | Specify the output file\n"
         "--verbose, -v  | Verbose output\n"
         "--dbg-dump-tok | Dump the tokens after lexical analysis\n"
         "--dbg-dump-ast | Dump ast after syntax analysis\n");
}

// -------------------------------------------------------------------------- //

int
main(int argc, char** argv)
{
  // Init
  if (argc < 2) {
    printf("Too few arguments to 'lnc'\n");
    print_help();
    return 0;
  }
  ArgParse args = make_arg_parse(argc, argv);
  if (args.help) {
    print_help();
    exit(0);
  }

  // Lexical analysis
  Str src;
  FileErr file_err = read_file_str(&args.input, &src);
  if (file_err != kFileNoErr) {
    printf("Failed to open file '%s'\n", str_cstr(&args.input));
    exit(-1);
  }

  TokList tokens;
  LexErr err = tok_list_lex(&src, &tokens);
  if (err != kLexNoErr) {
    printf("Lexical analysis failed\n");
    return -1;
  }

  if (args.dbg_dump_tokens) {
    tok_list_dump(&tokens);
  }

  // Syntax analysis
  Parser parser = make_parser(&tokens);
  Ast ast = parser_parse(&parser);
  if (args.dbg_dump_ast) {
    LN_UNUSED(ast);
  }

  // Cleanup
  release_tok_list(&tokens);
  release_str(&src);
  release_arg_parse(&args);
  LN_CHECK_LEAK();
  return 0;
}