#include <ruby.h>
#include <ruby/thread.h>
#include "libuwebsockets.h"
#include "libusockets.h"

#define DEFAULT_APP 0
#define SSL_APP 1

static VALUE UWS_App;
static VALUE UWS_Module;
static VALUE UWS_AppResponse;
typedef struct
{
  VALUE value;
  void *app;
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

static void uws_rb_app_destroy(void *p)
{
  uws_rb_app_t *app = (uws_rb_app_t *)p;

  //free callbacks
  uws_rb_callback_list_t *callback = app->callbacks;

  while (callback)
  {
    uws_rb_callback_list_t *temp = callback;
    free(temp->value);
    free(temp);

    callback = callback->next;
  }

  //free app
  uws_app_destroy(DEFAULT_APP, app->ptr);
}

static void *uws_rb_app_blocking_run_without_gvl(void *data)
{

  uws_rb_app_t *app = (uws_rb_app_t *)data;
  uws_app_run(DEFAULT_APP, app->ptr);
  return NULL;
}

static VALUE uws_rb_app_run(VALUE self)
{

  uws_rb_app_t *app;
  Data_Get_Struct(self, uws_rb_app_t, app);
  rb_thread_call_without_gvl(uws_rb_app_blocking_run_without_gvl, (void *)app, (rb_unblock_function_t *)-1, NULL);
  return self;
}

static void uws_rb_generic_listen_handler(struct us_listen_socket_t *listen_socket, uws_app_listen_config_t config, void *user_data)
{

  uws_rb_callback_t *info = ((uws_rb_callback_t *)user_data);
  VALUE callback = info->value;

  VALUE port = INT2FIX(config.port);
  VALUE options = INT2FIX(config.options);
  VALUE host = Qnil;
  if (config.host)
  {
    host = rb_str_new2(config.host);
  }

  ((uws_rb_app_t *)info->app)->listening_socket = listen_socket;

  //free callback data
  free(user_data);

  VALUE UWS_ListenConfig = rb_const_get(UWS_Module, rb_intern("ListenConfig"));
  VALUE obj = rb_funcall(UWS_ListenConfig, rb_intern("new"), 3, port, host, options);

  rb_funcall(callback, rb_intern("call"), 1, obj);
}

static void uws_rb_generic_method_handler(uws_res_t *response, uws_req_t *request, void *user_data)
{
  VALUE callback = ((uws_rb_callback_t *)user_data)->value;

  uws_rb_app_response_t *c_response;
  VALUE rb_response = rb_funcall(UWS_AppResponse, rb_intern("new"), 0);
  Data_Get_Struct(rb_response, uws_rb_app_response_t, c_response);
  c_response->ptr = response;

  rb_funcall(callback, rb_intern("call"), 1, rb_response);
}

static VALUE uws_rb_app_get(VALUE self, VALUE pattern, VALUE callback)
{

  uws_rb_app_t *app;
  Data_Get_Struct(self, uws_rb_app_t, app);

  if (!RB_TYPE_P(pattern, T_STRING))
  {
    rb_raise(rb_eRuntimeError, "pattern must be a String");
    return Qnil;
  }
  if (!rb_obj_is_proc(callback))
  {
    rb_raise(rb_eRuntimeError, "Callback is required in App.listen");
    return Qnil;
  }

  const char *c_pattern = StringValueCStr(pattern);

  uws_rb_callback_t *rb_callback = (uws_rb_callback_t *)malloc(sizeof(uws_rb_callback_t));
  rb_callback->value = callback;
  rb_callback->app = app;

  uws_rb_callback_list_t *item = (uws_rb_callback_list_t *)malloc(sizeof(uws_rb_callback_list_t));
  item->value = rb_callback;
  item->next = app->callbacks;
  app->callbacks = item;

  uws_app_get(DEFAULT_APP, app->ptr, c_pattern, uws_rb_generic_method_handler, rb_callback);
  return self;
}

static VALUE uws_rb_app_listen(VALUE self, VALUE port_or_config, VALUE callback)
{

  uws_rb_app_t *app;
  Data_Get_Struct(self, uws_rb_app_t, app);

  if (NIL_P(callback) || !rb_obj_is_proc(callback))
  {
    rb_raise(rb_eRuntimeError, "Callback is required in App.listen");
    return Qnil;
  }

  if (RB_TYPE_P(port_or_config, T_FIXNUM))
  {
    uws_rb_callback_t *rb_callback = (uws_rb_callback_t *)malloc(sizeof(uws_rb_callback_t));
    rb_callback->value = callback;
    rb_callback->app = app;
    int port = RB_NUM2INT(port_or_config);
    uws_app_listen(DEFAULT_APP, app->ptr, port, uws_rb_generic_listen_handler, rb_callback);
  }
  else if (RB_TYPE_P(port_or_config, T_OBJECT))
  {

    uws_app_listen_config_t config;
    config.port = RB_NUM2INT(rb_iv_get(port_or_config, "port"));
    config.options = RB_NUM2INT(rb_iv_get(port_or_config, "options"));
    config.host = NULL;
    VALUE rb_host = rb_iv_get(port_or_config, "host");
    if (RB_TYPE_P(rb_host, T_STRING))
    {
      config.host = StringValueCStr(rb_host);
    }
    uws_rb_callback_t *rb_callback = (uws_rb_callback_t *)malloc(sizeof(uws_rb_callback_t));
    rb_callback->value = callback;
    rb_callback->app = app;
    uws_app_listen_with_config(DEFAULT_APP, app->ptr, config, uws_rb_generic_listen_handler, rb_callback);
  }
  return self;
}

static VALUE uws_rb_app_close(VALUE self)
{
  uws_rb_app_t *app;
  Data_Get_Struct(self, uws_rb_app_t, app);
  us_listen_socket_close(DEFAULT_APP, app->listening_socket);
  return self;
}

static VALUE uws_rb_app_response_new(VALUE klass)
{
  uws_rb_app_response_t *app;

  VALUE obj = Data_Make_Struct(klass, uws_rb_app_response_t, NULL, free, app);
  app->ptr = NULL;

  return obj;
}
static VALUE uws_rb_app_write_header(VALUE self, VALUE header, VALUE value)
{
  if (!RB_TYPE_P(header, T_STRING) || !RB_TYPE_P(value, T_STRING))
  {
    rb_raise(rb_eRuntimeError, "header name and value are required to be a String in AppResponse.write_header");
    return self;
  }
  uws_rb_app_response_t *response;
  Data_Get_Struct(self, uws_rb_app_response_t, response);
  uws_res_write_header(DEFAULT_APP, response->ptr, StringValueCStr(header), RSTRING_LEN(header), StringValueCStr(value), RSTRING_LEN(value));

  return self;
}

int uws_rb_set_all_headers(VALUE header, VALUE value, VALUE self)
{

  uws_rb_app_response_t *response;
  Data_Get_Struct(self, uws_rb_app_response_t, response);

  if (RB_TYPE_P(header, T_SYMBOL) && RB_TYPE_P(value, T_STRING))
  {
    header = rb_sym2str(header);
    uws_res_write_header(DEFAULT_APP, response->ptr, StringValueCStr(header), RSTRING_LEN(header), StringValueCStr(value), RSTRING_LEN(value));
  }
  return ST_CONTINUE;
}

static VALUE uws_rb_app_response_end_with_headers(VALUE self, VALUE message, VALUE headers)
{

  uws_rb_app_response_t *response;
  Data_Get_Struct(self, uws_rb_app_response_t, response);

  if (RB_TYPE_P(headers, T_HASH))
  {
    rb_hash_foreach(headers, uws_rb_set_all_headers, self);
  }

  if (RB_TYPE_P(message, T_STRING))
  {
    uws_res_end(DEFAULT_APP, response->ptr, StringValueCStr(message), RSTRING_LEN(message), false);
  }
  return self;
}

static VALUE uws_rb_app_response_end(VALUE self, VALUE message)
{
  uws_rb_app_response_t *response;
  Data_Get_Struct(self, uws_rb_app_response_t, response);

  if (RB_TYPE_P(message, T_STRING))
  {
    uws_res_end(DEFAULT_APP, response->ptr, StringValueCStr(message), RSTRING_LEN(message), false);
  }
  return self;
}

static VALUE uws_rb_app_new(VALUE klass)
{
  uws_rb_app_t *app;

  VALUE obj = Data_Make_Struct(klass, uws_rb_app_t, NULL, uws_rb_app_destroy, app);

  app->ptr = NULL;
  app->callbacks = NULL;

  return obj;
}

static VALUE uws_rb_app_init(VALUE self)
{
  uws_rb_app_t *app;

  Data_Get_Struct(self, uws_rb_app_t, app);

  struct us_socket_context_options_t options;
  options.key_file_name = NULL;
  options.cert_file_name = NULL;
  options.passphrase = NULL;
  options.dh_params_file_name = NULL;
  options.ca_file_name = NULL;
  options.ssl_prefer_low_memory_usage = 0;
  app->ptr = uws_create_app(DEFAULT_APP, options);
  if (uws_constructor_failed(DEFAULT_APP, app->ptr))
    rb_raise(rb_eNoMemError, "unable to allocate uws_app_t");

  return self;
}

void Init_uws(void)
{
  // #if RUBY_VERSION_MAJOR == 3
    rb_ext_ractor_safe(true);
  // #endif
  
  UWS_Module = rb_define_module("UWS");

  UWS_App = rb_define_class_under(UWS_Module, "App", rb_cObject);
  rb_define_alloc_func(UWS_App, uws_rb_app_new);
  rb_define_method(UWS_App, "initialize", uws_rb_app_init, 0);
  rb_define_method(UWS_App, "get", uws_rb_app_get, 2);
  rb_define_method(UWS_App, "listen", uws_rb_app_listen, 2);
  rb_define_method(UWS_App, "run", uws_rb_app_run, 0);
  rb_define_method(UWS_App, "close", uws_rb_app_close, 0);

  UWS_AppResponse = rb_define_class_under(UWS_Module, "AppReponse", rb_cObject);
  rb_define_alloc_func(UWS_AppResponse, uws_rb_app_response_new);
  rb_define_method(UWS_AppResponse, "write_header", uws_rb_app_write_header, 2);
  rb_define_method(UWS_AppResponse, "end", uws_rb_app_response_end, 1);
  rb_define_method(UWS_AppResponse, "end_with_headers", uws_rb_app_response_end_with_headers, 2);

  // rb_define_attr(UWS_App, "callbacks", 1, 1);
  //rb_iv_get(obj, "host")
}