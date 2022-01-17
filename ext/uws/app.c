#include "./uws.h"
#include "./app.h"

void uws_rb_generic_listen_handler(struct us_listen_socket_t *listen_socket, uws_app_listen_config_t config, void *user_data)
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

  ((uws_rb_app_t *)info->data)->listening_socket = listen_socket;

  //free callback data
  free(user_data);

  VALUE UWS_Module = rb_const_get(rb_cObject, rb_intern("UWS"));
  VALUE UWS_ListenConfig = rb_const_get(UWS_Module, rb_intern("ListenConfig"));
  VALUE obj = rb_funcall(UWS_ListenConfig, rb_intern("new"), 3, port, host, options);
  RB_GC_GUARD(UWS_ListenConfig);
  RB_GC_GUARD(port);
  RB_GC_GUARD(host);
  RB_GC_GUARD(options);

  rb_funcall(callback, rb_intern("call"), 1, obj);
  RB_GC_GUARD(obj);
}

VALUE uws_rb_app_default_init(VALUE self)
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
  app->ptr = uws_create_app(UWS_DEFAULT_APP, options);
  if (uws_constructor_failed(UWS_DEFAULT_APP, app->ptr))
    rb_raise(rb_eNoMemError, "unable to allocate uws_app_t");
  return self;
}
VALUE uws_rb_app_ssl_init(VALUE self, VALUE config)
{
  uws_rb_app_t *app;
  Data_Get_Struct(self, uws_rb_app_t, app);
  struct us_socket_context_options_t options;

  VALUE rb_key_file_name = Qnil;
  VALUE rb_cert_file_name = Qnil;
  VALUE rb_dh_params_file_name = Qnil;
  VALUE rb_ca_file_name = Qnil;
  VALUE rb_ssl_prefer_low_memory_usage = Qnil;
  VALUE rb_passphrase = Qnil;

  if (RB_TYPE_P(config, T_HASH))
  {
    rb_key_file_name = rb_hash_aref(config, ID2SYM(rb_intern("key_file_name")));
    if (RB_TYPE_P(rb_key_file_name, T_STRING) && RSTRING_LEN(rb_key_file_name) > 0)
    {
      options.key_file_name = StringValueCStr(rb_key_file_name);
    }
    else
    {
      options.key_file_name = NULL;
    }

    rb_cert_file_name = rb_hash_aref(config, ID2SYM(rb_intern("cert_file_name")));
    if (RB_TYPE_P(rb_cert_file_name, T_STRING) && RSTRING_LEN(rb_cert_file_name) > 0)
    {
      options.cert_file_name = StringValueCStr(rb_cert_file_name);
    }
    else
    {
      options.cert_file_name = NULL;
    }

    rb_passphrase = rb_hash_aref(config, ID2SYM(rb_intern("passphrase")));
    if (RB_TYPE_P(rb_passphrase, T_STRING) && RSTRING_LEN(rb_passphrase) > 0)
    {
      options.passphrase = StringValueCStr(rb_passphrase);
    }
    else
    {
      options.passphrase = NULL;
    }

    rb_dh_params_file_name = rb_hash_aref(config, ID2SYM(rb_intern("dh_params_file_name")));
    if (RB_TYPE_P(rb_dh_params_file_name, T_STRING) && RSTRING_LEN(rb_dh_params_file_name) > 0)
    {
      options.dh_params_file_name = StringValueCStr(rb_dh_params_file_name);
    }
    else
    {
      options.dh_params_file_name = NULL;
    }

    rb_ca_file_name = rb_hash_aref(config, ID2SYM(rb_intern("ca_file_name")));
    if (RB_TYPE_P(rb_ca_file_name, T_STRING) && RSTRING_LEN(rb_ca_file_name) > 0)
    {
      options.ca_file_name = StringValueCStr(rb_ca_file_name);
    }
    else
    {
      options.ca_file_name = NULL;
    }

    rb_ssl_prefer_low_memory_usage = rb_hash_aref(config, ID2SYM(rb_intern("ssl_prefer_low_memory_usage")));
    if (RB_TYPE_P(rb_ssl_prefer_low_memory_usage, T_FIXNUM))
    {
      options.ssl_prefer_low_memory_usage = RB_NUM2INT(rb_ssl_prefer_low_memory_usage);
    }
  }
  else if (RB_TYPE_P(config, T_OBJECT))
  {
    rb_key_file_name = rb_iv_get(config, "key_file_name");
    if (RB_TYPE_P(rb_key_file_name, T_STRING) && RSTRING_LEN(rb_key_file_name) > 0)
    {
      options.key_file_name = StringValueCStr(rb_key_file_name);
    }
    else
    {
      options.key_file_name = NULL;
    }

    rb_cert_file_name = rb_iv_get(config, "cert_file_name");
    if (RB_TYPE_P(rb_cert_file_name, T_STRING) && RSTRING_LEN(rb_cert_file_name) > 0)
    {
      options.cert_file_name = StringValueCStr(rb_cert_file_name);
    }
    else
    {
      options.cert_file_name = NULL;
    }

    rb_passphrase = rb_iv_get(config, "passphrase");
    if (RB_TYPE_P(rb_passphrase, T_STRING) && RSTRING_LEN(rb_passphrase) > 0)
    {
      options.passphrase = StringValueCStr(rb_passphrase);
    }
    else
    {
      options.passphrase = NULL;
    }

    rb_dh_params_file_name = rb_iv_get(config, "dh_params_file_name");
    if (RB_TYPE_P(rb_dh_params_file_name, T_STRING) && RSTRING_LEN(rb_dh_params_file_name) > 0)
    {
      options.dh_params_file_name = StringValueCStr(rb_dh_params_file_name);
    }
    else
    {
      options.dh_params_file_name = NULL;
    }

    rb_ca_file_name = rb_iv_get(config, "ca_file_name");
    if (RB_TYPE_P(rb_ca_file_name, T_STRING) && RSTRING_LEN(rb_ca_file_name) > 0)
    {
      options.ca_file_name = StringValueCStr(rb_ca_file_name);
    }
    else
    {
      options.ca_file_name = NULL;
    }

    rb_ssl_prefer_low_memory_usage = rb_iv_get(config, "ssl_prefer_low_memory_usage");
    if (RB_TYPE_P(rb_ssl_prefer_low_memory_usage, T_FIXNUM))
    {
      options.ssl_prefer_low_memory_usage = RB_NUM2INT(rb_ssl_prefer_low_memory_usage);
    }
  }
  else
  {
    rb_raise(rb_eRuntimeError, "Configuration (Object/Hash) in SSLApp contructor");
    return self;
  }

  app->ptr = uws_create_app(UWS_SSL_APP, options);
  if (uws_constructor_failed(UWS_SSL_APP, app->ptr))
    rb_raise(rb_eNoMemError, "unable to allocate uws_app_t");

  RB_GC_GUARD(rb_ssl_prefer_low_memory_usage);
  RB_GC_GUARD(rb_key_file_name);
  RB_GC_GUARD(rb_cert_file_name);
  RB_GC_GUARD(rb_dh_params_file_name);
  RB_GC_GUARD(rb_ca_file_name);
  RB_GC_GUARD(rb_passphrase);
  return self;
}