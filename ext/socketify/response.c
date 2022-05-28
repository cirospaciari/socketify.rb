#include "./response.h"

bool uws_rb_response_generic_writeble_handler(uws_res_t *res, uintmax_t value, void *user_data){
  uws_rb_callback_t *info = ((uws_rb_callback_t *)user_data);
  VALUE callback = info->value;
  //free callback data
  free(user_data);

  VALUE rb_value = ULL2NUM(value);

  VALUE result = rb_funcall(callback, rb_intern("call"), 1, rb_value);
  RB_GC_GUARD(callback);
  return RB_TYPE_P(result, T_TRUE) ? 1 : 0;
}

void uws_rb_response_generic_aborted_handler(uws_res_t *res, void *user_data){
 uws_rb_callback_t *info = ((uws_rb_callback_t *)user_data);
  VALUE callback = info->value;
  //free callback data
  free(user_data);

  rb_funcall(callback, rb_intern("call"), 0);
  RB_GC_GUARD(callback);
}

void uws_rb_response_generic_data_handler(uws_res_t *res, const char *chunk, size_t chunk_length, bool is_end, void *opcional_data){
  uws_rb_callback_t *info = ((uws_rb_callback_t *)opcional_data);
  VALUE callback = info->value;
  //free callback data
  free(opcional_data);

  VALUE rb_chunk = Qnil;
  if(chunk && chunk_length > 0){
      rb_chunk = rb_str_new(chunk, chunk_length);
  }
  VALUE rb_is_end = is_end ? Qtrue : Qfalse;
    
  rb_funcall(callback, rb_intern("call"), 2, rb_chunk, rb_is_end);
  RB_GC_GUARD(callback);
  RB_GC_GUARD(rb_chunk);
}