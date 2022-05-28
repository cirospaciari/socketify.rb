#include "./socketify.h"
#include "./response.h"
#include "./app.h"
#include "./request.h"

static VALUE UWS_AppResponse;
static VALUE UWS_AppSSLResponse;
static VALUE UWS_AppRequest;

//statically implements responses
uws_response(UWS_DEFAULT_APP, default);
uws_response(UWS_SSL_APP, ssl);

//statically implements default app
uws_app(UWS_DEFAULT_APP, default, UWS_AppResponse, UWS_AppRequest);
uws_app_http_method(UWS_DEFAULT_APP, default, get);
uws_app_http_method(UWS_DEFAULT_APP, default, post);
uws_app_http_method(UWS_DEFAULT_APP, default, options);
uws_app_http_method(UWS_DEFAULT_APP, default, delete);
uws_app_http_method(UWS_DEFAULT_APP, default, patch);
uws_app_http_method(UWS_DEFAULT_APP, default, put);
uws_app_http_method(UWS_DEFAULT_APP, default, head);
uws_app_http_method(UWS_DEFAULT_APP, default, connect);
uws_app_http_method(UWS_DEFAULT_APP, default, trace);
uws_app_http_method(UWS_DEFAULT_APP, default, any);
//statically implement ssl app
uws_app(UWS_SSL_APP, ssl, UWS_AppSSLResponse, UWS_AppRequest);
uws_app_http_method(UWS_SSL_APP, ssl, get);
uws_app_http_method(UWS_SSL_APP, ssl, post);
uws_app_http_method(UWS_SSL_APP, ssl, options);
uws_app_http_method(UWS_SSL_APP, ssl, delete);
uws_app_http_method(UWS_SSL_APP, ssl, patch);
uws_app_http_method(UWS_SSL_APP, ssl, put);
uws_app_http_method(UWS_SSL_APP, ssl, head);
uws_app_http_method(UWS_SSL_APP, ssl, connect);
uws_app_http_method(UWS_SSL_APP, ssl, trace);
uws_app_http_method(UWS_SSL_APP, ssl, any);

//Initialize UWS module for ruby
void Init_socketify(void)
{
  /* Ractor is still experimental so its disabled 
  #ifdef RB_EXT_RACTOR_SAFE
    RB_EXT_RACTOR_SAFE(true);
  #endif
  */

  VALUE UWS_App;
  VALUE UWS_SSLApp;
  VALUE UWS_Module;

  //init module
  UWS_Module = rb_define_module("Socketify");

  //init response
  uws_response_init(UWS_AppResponse, UWS_Module, default, "AppReponse");
  uws_response_init(UWS_AppSSLResponse, UWS_Module, ssl, "AppSSLReponse");
  //init request
  UWS_AppRequest = uws_request_init(UWS_Module);

  //init app
  uws_app_init(UWS_App, UWS_Module, default, "App", 0);
  uws_app_init(UWS_SSLApp, UWS_Module, ssl, "SSLApp", 1);
}