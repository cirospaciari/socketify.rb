
#ifndef UWS_RB_H
#define UWS_RB_H

#include <ruby.h>
#include <ruby/thread.h>
#include "libuwebsockets.h"
#include "libusockets.h"

#define UWS_DEFAULT_APP 0
#define UWS_SSL_APP 1

typedef struct
{
  VALUE value;
  void *data;
} uws_rb_callback_t;


typedef struct uws_rb_callback_list_s
{
  uws_rb_callback_t *value;
  struct uws_rb_callback_list_s *next;
} uws_rb_callback_list_t;

typedef struct
{
  void *ptr;
  void *listening_socket;
  uws_rb_callback_list_t *callbacks;
  int is_running;
} uws_rb_app_t;

typedef struct
{
  void *ptr;
} uws_rb_app_response_t;


typedef struct
{
  void *ptr;
} uws_rb_app_request_t;


#endif