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

#include "args.h"

// ========================================================================== //
// Args
// ========================================================================== //

void
make_args(int argc, char** argv, Args* p_args)
{
  *p_args = (Args){};
  Args args = {};
  args.input = make_str_list(4);

  for (int i = 1; i < argc; i++) {
    if (cstr_eq(argv[i], "--help") || cstr_eq(argv[i], "-h")) {
      args.help = true;
    } else if (cstr_eq(argv[i], "--output") || cstr_eq(argv[i], "-o")) {
      if (argc < i + 2) {
        printf(
          "Missing arguments to '%s'. Please specify an output file path\n",
          argv[i]);
        exit(-1);
      }
      args.output = make_str_copy(argv[++i]);
    } else if (cstr_eq(argv[i], "--verbose") || cstr_eq(argv[i], "-v")) {
      args.verbose = true;
    } else if (cstr_eq(argv[i], "--lsp")) {
      if (argc < i + 4) {
        printf("Missing arguments to '--lsp'. Please specify an 'type' (tcp | "
               "ipc), 'host' and 'port'\n");
        exit(-2);
      }
      args.lsp = true;
      if (argv[i + 1][0] == '-') {
        printf("Missing argument 'type' to '--lsp', please specify either "
               "'tcp' or 'ipc'\n");
        exit(-3);
      }
      args.lsp_data.type = make_str_copy(argv[++i]);
      if (argv[i + 1][0] == '-') {
        printf("Missing argument 'host' to '--lsp', please specify the host "
               "address\n");
        exit(-4);
      }
      args.lsp_data.host = make_str_copy(argv[++i]);
      if (argv[i + 1][0] == '-') {
        printf("Missing argument 'port' to '--lsp', please the port\n");
        exit(-5);
      }
      args.lsp_data.port = make_str_copy(argv[++i]);
    } else if (cstr_eq(argv[i], "--dbg-dump-tok")) {
      args.dbg_dump_tokens = true;
    } else if (cstr_eq(argv[i], "--dbg-dump-ast")) {
      args.dbg_dump_ast = true;
    } else {
      str_list_append(&args.input, &make_str(argv[i]));
    }
  }

  *p_args = args;
}

// -------------------------------------------------------------------------- //

void
release_args(Args* p_args)
{
  release_str(&p_args->lsp_data.port);
  release_str(&p_args->lsp_data.host);
  release_str(&p_args->lsp_data.type);
  release_str(&p_args->output);
  release_str_list(&p_args->input);
}
