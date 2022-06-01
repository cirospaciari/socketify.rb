#ifndef UWS_RESPONSE_RB_H
#define UWS_RESPONSE_RB_H

#include "./socketify.h"

bool uws_rb_response_generic_writeble_handler(uws_res_t *res, uintmax_t value, void *user_data);
void uws_rb_response_generic_aborted_handler(uws_res_t *res, void *user_data);
void uws_rb_response_generic_data_handler(uws_res_t *res, const char *chunk, size_t chunk_length, bool is_end, void *opcional_data);

#define uws_response_init(STATIC_VAR_NAME, STATIC_MODULE_NAME, TYPENAME, RUBY_NAME)                              \
    STATIC_VAR_NAME = rb_define_class_under(STATIC_MODULE_NAME, RUBY_NAME, rb_cObject);                          \
    rb_define_alloc_func(STATIC_VAR_NAME, uws_rb_app_##TYPENAME##_response_new);                                 \
    rb_define_method(STATIC_VAR_NAME, "write_header", uws_rb_app_##TYPENAME##_write_header, 2);                  \
    rb_define_method(STATIC_VAR_NAME, "write", uws_rb_app_##TYPENAME##_response_write, 1);                       \
    rb_define_method(STATIC_VAR_NAME, "write_status", uws_rb_app_##TYPENAME##_response_write_status, 1);         \
    rb_define_method(STATIC_VAR_NAME, "end", uws_rb_app_##TYPENAME##_response_end, 1);                           \
    rb_define_method(STATIC_VAR_NAME, "pause", uws_rb_app_##TYPENAME##_response_pause, 0);                       \
    rb_define_method(STATIC_VAR_NAME, "resume", uws_rb_app_##TYPENAME##_response_resume, 0);                     \
    rb_define_method(STATIC_VAR_NAME, "write_continue", uws_rb_app_##TYPENAME##_response_write_continue, 0);     \
    rb_define_method(STATIC_VAR_NAME, "end_without_body", uws_rb_app_##TYPENAME##_response_end_without_body, 0); \
    rb_define_method(STATIC_VAR_NAME, "has_responded?", uws_rb_app_##TYPENAME##_response_has_responded, 0);      \
    rb_define_method(STATIC_VAR_NAME, "get_write_offset", uws_rb_app_##TYPENAME##_response_get_write_offset, 0); \
    rb_define_method(STATIC_VAR_NAME, "on_aborted", uws_rb_app_##TYPENAME##_response_on_aborted, 1);             \
    rb_define_method(STATIC_VAR_NAME, "on_data", uws_rb_app_##TYPENAME##_response_on_data, 1);                   \
    rb_define_method(STATIC_VAR_NAME, "on_writable", uws_rb_app_##TYPENAME##_response_on_writable, 1);           \
    rb_define_method(STATIC_VAR_NAME, "upgrade", uws_rb_app_##TYPENAME##_response_upgrade, 5);

#define uws_response(TYPE, TYPENAME)                                                                                                                                                  \
    static VALUE uws_rb_app_##TYPENAME##_response_new(VALUE klass)                                                                                                                    \
    {                                                                                                                                                                                 \
        uws_rb_app_response_t *response;                                                                                                                                              \
        VALUE obj = Data_Make_Struct(klass, uws_rb_app_response_t, NULL, free, response);                                                                                             \
        response->ptr = NULL;                                                                                                                                                         \
        return obj;                                                                                                                                                                   \
    }                                                                                                                                                                                 \
    static VALUE uws_rb_app_##TYPENAME##_write_header(VALUE self, VALUE header, VALUE value)                                                                                          \
    {                                                                                                                                                                                 \
        if (!RB_TYPE_P(header, T_STRING) || !RB_TYPE_P(value, T_STRING))                                                                                                              \
        {                                                                                                                                                                             \
            rb_raise(rb_eRuntimeError, "header name is required to be a String in AppResponse.write_header");                                                                         \
            return self;                                                                                                                                                              \
        }                                                                                                                                                                             \
        uws_rb_app_response_t *response;                                                                                                                                              \
        Data_Get_Struct(self, uws_rb_app_response_t, response);                                                                                                                       \
        if (RB_TYPE_P(value, T_STRING))                                                                                                                                               \
        {                                                                                                                                                                             \
            uws_res_write_header(TYPE, response->ptr, RSTRING_PTR(header), RSTRING_LEN(header), RSTRING_PTR(value), RSTRING_LEN(value));                                              \
            RB_GC_GUARD(header);                                                                                                                                                      \
            RB_GC_GUARD(value);                                                                                                                                                       \
            return self;                                                                                                                                                              \
        }                                                                                                                                                                             \
        if (RB_TYPE_P(value, T_FIXNUM))                                                                                                                                               \
        {                                                                                                                                                                             \
            int c_value = RB_NUM2INT(value);                                                                                                                                          \
            if (c_value < 0)                                                                                                                                                          \
            {                                                                                                                                                                         \
                rb_raise(rb_eRuntimeError, "value is required to be a String or Positive Integer in AppResponse.write_header");                                                       \
                return self;                                                                                                                                                          \
            }                                                                                                                                                                         \
            uws_res_write_header_int(TYPE, response->ptr, RSTRING_PTR(header), RSTRING_LEN(header), c_value);                                                                         \
            RB_GC_GUARD(header);                                                                                                                                                      \
            return self;                                                                                                                                                              \
        }                                                                                                                                                                             \
        rb_raise(rb_eRuntimeError, "value is required to be a String or Positive Integer in AppResponse.write_header");                                                               \
        return self;                                                                                                                                                                  \
    }                                                                                                                                                                                 \
    static VALUE uws_rb_app_##TYPENAME##_response_end(VALUE self, VALUE message)                                                                                                      \
    {                                                                                                                                                                                 \
        uws_rb_app_response_t *response;                                                                                                                                              \
        Data_Get_Struct(self, uws_rb_app_response_t, response);                                                                                                                       \
        if (RB_TYPE_P(message, T_STRING))                                                                                                                                             \
        {                                                                                                                                                                             \
            uws_res_end(TYPE, response->ptr, RSTRING_PTR(message), RSTRING_LEN(message), false);                                                                                      \
            RB_GC_GUARD(message);                                                                                                                                                     \
        }                                                                                                                                                                             \
        return self;                                                                                                                                                                  \
    }                                                                                                                                                                                 \
    static VALUE uws_rb_app_##TYPENAME##_response_write(VALUE self, VALUE message)                                                                                                    \
    {                                                                                                                                                                                 \
        uws_rb_app_response_t *response;                                                                                                                                              \
        Data_Get_Struct(self, uws_rb_app_response_t, response);                                                                                                                       \
        if (RB_TYPE_P(message, T_STRING))                                                                                                                                             \
        {                                                                                                                                                                             \
            int result = uws_res_write(TYPE, response->ptr, RSTRING_PTR(message), RSTRING_LEN(message));                                                                              \
            RB_GC_GUARD(message);                                                                                                                                                     \
            return result ? Qtrue : Qfalse;                                                                                                                                           \
        }                                                                                                                                                                             \
        return Qfalse;                                                                                                                                                                \
    }                                                                                                                                                                                 \
    static VALUE uws_rb_app_##TYPENAME##_response_write_status(VALUE self, VALUE status)                                                                                              \
    {                                                                                                                                                                                 \
        uws_rb_app_response_t *response;                                                                                                                                              \
        Data_Get_Struct(self, uws_rb_app_response_t, response);                                                                                                                       \
        if (RB_TYPE_P(status, T_STRING))                                                                                                                                              \
        {                                                                                                                                                                             \
            uws_res_write_status(TYPE, response->ptr, RSTRING_PTR(status), RSTRING_LEN(status));                                                                                      \
            RB_GC_GUARD(status);                                                                                                                                                      \
        }                                                                                                                                                                             \
        return self;                                                                                                                                                                  \
    }                                                                                                                                                                                 \
    static VALUE uws_rb_app_##TYPENAME##_response_pause(VALUE self)                                                                                                                   \
    {                                                                                                                                                                                 \
        uws_rb_app_response_t *response;                                                                                                                                              \
        Data_Get_Struct(self, uws_rb_app_response_t, response);                                                                                                                       \
        uws_res_pause(TYPE, response->ptr);                                                                                                                                           \
        return self;                                                                                                                                                                  \
    }                                                                                                                                                                                 \
    static VALUE uws_rb_app_##TYPENAME##_response_resume(VALUE self)                                                                                                                  \
    {                                                                                                                                                                                 \
        uws_rb_app_response_t *response;                                                                                                                                              \
        Data_Get_Struct(self, uws_rb_app_response_t, response);                                                                                                                       \
        uws_res_resume(TYPE, response->ptr);                                                                                                                                          \
        return self;                                                                                                                                                                  \
    }                                                                                                                                                                                 \
    static VALUE uws_rb_app_##TYPENAME##_response_write_continue(VALUE self)                                                                                                          \
    {                                                                                                                                                                                 \
        uws_rb_app_response_t *response;                                                                                                                                              \
        Data_Get_Struct(self, uws_rb_app_response_t, response);                                                                                                                       \
        uws_res_write_continue(TYPE, response->ptr);                                                                                                                                  \
        return self;                                                                                                                                                                  \
    }                                                                                                                                                                                 \
    static VALUE uws_rb_app_##TYPENAME##_response_end_without_body(VALUE self)                                                                                                        \
    {                                                                                                                                                                                 \
        uws_rb_app_response_t *response;                                                                                                                                              \
        Data_Get_Struct(self, uws_rb_app_response_t, response);                                                                                                                       \
        uws_res_end_without_body(TYPE, response->ptr, false);                                                                                                                                \
        return self;                                                                                                                                                                  \
    }                                                                                                                                                                                 \
    static VALUE uws_rb_app_##TYPENAME##_response_get_write_offset(VALUE self)                                                                                                        \
    {                                                                                                                                                                                 \
        uws_rb_app_response_t *response;                                                                                                                                              \
        Data_Get_Struct(self, uws_rb_app_response_t, response);                                                                                                                       \
        uintmax_t offset = uws_res_get_write_offset(TYPE, response->ptr);                                                                                                             \
        VALUE rb_offset = ULL2NUM(offset);                                                                                                                                            \
        return rb_offset;                                                                                                                                                             \
    }                                                                                                                                                                                 \
    static VALUE uws_rb_app_##TYPENAME##_response_has_responded(VALUE self)                                                                                                           \
    {                                                                                                                                                                                 \
        uws_rb_app_response_t *response;                                                                                                                                              \
        Data_Get_Struct(self, uws_rb_app_response_t, response);                                                                                                                       \
        return uws_res_has_responded(TYPE, response->ptr) ? Qtrue : Qfalse;                                                                                                           \
    }                                                                                                                                                                                 \
    static VALUE uws_rb_app_##TYPENAME##_response_on_aborted(VALUE self, VALUE callback)                                                                                              \
    {                                                                                                                                                                                 \
        uws_rb_app_response_t *response;                                                                                                                                              \
        Data_Get_Struct(self, uws_rb_app_response_t, response);                                                                                                                       \
        if (!rb_obj_is_proc(callback))                                                                                                                                                \
        {                                                                                                                                                                             \
            rb_raise(rb_eRuntimeError, "Callback needs to be a proc without arguments in AppResponse.on_aborted");                                                                    \
            return self;                                                                                                                                                              \
        }                                                                                                                                                                             \
        uws_rb_callback_t *rb_callback = (uws_rb_callback_t *)malloc(sizeof(uws_rb_callback_t));                                                                                      \
        rb_callback->value = callback;                                                                                                                                                \
        uws_res_on_aborted(TYPE, response->ptr, uws_rb_response_generic_aborted_handler, rb_callback);                                                                                \
        return self;                                                                                                                                                                  \
    }                                                                                                                                                                                 \
    static VALUE uws_rb_app_##TYPENAME##_response_on_data(VALUE self, VALUE callback)                                                                                                 \
    {                                                                                                                                                                                 \
        uws_rb_app_response_t *response;                                                                                                                                              \
        Data_Get_Struct(self, uws_rb_app_response_t, response);                                                                                                                       \
        if (!rb_obj_is_proc(callback))                                                                                                                                                \
        {                                                                                                                                                                             \
            rb_raise(rb_eRuntimeError, "Callback needs to be a proc with 2 arguments in AppResponse.on_data");                                                                        \
            return self;                                                                                                                                                              \
        }                                                                                                                                                                             \
        uws_rb_callback_t *rb_callback = (uws_rb_callback_t *)malloc(sizeof(uws_rb_callback_t));                                                                                      \
        rb_callback->value = callback;                                                                                                                                                \
        uws_res_on_data(TYPE, response->ptr, uws_rb_response_generic_data_handler, rb_callback);                                                                                      \
        return self;                                                                                                                                                                  \
    }                                                                                                                                                                                 \
    static VALUE uws_rb_app_##TYPENAME##_response_on_writable(VALUE self, VALUE callback)                                                                                             \
    {                                                                                                                                                                                 \
        uws_rb_app_response_t *response;                                                                                                                                              \
        Data_Get_Struct(self, uws_rb_app_response_t, response);                                                                                                                       \
        if (!rb_obj_is_proc(callback))                                                                                                                                                \
        {                                                                                                                                                                             \
            rb_raise(rb_eRuntimeError, "Callback needs to be a proc with 1 argument in AppResponse.on_writable");                                                                     \
            return self;                                                                                                                                                              \
        }                                                                                                                                                                             \
        uws_rb_callback_t *rb_callback = (uws_rb_callback_t *)malloc(sizeof(uws_rb_callback_t));                                                                                      \
        rb_callback->value = callback;                                                                                                                                                \
        uws_res_on_writable(TYPE, response->ptr, uws_rb_response_generic_writeble_handler, rb_callback);                                                                              \
        return self;                                                                                                                                                                  \
    }                                                                                                                                                                                 \
    static VALUE uws_rb_app_##TYPENAME##_response_upgrade(VALUE self, VALUE data, VALUE sec_web_socket_key, VALUE sec_web_socket_protocol, VALUE sec_web_socket_extensions, VALUE ws) \
    {                                                                                                                                                                                 \
        return self;                                                                                                                                                                  \
    }

#endif