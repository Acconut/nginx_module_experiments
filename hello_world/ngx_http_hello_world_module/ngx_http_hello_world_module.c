
#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>
#include <ngx_string.h>

static char *ngx_http_hello_world(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);

static ngx_command_t  ngx_http_hello_world_commands[] = {
  {
    ngx_string("print_hello_world"),
    NGX_HTTP_LOC_CONF|NGX_CONF_NOARGS,
    ngx_http_hello_world,
    0,
    0,
    NULL
  },
    ngx_null_command
};

static ngx_http_module_t  ngx_http_hello_world_module_ctx = {
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL
};

ngx_module_t ngx_http_hello_world_module = {
  NGX_MODULE_V1,
  &ngx_http_hello_world_module_ctx,
  ngx_http_hello_world_commands,
  NGX_HTTP_MODULE,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NGX_MODULE_V1_PADDING
};

static ngx_int_t ngx_http_hello_world_handler(ngx_http_request_t *r)
{
  ngx_str_t content = ngx_string("Hello! The request arguments are:\n");
  size_t sz = content.len + r->args.len;

  ngx_str_t content_type = ngx_string("text/plain");

  r->headers_out.content_type = content_type;
  r->headers_out.status = NGX_HTTP_OK;
  r->headers_out.content_length_n = sz;
  ngx_http_send_header(r);

  ngx_buf_t    *b, *b2;
  ngx_chain_t  *out, *out2;

  b = ngx_calloc_buf(r->pool);
  out = ngx_alloc_chain_link(r->pool);

  out->buf = b;
  out->next = NULL;

  b->pos = content.data;
  b->last = b->pos + content.len;
  b->memory = 1;
  b->last_buf = 1;

  // Only append the request arguments if they are non-zero. Nginx does not like
  // empty buffers and would log: "zero size buf in output"
  if (r->args.len > 0) {
    b2 = ngx_calloc_buf(r->pool);
    out2 = ngx_alloc_chain_link(r->pool);

    b->last_buf = 0;
    out->next = out2;

    out2->buf = b2; 
    out2->next = NULL;

    b2->pos = r->args.data;
    b2->last = r->args.data + r->args.len;
    b2->memory = 1;
    b2->last_buf = 1;
  }

  return ngx_http_output_filter(r, out);
}

static char *ngx_http_hello_world(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
  ngx_http_core_loc_conf_t  *clcf;
  clcf = ngx_http_conf_get_module_loc_conf(cf, ngx_http_core_module);
  clcf->handler = ngx_http_hello_world_handler;
  return NGX_CONF_OK;
}
