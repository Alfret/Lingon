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

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cJSON.h>

#include "lsp.h"

#define LN_LSP_PROP_ERR(e)                                                     \
  if (e != kLspNoErr) {                                                        \
    return e;                                                                  \
  }

static LspErr
lsp_sock_write(Lsp* lsp, u8* buf, u32 buf_size);

// ========================================================================== //
// LspErr
// ========================================================================== //

#define LN_LSP_ERR_STR_CASE(e)                                                 \
  case e: {                                                                    \
    return make_str(#e);                                                       \
  }

Str
lsp_err_str(LspErr err)
{
  switch (err) {
    LN_LSP_ERR_STR_CASE(kLspNoErr)
    LN_LSP_ERR_STR_CASE(kLspErrOther)
    LN_LSP_ERR_STR_CASE(kLspMemErr)
    LN_LSP_ERR_STR_CASE(kLspConnFailed)
    LN_LSP_ERR_STR_CASE(kLspConnLost)
    LN_LSP_ERR_STR_CASE(kLspRecvFail)
    default: {
      panic(make_str("Invalid LspErr value"));
    }
  }
}

#undef LN_LSP_ERR_STR_CASE

// -------------------------------------------------------------------------- //

LspErr
lsp_err_from_chif_result(chif_net_result result)
{
  switch (result) {
    case CHIF_NET_RESULT_UNKNOWN: {
      return kLspErrOther;
    }
    case CHIF_NET_RESULT_SUCCESS: {
      return kLspNoErr;
    }
    case CHIF_NET_RESULT_CONNECTION_REFUSED:
    case CHIF_NET_RESULT_ACCESS_DENIED:
    case CHIF_NET_RESULT_ALREADY_CONNECTED:
    case CHIF_NET_RESULT_NOT_LISTENING_OR_NOT_CONNECTED: {
      return kLspConnFailed;
    }
    case CHIF_NET_RESULT_CONNECTION_ABORTED:
    case CHIF_NET_RESULT_CONNECTION_CLOSED: {
      return kLspConnLost;
    }
    default: {
      return kLspErrOther;
    }
  }
}

// ========================================================================== //
// JRPC
// ========================================================================== //

LspErr
jrpc_send_json(Lsp* lsp, const cJSON* json)
{
  // Generate json string
  char* json_str = cJSON_PrintUnformatted(json);

  // Format response
  Str msg = str_format(make_str("Content-Length: %u\r\n"
                                "\r\n"
                                "%s"),
                       cstr_size(json_str),
                       json_str);
  cJSON_free(json_str);

  // Debug
#if 0
  char* dbg_str = cJSON_Print(json);
  printf("Sending JSON:\n%s\n", dbg_str);
  cJSON_free(dbg_str);
#endif

  // Write response
  return lsp_sock_write(lsp, str_buf(&msg), msg.size);
}

// -------------------------------------------------------------------------- //

LspErr
jrpc_handle_init(Lsp* lsp, cJSON* json)
{
  // Determine id
  cJSON* req_id_json = cJSON_GetObjectItemCaseSensitive(json, "id");
  assrt(cJSON_IsNumber(req_id_json), make_str("request id must be a number"));
  const f64 req_id = req_id_json->valuedouble;

  // Build response
  cJSON* j_resp = cJSON_CreateObject();
  cJSON_AddStringToObject(j_resp, "jsonrpc", "2.0");
  cJSON_AddNumberToObject(j_resp, "id", req_id);

  cJSON* j_cap = cJSON_CreateObject();
  cJSON_AddNumberToObject(j_cap, "textDocumentSync", 1.0);
  cJSON* j_comp = cJSON_CreateObject();
  cJSON_AddBoolToObject(j_comp, "resolveProvider", false);
  cJSON* j_trig_char = cJSON_AddArrayToObject(j_comp, "triggerCharacters");
  cJSON* j_trig_char0 = cJSON_CreateString("/");
  cJSON_AddItemToArray(j_trig_char, j_trig_char0);
  cJSON_AddItemToObject(j_cap, "completionProvider", j_comp);

  cJSON* j_res = cJSON_CreateObject();
  cJSON_AddItemToObject(j_resp, "result", j_res);
  cJSON_AddBoolToObject(j_cap, "hoverProvider", true);
  cJSON_AddBoolToObject(j_cap, "documentSymbolProvider", false);
  cJSON_AddBoolToObject(j_cap, "referencesProvider", false);
  cJSON_AddBoolToObject(j_cap, "definitionProvider", false);
  cJSON_AddBoolToObject(j_cap, "documentHighlightProvider", false);
  cJSON_AddBoolToObject(j_cap, "codeActionProvider", false);
  cJSON_AddBoolToObject(j_cap, "renameProvider", false);
  cJSON_AddItemToObject(j_cap, "colorProvider", cJSON_CreateObject());
  cJSON_AddBoolToObject(j_cap, "foldingRangeProvider", false);
  cJSON_AddItemToObject(j_res, "capabilities", j_cap);

  // Send response
  LspErr err = jrpc_send_json(lsp, j_resp);
  cJSON_Delete(j_resp);
  return err;
}

// -------------------------------------------------------------------------- //

LspErr
jrpc_handle_hover(Lsp* lsp, cJSON* json)
{
  LN_UNUSED(json);

  // Hover message
  Str hover = make_str("No information available");

  // Determine id
  cJSON* req_id_json = cJSON_GetObjectItemCaseSensitive(json, "id");
  assrt(cJSON_IsNumber(req_id_json), make_str("request id must be a number"));
  const f64 req_id = req_id_json->valuedouble;

  // Build response
  cJSON* j_resp = cJSON_CreateObject();
  cJSON_AddStringToObject(j_resp, "jsonrpc", "2.0");
  cJSON_AddNumberToObject(j_resp, "id", req_id);
  cJSON* j_res = cJSON_CreateObject();
  cJSON_AddItemToObject(j_resp, "result", j_res);
  cJSON_AddStringToObject(j_res, "contents", str_cstr(&hover));

  // Send response
  LspErr err = jrpc_send_json(lsp, j_resp);
  cJSON_Delete(j_resp);
  return err;
}

// ========================================================================== //
// Net
// ========================================================================== //

#define s_recv_buf_size 32768

// -------------------------------------------------------------------------- //

static u8 s_recv_buf[s_recv_buf_size + 1];

// -------------------------------------------------------------------------- //

/* Get offset to right after '\r\n\r\n' */
u32
lsp_cont_off(u8* buf, u32 buf_size)
{
  u32 crlf_off = 0;
  for (u32 o = 0; o < buf_size - 3; o++) {
    if (buf[o + 0] == '\r' && buf[o + 1] == '\n' && buf[o + 2] == '\r' &&
        buf[o + 3] == '\n') {
      crlf_off = o + 4;
      break;
    }
  }
  assrt(
    crlf_off != 0,
    make_str("Could not find any occurance of '\\r\\n\\r\\n' in the buffer"));
  return crlf_off;
}

// -------------------------------------------------------------------------- //

u32
lsp_cont_len(u8* buf, u32 buf_size)
{
  u32 beg = 16, count = 0;
  for (u32 i = beg; i < buf_size; i++) {
    if (buf[i] == '\r') {
      break;
    }
    count++;
  }
  char str_buf[32];
  assrt(count < 32, make_str("Invalid 'Content-Length'"));
  str_buf[count] = 0;
  memcpy(str_buf, buf + beg, buf_size);
  return strtoul((char*)(buf + beg), NULL, 10);
}

// -------------------------------------------------------------------------- //

LspErr
lsp_handle_msg(Lsp* lsp, char* str_buf)
{
  // printf("START_MESSAGE:\n%s\nEND_MESSAGE\n\n", str_buf);

  // Determine method
  cJSON* json = cJSON_Parse(str_buf);
  cJSON* method = cJSON_GetObjectItem(json, "method");
  assrt(cJSON_IsString(method), make_str("method tag must be a string"));
  const Str method_str = make_str(cJSON_GetStringValue(method));

  // Delegate control
  if (str_eq(&method_str, &make_str("initialize"))) {
    jrpc_handle_init(lsp, json);
  } else if (str_eq(&method_str, &make_str("textDocument/hover"))) {
    jrpc_handle_hover(lsp, json);
  }

  return kLspNoErr;
}

// -------------------------------------------------------------------------- //

LspErr
lsp_sock_can_read(Lsp* lsp, bool* p_can_read)
{
  int can_read;
  chif_net_result result = chif_net_can_read(lsp->sock, &can_read, 100);
  if (result != CHIF_NET_RESULT_SUCCESS) {
    *p_can_read = false;
    return lsp_err_from_chif_result(result);
  }
  *p_can_read = (bool)can_read;
  return kLspNoErr;
}

// -------------------------------------------------------------------------- //

LspErr
lsp_sock_read(Lsp* lsp, u8* buf, u32 buf_size, u32* read)
{
  int _read = 0;
  chif_net_result result = chif_net_read(lsp->sock, buf, buf_size, &_read);
  if (result != CHIF_NET_RESULT_SUCCESS) {
    *read = 0;
    return lsp_err_from_chif_result(result);
  }
  *read = (u32)_read;
  return kLspNoErr;
}

// -------------------------------------------------------------------------- //

LspErr
lsp_sock_write(Lsp* lsp, u8* buf, u32 buf_size)
{
  int sent;
  chif_net_result result = chif_net_write(lsp->sock, buf, buf_size, &sent);
  if (result != CHIF_NET_RESULT_SUCCESS) {
    return kLspErrOther;
  }
  return kLspNoErr;
}

// -------------------------------------------------------------------------- //

LspErr
lsp_sock_avail(Lsp* lsp, u32* p_avail)
{
  unsigned long _avail = 0;
  chif_net_result result = chif_net_get_bytes_available(lsp->sock, &_avail);
  if (result != CHIF_NET_RESULT_SUCCESS) {
    *p_avail = 0;
    return lsp_err_from_chif_result(result);
  }
  *p_avail = _avail;
  return kLspNoErr;
}

// -------------------------------------------------------------------------- //

/* Read 'size' bytes from socket. Buffer is guaranteed to be large enough */
LspErr
lsp_sock_read_size(Lsp* lsp, u8* buf, u32 size)
{
  u32 off = 0;
  while (size > 0) {
    u32 read;
    LspErr err = lsp_sock_read(lsp, buf + off, size, &read);
    LN_LSP_PROP_ERR(err);
    off += read;
    size -= read;
  }

  return kLspNoErr;
}

// -------------------------------------------------------------------------- //

LspErr
lsp_recv(Lsp* lsp)
{
  // Do we have something to read?
  bool can_read;
  LspErr err = lsp_sock_can_read(lsp, &can_read);
  LN_LSP_PROP_ERR(err);
  if (!can_read) {
    return kLspNoErr;
  }

  // How much?
  u32 avail;
  err = lsp_sock_avail(lsp, &avail);
  LN_LSP_PROP_ERR(err);

  // Get a buffer
  bool heap_buf = false;
  u8* buf = s_recv_buf;
  u32 buf_size = s_recv_buf_size;
  if (avail > buf_size) {
    buf_size = avail;
    buf = (u8*)alloc(buf_size + 1, kLnMinAlign);
    heap_buf = true;
  }
  assrt(buf != NULL, make_str("Invalid read buffer"));

  // Read
  u32 read;
  err = lsp_sock_read(lsp, buf, avail, &read);
  LN_LSP_PROP_ERR(err);
  assrt(read == avail, make_str("Failed to read entire message from client"));

  // Check the content size
  u32 cont_len = lsp_cont_len(buf, avail);
  u32 cont_off = lsp_cont_off(buf, avail);
  u32 pack_size = cont_len + cont_off;
  u32 size_left = pack_size - avail;

  // Wait for entire message
  if (size_left > 0) {
    // Do we need to resize buffer again?
    if (buf_size < pack_size) {
      u32 _buf_size = pack_size;
      u8* _buf = (u8*)alloc(buf_size + 1, kLnMinAlign);
      memcpy(_buf, buf, buf_size);
      if (heap_buf) {
        release(buf);
      }
      buf_size = _buf_size;
      buf = _buf;
      heap_buf = true;
    }

    // Read entire packet
    lsp_sock_read_size(lsp, buf + avail, size_left);
  }

  // Send of the content
  buf[pack_size] = 0;
  lsp_handle_msg(lsp, (char*)(buf + cont_off));

  // Free buf
  if (heap_buf) {
    release(buf);
  }
  return kLspNoErr;
};

// ========================================================================== //
// Lsp
// ========================================================================== //

Lsp
make_lsp(void)
{
  setvbuf(stdout, NULL, _IONBF, 0);
  chif_net_startup();
  return (Lsp){ .sock = CHIF_NET_INVALID_SOCKET };
}

// -------------------------------------------------------------------------- //

void
release_lsp(Lsp* lsp)
{
  lsp_disconnect(lsp);
  chif_net_shutdown();
}

// -------------------------------------------------------------------------- //

LspErr
lsp_connect(Lsp* lsp, Str type, Str host, Str port)
{
  assrt(lsp->sock == CHIF_NET_INVALID_SOCKET,
        make_str("LSP is already connected"));
  assrt(str_eq(&type, &make_str("tcp")),
        make_str("Only tcp servers are implemented at the moment"));

  printf("LNC: Started LSP server. Type: '%s', Host: '%s', Port: '%s'\n",
         str_cstr(&type),
         str_cstr(&host),
         str_cstr(&port));

  // Create socket
  chif_net_startup();
  chif_net_address_family af = CHIF_NET_ADDRESS_FAMILY_IPV4;
  chif_net_transport_protocol tp = CHIF_NET_TRANSPORT_PROTOCOL_TCP;
  chif_net_result result = chif_net_open_socket(&lsp->sock, tp, af);
  if (result != CHIF_NET_RESULT_SUCCESS) {
    fprintf(stderr, "Failed to open socket\n");
    return kLspErrOther;
  }

  // Connect
  chif_net_address addr;
  result =
    chif_net_create_address(&addr, str_cstr(&host), str_cstr(&port), af, tp);
  if (result != CHIF_NET_RESULT_SUCCESS) {
    fprintf(stderr, "Failed to create address\n");
    return kLspErrOther;
  }
  result = chif_net_connect(lsp->sock, &addr);
  if (result != CHIF_NET_RESULT_SUCCESS) {
    fprintf(stderr, "Failed to connect to LSP client\n");
    return kLspConnFailed;
  }

  return kLspNoErr;
}

// -------------------------------------------------------------------------- //

void
lsp_disconnect(Lsp* lsp)
{
  if (lsp->sock == CHIF_NET_INVALID_SOCKET) {
    return;
  }
  chif_net_close_socket(&lsp->sock);
  lsp->sock = CHIF_NET_INVALID_SOCKET;
}

// -------------------------------------------------------------------------- //

LspErr
lsp_run(Lsp* lsp)
{
  while (1) {
    LspErr err = lsp_recv(lsp);
    if (err != kLspNoErr) {
      return err;
    }
  }

  return kLspNoErr;
}