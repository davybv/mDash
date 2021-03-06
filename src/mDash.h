// Copyright (c) 2019 Cesanta Software Limited
// All rights reserved

#pragma once

#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

#if ARDUINO
#define __TOSTR(x) #x
#define STR(x) __TOSTR(x)
#ifndef ARDUINO_BOARD
#define ARDUINO_BOARD "?"
#endif
#define MDASH_FRAMEWORK "a-" STR(ARDUINO) "-" ARDUINO_BOARD
#else
#define MDASH_FRAMEWORK "idf"
#endif

#ifndef MDASH_APP_NAME
#define MDASH_APP_NAME __FILE__
#endif

#define MDASH_BUILD_TIME __DATE__ "-" __TIME__

#define mDashBeginWithWifi(fn, a, b, d)                        \
  mDashInitWithWifi((fn), (a), (b), NULL, (d), MDASH_APP_NAME, \
                    MDASH_BUILD_TIME, MDASH_FRAMEWORK)

#define mDashBegin(b) \
  mDashInit(NULL, (b), MDASH_APP_NAME, MDASH_BUILD_TIME, MDASH_FRAMEWORK)

// mDash housekeeping
void mDashInitWithWifi(void (*fn)(const char *wifi_name, const char *wifi_pass),
                       const char *wifi_name, const char *wifi_pass,
                       const char *device_id, const char *device_pass,
                       const char *app_name, const char *build_time,
                       const char *framework);
void mDashInit(const char *device_id, const char *device_pass,
               const char *app_name, const char *build_time,
               const char *framework);

void mDashSetLogLevel(int logLevel);
void mDashSetURL(const char *);
int mDashGetState(void);  // current connection state, MDASH_EVENT_*
const char *mDashGetSdkVersion(void);
unsigned long mDashGetFreeRam(void);

// Events
enum {
  // Events defined by the mDash library
  MDASH_EVENT_NETWORK_LOST = 0,       // event_data: NULL
  MDASH_EVENT_NETWORK_AP = 1,         // event_data: NULL
  MDASH_EVENT_NETWORK_CONNECTED = 2,  // event_data: NULL
  MDASH_EVENT_CLOUD_CONNECTED = 3,    // event_data: NULL
  MDASH_EVENT_OTA_BEGIN = 4,          // event_data: opaque void *
  MDASH_EVENT_OTA_WRITE = 5,          // event_data: opaque void *
  MDASH_EVENT_OTA_END = 6,            // event_data: opaque void *

  // Events defined by user
  MDASH_EVENT_USER = 100,  // Starting number for user-based events
};
typedef void (*evh_t)(void *event_data, void *userdata);
void mDashRegisterEventHandler(int event, evh_t fn, void *userdata);
int mDashTriggerEvent(int event, void *event_data);

// Logging API
enum { LL_NONE, LL_CRIT, LL_INFO, LL_DEBUG };
#define MLOG(ll, fmt, ...) mlog(ll, __func__, (fmt), __VA_ARGS__)
void mlog(int ll, const char *fn, const char *fmt, ...);

// Data storage
int mDashStore(const char *topic, const char *json_fmt, ...);

// Provisioning API
void mDashCLI(unsigned char input_byte);

// Notification API
int mDashNotify(const char *name, const char *fmt, ...);
#define mDashShadowUpdate(fmt, ...) \
  mDashNotify("Dash.Shadow.Update", (fmt), __VA_ARGS__)

// Configuration API
int mDashConfigGet(const char *name, char *buf, int bufsize);
int mDashConfigSet(const char *name, const char *value);
int mDashConfigReset(void);
void mDashCLI(unsigned char input_byte);

// mjson API - see documentation at https://github.com/cesanta/mjson
#ifndef MJSON_H
#define MJSON_H

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef MJSON_ENABLE_PRINT
#define MJSON_ENABLE_PRINT 1
#endif

#ifndef MJSON_ENABLE_RPC
#define MJSON_ENABLE_RPC 1
#endif

#ifndef MJSON_ENABLE_BASE64
#define MJSON_ENABLE_BASE64 1
#endif

#ifndef MJSON_RPC_IN_BUF_SIZE
#define MJSON_RPC_IN_BUF_SIZE 256
#endif

#ifndef ATTR
#define ATTR
#endif

enum {
  MJSON_ERROR_INVALID_INPUT = -1,
  MJSON_ERROR_TOO_DEEP = -2,
};

enum mjson_tok {
  MJSON_TOK_INVALID = 0,
  MJSON_TOK_KEY = 1,
  MJSON_TOK_STRING = 11,
  MJSON_TOK_NUMBER = 12,
  MJSON_TOK_TRUE = 13,
  MJSON_TOK_FALSE = 14,
  MJSON_TOK_NULL = 15,
  MJSON_TOK_ARRAY = 91,
  MJSON_TOK_OBJECT = 123,
};
#define MJSON_TOK_IS_VALUE(t) ((t) > 10 && (t) < 20)

