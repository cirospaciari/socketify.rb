#ifndef UWS_APP_RB_H
#define UWS_APP_RB_H

#include "./uws.h"

#define uws_app_init(STATIC_VAR_NAME, STATIC_MODULE_NAME, TYPENAME, RUBY_NAME, CONSTRUCTOR_PARAM_LENGTH)     \
    STATIC_VAR_NAME = rb_define_class_under(STATIC_MODULE_NAME, RUBY_NAME, rb_cObject);                      \
    rb_define_alloc_func(STATIC_VAR_NAME, uws_rb_app_##TYPENAME##_new);                                      \
    rb_define_method(STATIC_VAR_NAME, "initialize", uws_rb_app_##TYPENAME##_init, CONSTRUCTOR_PARAM_LENGTH); \
    rb_define_method(STATIC_VAR_NAME, "run", uws_rb_app_##TYPENAME##_run, 0);                                \
    rb_define_method(STATIC_VAR_NAME, "close", uws_rb_app_##TYPENAME##_close, 0);                            \
    rb_define_method(STATIC_VAR_NAME, "listen", uws_rb_app_##TYPENAME##_listen, 2);                          \
    rb_define_method(STATIC_VAR_NAME, "get", uws_rb_app_##TYPENAME##_get, 2);                                \
    rb_define_method(STATIC_VAR_NAME, "post", uws_rb_app_##TYPENAME##_post, 2);                              \
    rb_define_method(STATIC_VAR_NAME, "options", uws_rb_app_##TYPENAME##_options, 2);                        \
    rb_define_method(STATIC_VAR_NAME, "delete", uws_rb_app_##TYPENAME##_delete, 2);                          \
    rb_define_method(STATIC_VAR_NAME, "patch", uws_rb_app_##TYPENAME##_patch, 2);                            \
    rb_define_method(STATIC_VAR_NAME, "put", uws_rb_app_##TYPENAME##_put, 2);                                \
    rb_define_method(STATIC_VAR_NAME, "head", uws_rb_app_##TYPENAME##_head, 2);                              \
    rb_define_method(STATIC_VAR_NAME, "connect", uws_rb_app_##TYPENAME##_connect, 2);                        \
    rb_define_method(STATIC_VAR_NAME, "trace", uws_rb_app_##TYPENAME##_trace, 2);                            \
    rb_define_method(STATIC_VAR_NAME, "any", uws_rb_app_##TYPENAME##_any, 2);                                \
    rb_define_method(STATIC_VAR_NAME, "num_subscribers", uws_rb_app_##TYPENAME##_num_subscribers, 1);        \
    rb_define_method(STATIC_VAR_NAME, "remove_server_name", uws_rb_app_##TYPENAME##_remove_server_name, 1);  \
    rb_define_method(STATIC_VAR_NAME, "add_server_name", uws_rb_app_##TYPENAME##_add_server_name, -1);

#define uws_app_http_method(TYPE, TYPENAME, METHOD)                                                           \
    static VALUE uws_rb_app_##TYPENAME##_##METHOD(VALUE self, VALUE pattern, VALUE callback)                  \
    {                                                                                                         \
        uws_rb_app_t *app;                                                                                    \
        Data_Get_Struct(self, uws_rb_app_t, app);                                                             \
        if (!RB_TYPE_P(pattern, T_STRING))                                                                    \
        {                                                                                                     \
            rb_raise(rb_eRuntimeError, "pattern must be a String");                                           \
            return Qnil;                                                                                      \
        }                                                                                                     \
        if (!rb_obj_is_proc(callback))                                                                        \
        {                                                                                                     \
            rb_raise(rb_eRuntimeError, "Callback is required in App.listen");                                 \
            return Qnil;                                                                                      \
        }                                                                                                     \
        /*when size is not passed use StringValueCStr instead of RSTRING_PTR*/                                \
        const char *c_pattern = StringValueCStr(pattern);                                                     \
        uws_rb_callback_t *rb_callback = (uws_rb_callback_t *)malloc(sizeof(uws_rb_callback_t));              \
        rb_callback->value = callback;                                                                        \
        rb_callback->data = app;                                                                              \
        uws_rb_callback_list_t *item = (uws_rb_callback_list_t *)malloc(sizeof(uws_rb_callback_list_t));      \
        item->value = rb_callback;                                                                            \
        item->next = app->callbacks;                                                                          \
        app->callbacks = item;                                                                                \
        uws_app_##METHOD(TYPE, app->ptr, c_pattern, uws_rb_generic_##TYPENAME##_method_handler, rb_callback); \
        RB_GC_GUARD(pattern);                                                                                 \
        RB_GC_GUARD(callback);                                                                                \
        return self;                                                                                          \
    }

#define uws_app(TYPE, TYPENAME, STATIC_UWS_APP_RESPONSE, STATIC_UWS_APP_REQUEST)                                                                  \
    static void uws_rb_app_##TYPENAME##_destroy(void *p)                                                                                          \
    {                                                                                                                                             \
        uws_rb_app_t *app = (uws_rb_app_t *)p;                                                                                                    \
        /*free callbacks*/                                                                                                                        \
        uws_rb_callback_list_t *callback = app->callbacks;                                                                                        \
        while (callback)                                                                                                                          \
        {                                                                                                                                         \
            uws_rb_callback_list_t *temp = callback;                                                                                              \
            free(temp->value);                                                                                                                    \
            free(temp);                                                                                                                           \
            callback = callback->next;                                                                                                            \
        }                                                                                                                                         \
        /*free app*/                                                                                                                              \
        uws_app_destroy(TYPE, app->ptr);                                                                                                          \
    }                                                                                                                                             \
    static VALUE uws_rb_app_##TYPENAME##_new(VALUE klass)                                                                                         \
    {                                                                                                                                             \
        uws_rb_app_t *app;                                                                                                                        \
        VALUE obj = Data_Make_Struct(klass, uws_rb_app_t, NULL, uws_rb_app_##TYPENAME##_destroy, app);                                            \
        app->ptr = NULL;                                                                                                                          \
        app->callbacks = NULL;                                                                                                                    \
        return obj;                                                                                                                               \
    }                                                                                                                                             \
    static void *uws_rb_app_##TYPENAME##_blocking_run_without_gvl(void *data)                                                                     \
    {                                                                                                                                             \
        uws_rb_app_t *app = (uws_rb_app_t *)data;                                                                                                 \
        uws_app_run(TYPE, app->ptr);                                                                                                              \
        return NULL;                                                                                                                              \
    }                                                                                                                                             \
    static VALUE uws_rb_app_##TYPENAME##_run(VALUE self)                                                                                          \
    {                                                                                                                                             \
        uws_rb_app_t *app;                                                                                                                        \
        Data_Get_Struct(self, uws_rb_app_t, app);                                                                                                 \
        rb_thread_call_without_gvl(uws_rb_app_##TYPENAME##_blocking_run_without_gvl, (void *)app, (rb_unblock_function_t *)-1, NULL);             \
        return self;                                                                                                                              \
    }                                                                                                                                             \
    static void uws_rb_generic_##TYPENAME##_method_handler(uws_res_t *response, uws_req_t *request, void *user_data)                              \
    {                                                                                                                                             \
        uws_rb_callback_t *callback_info = ((uws_rb_callback_t *)user_data);                                                                      \
        VALUE callback = callback_info->value;                                                                                                    \
        uws_rb_app_response_t *c_response;                                                                                                        \
        VALUE rb_response = rb_funcall(STATIC_UWS_APP_RESPONSE, rb_intern("new"), 0);                                                             \
        Data_Get_Struct(rb_response, uws_rb_app_response_t, c_response);                                                                          \
        c_response->ptr = response;                                                                                                               \
        uws_rb_app_request_t *c_request;                                                                                                          \
        VALUE rb_request = rb_funcall(STATIC_UWS_APP_REQUEST, rb_intern("new"), 0);                                                               \
        Data_Get_Struct(rb_request, uws_rb_app_request_t, c_request);                                                                             \
        c_request->ptr = request;                                                                                                                 \
        rb_funcall(callback, rb_intern("call"), 2, rb_response, rb_request);                                                                      \
        RB_GC_GUARD(rb_response);                                                                                                                 \
    }                                                                                                                                             \
    static VALUE uws_rb_app_##TYPENAME##_listen(VALUE self, VALUE port_or_config, VALUE callback)                                                 \
    {                                                                                                                                             \
        uws_rb_app_t *app;                                                                                                                        \
        Data_Get_Struct(self, uws_rb_app_t, app);                                                                                                 \
        if (NIL_P(callback) || !rb_obj_is_proc(callback))                                                                                         \
        {                                                                                                                                         \
            rb_raise(rb_eRuntimeError, "Callback is required in App.listen");                                                                     \
            return Qnil;                                                                                                                          \
        }                                                                                                                                         \
        if (RB_TYPE_P(port_or_config, T_FIXNUM))                                                                                                  \
        {                                                                                                                                         \
            uws_rb_callback_t *rb_callback = (uws_rb_callback_t *)malloc(sizeof(uws_rb_callback_t));                                              \
            rb_callback->value = callback;                                                                                                        \
            rb_callback->data = app;                                                                                                              \
            int port = RB_NUM2INT(port_or_config);                                                                                                \
            uws_app_listen(TYPE, app->ptr, port, uws_rb_generic_listen_handler, rb_callback);                                                     \
            RB_GC_GUARD(callback);                                                                                                                \
        }                                                                                                                                         \
        else if (RB_TYPE_P(port_or_config, T_OBJECT))                                                                                             \
        {                                                                                                                                         \
            uws_app_listen_config_t config;                                                                                                       \
            VALUE rb_port = rb_iv_get(port_or_config, "port");                                                                                    \
            if (RB_TYPE_P(rb_port, T_FIXNUM))                                                                                                     \
            {                                                                                                                                     \
                config.port = RB_NUM2INT(rb_port);                                                                                                \
            }                                                                                                                                     \
            else                                                                                                                                  \
            {                                                                                                                                     \
                config.port = 8080;                                                                                                               \
            }                                                                                                                                     \
            VALUE rb_options = rb_iv_get(port_or_config, "options");                                                                              \
            if (RB_TYPE_P(rb_options, T_FIXNUM))                                                                                                  \
            {                                                                                                                                     \
                config.options = RB_NUM2INT(rb_options);                                                                                          \
            }                                                                                                                                     \
            else                                                                                                                                  \
            {                                                                                                                                     \
                config.options = 0;                                                                                                               \
            }                                                                                                                                     \
            config.host = NULL;                                                                                                                   \
            VALUE rb_host = rb_iv_get(port_or_config, "host");                                                                                    \
            if (RB_TYPE_P(rb_host, T_STRING) && RSTRING_LEN(rb_host) > 0)                                                                         \
            {                                                                                                                                     \
                config.host = StringValueCStr(rb_host);                                                                                           \
            }                                                                                                                                     \
            else                                                                                                                                  \
            {                                                                                                                                     \
                config.host = "127.0.0.1";                                                                                                        \
            }                                                                                                                                     \
            uws_rb_callback_t *rb_callback = (uws_rb_callback_t *)malloc(sizeof(uws_rb_callback_t));                                              \
            rb_callback->value = callback;                                                                                                        \
            rb_callback->data = app;                                                                                                              \
            uws_app_listen_with_config(TYPE, app->ptr, config, uws_rb_generic_listen_handler, rb_callback);                                       \
            RB_GC_GUARD(rb_host);                                                                                                                 \
            RB_GC_GUARD(callback);                                                                                                                \
        }                                                                                                                                         \
        else if (RB_TYPE_P(port_or_config, T_HASH))                                                                                               \
        {                                                                                                                                         \
            uws_app_listen_config_t config;                                                                                                       \
            VALUE rb_port = rb_hash_aref(port_or_config, ID2SYM(rb_intern("port")));                                                              \
            if (RB_TYPE_P(rb_port, T_FIXNUM))                                                                                                     \
            {                                                                                                                                     \
                config.port = RB_NUM2INT(rb_port);                                                                                                \
            }                                                                                                                                     \
            else                                                                                                                                  \
            {                                                                                                                                     \
                config.port = 8080;                                                                                                               \
            }                                                                                                                                     \
            VALUE rb_options = rb_hash_aref(port_or_config, ID2SYM(rb_intern("options")));                                                        \
            if (RB_TYPE_P(rb_options, T_FIXNUM))                                                                                                  \
            {                                                                                                                                     \
                config.options = RB_NUM2INT(rb_options);                                                                                          \
            }                                                                                                                                     \
            else                                                                                                                                  \
            {                                                                                                                                     \
                config.options = 0;                                                                                                               \
            }                                                                                                                                     \
            config.host = NULL;                                                                                                                   \
            VALUE rb_host = rb_hash_aref(port_or_config, ID2SYM(rb_intern("host")));                                                              \
            if (RB_TYPE_P(rb_host, T_STRING) && RSTRING_LEN(rb_host) > 0)                                                                         \
            {                                                                                                                                     \
                config.host = StringValueCStr(rb_host);                                                                                           \
            }                                                                                                                                     \
            else                                                                                                                                  \
            {                                                                                                                                     \
                config.host = "127.0.0.1";                                                                                                        \
            }                                                                                                                                     \
            uws_rb_callback_t *rb_callback = (uws_rb_callback_t *)malloc(sizeof(uws_rb_callback_t));                                              \
            rb_callback->value = callback;                                                                                                        \
            rb_callback->data = app;                                                                                                              \
            uws_app_listen_with_config(TYPE, app->ptr, config, uws_rb_generic_listen_handler, rb_callback);                                       \
            RB_GC_GUARD(rb_host);                                                                                                                 \
            RB_GC_GUARD(callback);                                                                                                                \
        }                                                                                                                                         \
        else                                                                                                                                      \
        {                                                                                                                                         \
            rb_raise(rb_eRuntimeError, "Invalid Port (integer) or Configuration (Object/Hash) in App.listen");                                    \
        }                                                                                                                                         \
        return self;                                                                                                                              \
    }                                                                                                                                             \
    static VALUE uws_rb_app_##TYPENAME##_close(VALUE self)                                                                                        \
    {                                                                                                                                             \
        uws_rb_app_t *app;                                                                                                                        \
        Data_Get_Struct(self, uws_rb_app_t, app);                                                                                                 \
        us_listen_socket_close(TYPE, app->listening_socket);                                                                                      \
        return self;                                                                                                                              \
    }                                                                                                                                             \
    static VALUE uws_rb_app_##TYPENAME##_num_subscribers(VALUE self, VALUE topic)                                                                 \
    {                                                                                                                                             \
        uws_rb_app_t *app;                                                                                                                        \
        Data_Get_Struct(self, uws_rb_app_t, app);                                                                                                 \
        if (RB_TYPE_P(topic, T_STRING) && RSTRING_LEN(topic) > 0)                                                                                 \
        {                                                                                                                                         \
            unsigned int subscribers = uws_num_subscribers(TYPE, app->listening_socket, StringValueCStr(topic));                                  \
            RB_GC_GUARD(topic);                                                                                                                   \
            return UINT2NUM(subscribers);                                                                                                         \
        }                                                                                                                                         \
        return INT2NUM(0);                                                                                                                        \
    }                                                                                                                                             \
    static VALUE uws_rb_app_##TYPENAME##_remove_server_name(VALUE self, VALUE hostname_pattern)                                                   \
    {                                                                                                                                             \
        uws_rb_app_t *app;                                                                                                                        \
        Data_Get_Struct(self, uws_rb_app_t, app);                                                                                                 \
        if (RB_TYPE_P(hostname_pattern, T_STRING) && RSTRING_LEN(hostname_pattern) > 0)                                                           \
        {                                                                                                                                         \
            uws_remove_server_name(TYPE, app->listening_socket, StringValueCStr(hostname_pattern));                                               \
            RB_GC_GUARD(hostname_pattern);                                                                                                        \
        }                                                                                                                                         \
        return self;                                                                                                                              \
    }                                                                                                                                             \
    static VALUE uws_rb_app_##TYPENAME##_add_server_name(int argc, VALUE *argv, VALUE self)                                                       \
    {                                                                                                                                             \
        VALUE hostname_pattern, config;                                                                                                           \
        rb_scan_args(argc, argv, "11", &hostname_pattern, &config); /* informs ruby that the method takes 1 mandatory and 1 optional argument, */ \
                                                                    /* the values of which are stored in hostname_pattern and config.*/           \
                                                                                                                                                  \
        uws_rb_app_t *app;                                                                                                                        \
        Data_Get_Struct(self, uws_rb_app_t, app);                                                                                                 \
        if (RB_TYPE_P(hostname_pattern, T_STRING) && RSTRING_LEN(hostname_pattern) > 0)                                                           \
        {                                                                                                                                         \
            if (NIL_P(config))                                                                                                                    \
            { /* if no config was given use default values*/                                                                                      \
                uws_add_server_name(TYPE, app->listening_socket, StringValueCStr(hostname_pattern));                                              \
            }                                                                                                                                     \
            else                                                                                                                                  \
            {                                                                                                                                     \
                struct us_socket_context_options_t options;                                                                                       \
                                                                                                                                                  \
                VALUE rb_key_file_name = Qnil;                                                                                                    \
                VALUE rb_cert_file_name = Qnil;                                                                                                   \
                VALUE rb_dh_params_file_name = Qnil;                                                                                              \
                VALUE rb_ca_file_name = Qnil;                                                                                                     \
                VALUE rb_ssl_prefer_low_memory_usage = Qnil;                                                                                      \
                VALUE rb_passphrase = Qnil;                                                                                                       \
                                                                                                                                                  \
                if (RB_TYPE_P(config, T_HASH))                                                                                                    \
                {                                                                                                                                 \
                    rb_key_file_name = rb_hash_aref(config, ID2SYM(rb_intern("key_file_name")));                                                  \
                    if (RB_TYPE_P(rb_key_file_name, T_STRING) && RSTRING_LEN(rb_key_file_name) > 0)                                               \
                    {                                                                                                                             \
                        options.key_file_name = StringValueCStr(rb_key_file_name);                                                                \
                    }                                                                                                                             \
                    else                                                                                                                          \
                    {                                                                                                                             \
                        options.key_file_name = NULL;                                                                                             \
                    }                                                                                                                             \
                                                                                                                                                  \
                    rb_cert_file_name = rb_hash_aref(config, ID2SYM(rb_intern("cert_file_name")));                                                \
                    if (RB_TYPE_P(rb_cert_file_name, T_STRING) && RSTRING_LEN(rb_cert_file_name) > 0)                                             \
                    {                                                                                                                             \
                        options.cert_file_name = StringValueCStr(rb_cert_file_name);                                                              \
                    }                                                                                                                             \
                    else                                                                                                                          \
                    {                                                                                                                             \
                        options.cert_file_name = NULL;                                                                                            \
                    }                                                                                                                             \
                                                                                                                                                  \
                    rb_passphrase = rb_hash_aref(config, ID2SYM(rb_intern("passphrase")));                                                        \
                    if (RB_TYPE_P(rb_passphrase, T_STRING) && RSTRING_LEN(rb_passphrase) > 0)                                                     \
                    {                                                                                                                             \
                        options.passphrase = StringValueCStr(rb_passphrase);                                                                      \
                    }                                                                                                                             \
                    else                                                                                                                          \
                    {                                                                                                                             \
                        options.passphrase = NULL;                                                                                                \
                    }                                                                                                                             \
                                                                                                                                                  \
                    rb_dh_params_file_name = rb_hash_aref(config, ID2SYM(rb_intern("dh_params_file_name")));                                      \
                    if (RB_TYPE_P(rb_dh_params_file_name, T_STRING) && RSTRING_LEN(rb_dh_params_file_name) > 0)                                   \
                    {                                                                                                                             \
                        options.dh_params_file_name = StringValueCStr(rb_dh_params_file_name);                                                    \
                    }                                                                                                                             \
                    else                                                                                                                          \
                    {                                                                                                                             \
                        options.dh_params_file_name = NULL;                                                                                       \
                    }                                                                                                                             \
                                                                                                                                                  \
                    rb_ca_file_name = rb_hash_aref(config, ID2SYM(rb_intern("ca_file_name")));                                                    \
                    if (RB_TYPE_P(rb_ca_file_name, T_STRING) && RSTRING_LEN(rb_ca_file_name) > 0)                                                 \
                    {                                                                                                                             \
                        options.ca_file_name = StringValueCStr(rb_ca_file_name);                                                                  \
                    }                                                                                                                             \
                    else                                                                                                                          \
                    {                                                                                                                             \
                        options.ca_file_name = NULL;                                                                                              \
                    }                                                                                                                             \
                                                                                                                                                  \
                    rb_ssl_prefer_low_memory_usage = rb_hash_aref(config, ID2SYM(rb_intern("ssl_prefer_low_memory_usage")));                      \
                    if (RB_TYPE_P(rb_ssl_prefer_low_memory_usage, T_FIXNUM))                                                                      \
                    {                                                                                                                             \
                        options.ssl_prefer_low_memory_usage = RB_NUM2INT(rb_ssl_prefer_low_memory_usage);                                         \
                    }                                                                                                                             \
                }                                                                                                                                 \
                else if (RB_TYPE_P(config, T_OBJECT))                                                                                             \
                {                                                                                                                                 \
                    rb_key_file_name = rb_iv_get(config, "key_file_name");                                                                        \
                    if (RB_TYPE_P(rb_key_file_name, T_STRING) && RSTRING_LEN(rb_key_file_name) > 0)                                               \
                    {                                                                                                                             \
                        options.key_file_name = StringValueCStr(rb_key_file_name);                                                                \
                    }                                                                                                                             \
                    else                                                                                                                          \
                    {                                                                                                                             \
                        options.key_file_name = NULL;                                                                                             \
                    }                                                                                                                             \
                                                                                                                                                  \
                    rb_cert_file_name = rb_iv_get(config, "cert_file_name");                                                                      \
                    if (RB_TYPE_P(rb_cert_file_name, T_STRING) && RSTRING_LEN(rb_cert_file_name) > 0)                                             \
                    {                                                                                                                             \
                        options.cert_file_name = StringValueCStr(rb_cert_file_name);                                                              \
                    }                                                                                                                             \
                    else                                                                                                                          \
                    {                                                                                                                             \
                        options.cert_file_name = NULL;                                                                                            \
                    }                                                                                                                             \
                                                                                                                                                  \
                    rb_passphrase = rb_iv_get(config, "passphrase");                                                                              \
                    if (RB_TYPE_P(rb_passphrase, T_STRING) && RSTRING_LEN(rb_passphrase) > 0)                                                     \
                    {                                                                                                                             \
                        options.passphrase = StringValueCStr(rb_passphrase);                                                                      \
                    }                                                                                                                             \
                    else                                                                                                                          \
                    {                                                                                                                             \
                        options.passphrase = NULL;                                                                                                \
                    }                                                                                                                             \
                                                                                                                                                  \
                    rb_dh_params_file_name = rb_iv_get(config, "dh_params_file_name");                                                            \
                    if (RB_TYPE_P(rb_dh_params_file_name, T_STRING) && RSTRING_LEN(rb_dh_params_file_name) > 0)                                   \
                    {                                                                                                                             \
                        options.dh_params_file_name = StringValueCStr(rb_dh_params_file_name);                                                    \
                    }                                                                                                                             \
                    else                                                                                                                          \
                    {                                                                                                                             \
                        options.dh_params_file_name = NULL;                                                                                       \
                    }                                                                                                                             \
                                                                                                                                                  \
                    rb_ca_file_name = rb_iv_get(config, "ca_file_name");                                                                          \
                    if (RB_TYPE_P(rb_ca_file_name, T_STRING) && RSTRING_LEN(rb_ca_file_name) > 0)                                                 \
                    {                                                                                                                             \
                        options.ca_file_name = StringValueCStr(rb_ca_file_name);                                                                  \
                    }                                                                                                                             \
                    else                                                                                                                          \
                    {                                                                                                                             \
                        options.ca_file_name = NULL;                                                                                              \
                    }                                                                                                                             \
                                                                                                                                                  \
                    rb_ssl_prefer_low_memory_usage = rb_iv_get(config, "ssl_prefer_low_memory_usage");                                            \
                    if (RB_TYPE_P(rb_ssl_prefer_low_memory_usage, T_FIXNUM))                                                                      \
                    {                                                                                                                             \
                        options.ssl_prefer_low_memory_usage = RB_NUM2INT(rb_ssl_prefer_low_memory_usage);                                         \
                    }                                                                                                                             \
                }                                                                                                                                 \
                else                                                                                                                              \
                {                                                                                                                                 \
                    rb_raise(rb_eRuntimeError, "Options must be an Object/Has) in App.add_server_name");                                          \
                    return self;                                                                                                                  \
                }                                                                                                                                 \
                uws_add_server_name_with_options(TYPE, app->listening_socket, StringValueCStr(hostname_pattern), options);                        \
                RB_GC_GUARD(options);                                                                                                             \
            }                                                                                                                                     \
            RB_GC_GUARD(hostname_pattern);                                                                                                        \
        }                                                                                                                                         \
        return self;                                                                                                                              \
    }

//generic used handlers
void uws_rb_generic_listen_handler(struct us_listen_socket_t *listen_socket, uws_app_listen_config_t config, void *user_data);
//app constructors
VALUE uws_rb_app_default_init(VALUE self);
VALUE uws_rb_app_ssl_init(VALUE self, VALUE config);

#endif