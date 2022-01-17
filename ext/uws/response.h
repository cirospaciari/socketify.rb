#ifndef UWS_RESPONSE_RB_H
#define UWS_RESPONSE_RB_H

#include "./uws.h"
#define uws_response_init(STATIC_VAR_NAME, STATIC_MODULE_NAME, TYPENAME, RUBY_NAME)             \
    STATIC_VAR_NAME = rb_define_class_under(STATIC_MODULE_NAME, RUBY_NAME, rb_cObject);         \
    rb_define_alloc_func(STATIC_VAR_NAME, uws_rb_app_##TYPENAME##_response_new);                \
    rb_define_method(STATIC_VAR_NAME, "write_header", uws_rb_app_##TYPENAME##_write_header, 2); \
    rb_define_method(STATIC_VAR_NAME, "end", uws_rb_app_##TYPENAME##_response_end, 1);

#define uws_response(TYPE, TYPENAME)                                                                                                 \
    static VALUE uws_rb_app_##TYPENAME##_response_new(VALUE klass)                                                                   \
    {                                                                                                                                \
        uws_rb_app_response_t *response;                                                                                             \
        VALUE obj = Data_Make_Struct(klass, uws_rb_app_response_t, NULL, free, response);                                            \
        response->ptr = NULL;                                                                                                        \
        return obj;                                                                                                                  \
    }                                                                                                                                \
    static VALUE uws_rb_app_##TYPENAME##_write_header(VALUE self, VALUE header, VALUE value)                                         \
    {                                                                                                                                \
        if (!RB_TYPE_P(header, T_STRING) || !RB_TYPE_P(value, T_STRING))                                                             \
        {                                                                                                                            \
            rb_raise(rb_eRuntimeError, "header name and value are required to be a String in AppResponse.write_header");             \
            return self;                                                                                                             \
        }                                                                                                                            \
        uws_rb_app_response_t *response;                                                                                             \
        Data_Get_Struct(self, uws_rb_app_response_t, response);                                                                      \
        uws_res_write_header(TYPE, response->ptr, RSTRING_PTR(header), RSTRING_LEN(header), RSTRING_PTR(value), RSTRING_LEN(value)); \
        RB_GC_GUARD(header);                                                                                                         \
        RB_GC_GUARD(value);                                                                                                          \
        return self;                                                                                                                 \
    }                                                                                                                                \
    static VALUE uws_rb_app_##TYPENAME##_response_end(VALUE self, VALUE message)                                                     \
    {                                                                                                                                \
        uws_rb_app_response_t *response;                                                                                             \
        Data_Get_Struct(self, uws_rb_app_response_t, response);                                                                      \
        if (RB_TYPE_P(message, T_STRING))                                                                                            \
        {                                                                                                                            \
            uws_res_end(TYPE, response->ptr, RSTRING_PTR(message), RSTRING_LEN(message), false);                                     \
            RB_GC_GUARD(message);                                                                                                    \
        }                                                                                                                            \
        return self;                                                                                                                 \
    }

#endif