typedef void (*mjson_cb_t)(int ev, const char *s, int off, int len, void *ud);

#ifndef MJSON_MAX_DEPTH
#define MJSON_MAX_DEPTH 20
#endif

int mjson(const char *s, int len, mjson_cb_t cb, void *ud);
enum mjson_tok mjson_find(const char *s, int len, const char *jp,
                          const char **tokptr, int *toklen);
int mjson_get_number(const char *s, int len, const char *path, double *v);
int mjson_get_bool(const char *s, int len, const char *path, int *v);
int mjson_get_string(const char *s, int len, const char *path, char *to, int n);

#if MJSON_ENABLE_BASE64
int mjson_get_base64(const char *s, int len, const char *path, char *to, int n);
#endif

#if MJSON_ENABLE_PRINT
typedef int (*mjson_print_fn_t)(const char *buf, int len, void *userdata);
typedef int (*mjson_vprint_fn_t)(mjson_print_fn_t, void *, va_list *);

struct mjson_fixedbuf {
  char *ptr;
  int size, len;
};

int mjson_printf(mjson_print_fn_t, void *, const char *fmt, ...);
int mjson_vprintf(mjson_print_fn_t, void *, const char *fmt, va_list ap);
int mjson_print_str(mjson_print_fn_t, void *, const char *s, int len);
int mjson_print_int(mjson_print_fn_t, void *, int value, int is_signed);
int mjson_print_long(mjson_print_fn_t, void *, long value, int is_signed);

int mjson_print_file(const char *ptr, int len, void *userdata);
int mjson_print_fixed_buf(const char *ptr, int len, void *userdata);
int mjson_print_dynamic_buf(const char *ptr, int len, void *userdata);

#endif  // MJSON_ENABLE_PRINT

#if MJSON_ENABLE_RPC

void jsonrpc_init(void (*response_cb)(const char *, int, void *),
                  void *userdata);

struct jsonrpc_request {
  const char *params;   // Points to the "params" in the request frame
  int params_len;       // Length of the "params"
  const char *id;       // Points to the "id" in the request frame
  int id_len;           // Length of the "id"
  mjson_print_fn_t fn;  // Printer function
  void *fndata;         // Printer function data
  void *userdata;       // Callback's user data as specified at export time
};

struct jsonrpc_method {
  const char *method;
  int method_sz;
  void (*cb)(struct jsonrpc_request *);
  void *cbdata;
  struct jsonrpc_method *next;
};

// Main RPC context, stores current request information and a list of
// exported RPC methods.
struct jsonrpc_ctx {
  struct jsonrpc_method *methods;
  void *userdata;
  void (*response_cb)(const char *buf, int len, void *userdata);
  int in_len;
  char in[MJSON_RPC_IN_BUF_SIZE];
};

// Registers function fn under the given name within the given RPC context
#define jsonrpc_ctx_export(ctx, name, fn, ud)                                \
  do {                                                                       \
    static struct jsonrpc_method m = {(name), sizeof(name) - 1, (fn), 0, 0}; \
    m.cbdata = (ud);                                                         \
    m.next = (ctx)->methods;                                                 \
    (ctx)->methods = &m;                                                     \
  } while (0)

void jsonrpc_ctx_init(struct jsonrpc_ctx *ctx,
                      void (*response_cb)(const char *, int, void *),
                      void *userdata);
int jsonrpc_call(mjson_print_fn_t fn, void *fndata, const char *fmt, ...);
void jsonrpc_return_error(struct jsonrpc_request *r, int code,
                          const char *message, const char *data_fmt, ...);
void jsonrpc_return_success(struct jsonrpc_request *r, const char *result_fmt,
                            ...);
void jsonrpc_ctx_process(struct jsonrpc_ctx *ctx, char *req, int req_sz,
                         mjson_print_fn_t fn, void *fndata);
void jsonrpc_ctx_process_byte(struct jsonrpc_ctx *ctx, unsigned char ch,
                              mjson_print_fn_t fn, void *fndata);

extern struct jsonrpc_ctx jsonrpc_default_context;

#define jsonrpc_export(name, fn, ud) \
  jsonrpc_ctx_export(&jsonrpc_default_context, (name), (fn), (ud))

#define jsonrpc_process(buf, len, fn, data) \
  jsonrpc_ctx_process(&jsonrpc_default_context, (buf), (len), (fn), (data))

#define jsonrpc_process_byte(x, fn, data) \
  jsonrpc_ctx_process_byte(&jsonrpc_default_context, (x), (fn), (data))

#define JSONRPC_ERROR_INVALID -32700    /* Invalid JSON was received */
#define JSONRPC_ERROR_NOT_FOUND -32601  /* The method does not exist */
#define JSONRPC_ERROR_BAD_PARAMS -32602 /* Invalid params passed */
#define JSONRPC_ERROR_INTERNAL -32603   /* Internal JSON-RPC error */

#endif  // MJSON_ENABLE_RPC
#endif  // MJSON_H


#ifdef __cplusplus
}
#endif
