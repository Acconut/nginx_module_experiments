#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>

ngx_str_t audio_element = ngx_string(
  "<audio controls loop autoplay src=\"https://upload.wikimedia.org/wikipedia/commons/8/85/Holst-_mars.ogg\"></audio>"
);

static char *
ngx_http_inject_audio(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
  return NGX_OK;
}
static ngx_int_t ngx_http_inject_audio_init(ngx_conf_t *cf);

static ngx_command_t  ngx_http_inject_audio_commands[] = {

  { ngx_string("inject_audio"),
  NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_NOARGS,
  ngx_http_inject_audio,
  0,
  0,
  NULL },

  ngx_null_command
};


static ngx_http_module_t  ngx_http_inject_audio_module_ctx = {
  NULL,                                         /* proconfiguration */
  ngx_http_inject_audio_init,               /* postconfiguration */

  NULL,                                         /* create main configuration */
  NULL,                                         /* init main configuration */

  NULL,                                         /* create server configuration */
  NULL,                                         /* merge server configuration */

  NULL,                         /* create location configuration */
  NULL                          /* merge location configuration */
};




ngx_module_t  ngx_http_inject_audio_module = {
  NGX_MODULE_V1,
  &ngx_http_inject_audio_module_ctx, /* module context */
  ngx_http_inject_audio_commands,    /* module directives */
  NGX_HTTP_MODULE,                       /* module type */
  NULL,                                  /* init master */
  NULL,                                  /* init module */
  NULL,                                  /* init process */
  NULL,                                  /* init thread */
  NULL,                                  /* exit thread */
  NULL,                                  /* exit process */
  NULL,                                  /* exit master */
  NGX_MODULE_V1_PADDING
};

static ngx_http_output_header_filter_pt ngx_http_next_header_filter;
static ngx_http_output_body_filter_pt   ngx_http_next_body_filter;

static ngx_int_t
ngx_http_inject_audio_header_filter(ngx_http_request_t *r)
{
  r->headers_out.content_length_n += audio_element.len;

  return ngx_http_next_header_filter(r);
}


static ngx_int_t
ngx_http_inject_audio_body_filter(ngx_http_request_t *r, ngx_chain_t *in)
{

  ngx_buf_t             *buf;
  ngx_chain_t           *link;

  buf = ngx_calloc_buf(r->pool);

  buf->pos = audio_element.data;
  buf->last = buf->pos + audio_element.len;
  buf->start = buf->pos;
  buf->end = buf->last;
  buf->last_buf = 1;
  buf->memory = 1;

  link = ngx_alloc_chain_link(r->pool);

  link->buf = buf;
  link->next = NULL;

  // Find the last chain and attach our buffer there.
  while(in->next != NULL) in = in->next;
  in->buf->last_buf = 0;
  in->next = link;

  return ngx_http_next_body_filter(r, in);
}

static ngx_int_t
ngx_http_inject_audio_init(ngx_conf_t *cf)
{
  ngx_http_next_body_filter = ngx_http_top_body_filter;
  ngx_http_top_body_filter = ngx_http_inject_audio_body_filter;

  ngx_http_next_header_filter = ngx_http_top_header_filter;
  ngx_http_top_header_filter = ngx_http_inject_audio_header_filter;

  return NGX_OK;
}
