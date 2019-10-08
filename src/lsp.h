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

#ifndef LN_LSP_H
#define LN_LSP_H

#include <chif_net.h>

#include "common.h"
#include "str.h"

// ========================================================================== //
// LspErr
// ========================================================================== //

/* Lsp errors */
typedef enum LspErr
{
  /* No error */
  kLspNoErr = 0,
  /* Other error */
  kLspErrOther,
  /* Memory error */
  kLspMemErr,
  /* Failed to connect */
  kLspConnFailed,
  /* Connection lost */
  kLspConnLost,
  /* Failed to read */
  kLspRecvFail
} LspErr;

// -------------------------------------------------------------------------- //

Str
lsp_err_str(LspErr err);

// ========================================================================== //
// Lsp
// ========================================================================== //

/* Lsp */
typedef struct Lsp
{
  /* Socket */
  chif_net_socket sock;
  /* Data for split packets */
  struct
  {
    /* Content-Length */
    u32 size;
  } header;
} Lsp;

// -------------------------------------------------------------------------- //

Lsp
make_lsp(void);

// -------------------------------------------------------------------------- //

void
release_lsp(Lsp* lsp);

// -------------------------------------------------------------------------- //

LspErr
lsp_connect(Lsp* lsp, Str type, Str host, Str port);

// -------------------------------------------------------------------------- //

void
lsp_disconnect(Lsp* lsp);

// -------------------------------------------------------------------------- //

LspErr
lsp_run(Lsp* lsp);

#endif // LN_LSP_H