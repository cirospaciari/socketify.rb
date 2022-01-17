#include "./request.h"

static VALUE uws_rb_app_request_new(VALUE klass)
{
    uws_rb_app_request_t *response;
    VALUE obj = Data_Make_Struct(klass, uws_rb_app_request_t, NULL, free, response);
    response->ptr = NULL;
    return obj;
}

static VALUE uws_rb_req_is_ancient(VALUE self)
{
    uws_rb_app_request_t *response;
    Data_Get_Struct(self, uws_rb_app_request_t, response);

    return uws_req_is_ancient(response->ptr) ? Qtrue : Qfalse;
}

static VALUE uws_rb_req_get_yield(VALUE self)
{
    uws_rb_app_request_t *response;
    Data_Get_Struct(self, uws_rb_app_request_t, response);

    return uws_req_get_yield(response->ptr) ? Qtrue : Qfalse;
}

static VALUE uws_rb_req_set_yield(VALUE self, VALUE yield)
{
    uws_rb_app_request_t *response;
    Data_Get_Struct(self, uws_rb_app_request_t, response);

    uws_req_set_field(response->ptr, RB_TYPE_P(yield, T_TRUE) ? 1 : 0);
    return self;
}

static VALUE uws_rb_req_get_url(VALUE self)
{
    uws_rb_app_request_t *response;
    Data_Get_Struct(self, uws_rb_app_request_t, response);

    const char *value;
    size_t length = uws_req_get_url(response->ptr, &value);

    VALUE result = length > 0 ? rb_str_new(value, length) : Qnil;
    return result;
}

static VALUE uws_rb_req_get_method(VALUE self)
{
    uws_rb_app_request_t *response;
    Data_Get_Struct(self, uws_rb_app_request_t, response);

    const char *value;
    size_t length = uws_req_get_method(response->ptr, &value);

    VALUE result = length > 0 ? rb_str_new(value, length) : Qnil;
    return result;
}

static VALUE uws_rb_req_get_header(VALUE self, VALUE lower_case_header)
{
    if (!RB_TYPE_P(lower_case_header, T_STRING))
    {
        rb_raise(rb_eRuntimeError, "header name are required to be a non empty String in AppRequest.get_header");
        return Qnil;
    }
    size_t lower_case_header_length = RSTRING_LEN(lower_case_header);
    if (lower_case_header_length == 0)
    {
        rb_raise(rb_eRuntimeError, "header name are required to be a non empty String in AppRequest.get_header");
        return Qnil;
    }

    uws_rb_app_request_t *response;
    Data_Get_Struct(self, uws_rb_app_request_t, response);

    const char *value;
    const char *header = RSTRING_PTR(lower_case_header);
    size_t length = uws_req_get_header(response->ptr, header, lower_case_header_length, &value);
    RB_GC_GUARD(lower_case_header);
    VALUE result = length > 0 ? rb_str_new(value, length) : Qnil;
    return result;
}

static VALUE uws_rb_req_get_query(VALUE self, VALUE query_parameter)
{
    if (!RB_TYPE_P(query_parameter, T_STRING))
    {
        rb_raise(rb_eRuntimeError, "query parameter are required to be a non empty String in AppRequest.get_query");
        return Qnil;
    }
    size_t query_length = RSTRING_LEN(query_parameter);
    if (query_length == 0)
    {
        rb_raise(rb_eRuntimeError, "query parameter are required to be a non empty String in AppRequest.get_query");
        return Qnil;
    }

    uws_rb_app_request_t *response;
    Data_Get_Struct(self, uws_rb_app_request_t, response);

    const char *value;
    const char *query = RSTRING_PTR(query_parameter);
    size_t length = uws_req_get_query(response->ptr, query, query_length, &value);
    RB_GC_GUARD(query_parameter);
    VALUE result = length > 0 ? rb_str_new(value, length) : Qnil;
    return result;
}

static VALUE uws_rb_req_get_parameter(VALUE self, VALUE parameter_index)
{
    if (!RB_TYPE_P(parameter_index, T_FIXNUM))
    {
        rb_raise(rb_eRuntimeError, "Parameter index are required to be a positive Integer AppRequest.get_parameter");
        return Qnil;
    }
    int index = RB_NUM2INT(parameter_index);

    if (index < 0)
    {
        rb_raise(rb_eRuntimeError, "Parameter index are required to be a positive Integer AppRequest.get_parameter");
        return Qnil;
    }

    uws_rb_app_request_t *response;
    Data_Get_Struct(self, uws_rb_app_request_t, response);

    const char *value;
    size_t length = uws_req_get_parameter(response->ptr, index, &value);
    VALUE result = length > 0 ? rb_str_new(value, length) : Qnil;
    return result;
}

VALUE Init_uws_request(VALUE UWS_Module)
{
    VALUE UWS_AppRequest;
    UWS_AppRequest = rb_define_class_under(UWS_Module, "AppRequest", rb_cObject);
    rb_define_alloc_func(UWS_AppRequest, uws_rb_app_request_new);
    rb_define_method(UWS_AppRequest, "is_ancient?", uws_rb_req_is_ancient, 0);
    rb_define_method(UWS_AppRequest, "get_yield?", uws_rb_req_get_yield, 0);
    rb_define_method(UWS_AppRequest, "set_yield", uws_rb_req_set_yield, 1);
    rb_define_method(UWS_AppRequest, "get_url", uws_rb_req_get_url, 0);
    rb_define_method(UWS_AppRequest, "get_method", uws_rb_req_get_method, 0);
    rb_define_method(UWS_AppRequest, "get_header", uws_rb_req_get_header, 1);
    rb_define_method(UWS_AppRequest, "get_query", uws_rb_req_get_query, 1);
    rb_define_method(UWS_AppRequest, "get_parameter", uws_rb_req_get_parameter, 1);

    return UWS_AppRequest;
}