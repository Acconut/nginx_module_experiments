#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>

ngx_str_t audio_prefix = ngx_string(
  "<audio controls loop autoplay src=\"\"></audio>"
);
ngx_str_t audio_suffix = ngx_string(
  "\"></audio>"
);

static ngx_int_t ngx_http_inject_audio_init(ngx_conf_t *cf);

typedef struct {
    ngx_str_t  audio_url;
} ngx_http_inject_audio_conf_t;

static ngx_command_t  ngx_http_inject_audio_commands[] = {

  { ngx_string("inject_audio"),
  NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_TAKE1,
  ngx_conf_set_str_slot,
  NGX_HTTP_LOC_CONF_OFFSET,
  offsetof(ngx_http_inject_audio_conf_t, audio_url),
  NULL },

  ngx_null_command
};

static void *ngx_http_inject_audio_create_conf(ngx_conf_t *cf);
static char *ngx_http_inject_audio_merge_conf(ngx_conf_t *cf, void *parent, void *child);

static ngx_http_module_t  ngx_http_inject_audio_module_ctx = {
  NULL,                                         /* proconfiguration */
  ngx_http_inject_audio_init,                   /* postconfiguration */

  NULL,                                         /* create main configuration */
  NULL,                                         /* init main configuration */

  NULL,                                         /* create server configuration */
  NULL,                                         /* merge server configuration */

  ngx_http_inject_audio_create_conf,            /* create location configuration */
  ngx_http_inject_audio_merge_conf              /* merge location configuration */
};


static void *
ngx_http_inject_audio_create_conf(ngx_conf_t *cf)
{
    ngx_http_inject_audio_conf_t  *conf;

    conf = ngx_pcalloc(cf->pool, sizeof(ngx_http_inject_audio_conf_t));
    if (conf == NULL) {
        return NULL;
    }

    // pcalloc initializes everything to zero already.

    return conf;
}


static char *
ngx_http_inject_audio_merge_conf(ngx_conf_t *cf, void *parent, void *child)
{
    ngx_http_inject_audio_conf_t *prev = parent;
    ngx_http_inject_audio_conf_t *conf = child;

    ngx_conf_merge_str_value(conf->audio_url, prev->audio_url, "");

    return NGX_CONF_OK;
}

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
  ngx_http_inject_audio_conf_t  *cf;
  cf = ngx_http_get_module_loc_conf(r, ngx_http_inject_audio_module);

  size_t len = audio_prefix.len + cf->audio_url.len + audio_suffix.len;
  r->headers_out.content_length_n += len;

  return ngx_http_next_header_filter(r);
}

static ngx_int_t
ngx_http_inject_audio_body_filter(ngx_http_request_t *r, ngx_chain_t *in)
{
  ngx_http_inject_audio_conf_t *cf;
  cf = ngx_http_get_module_loc_conf(r, ngx_http_inject_audio_module);

  size_t audio_html_len = audio_prefix.len + cf->audio_url.len + audio_suffix.len;
  u_char      *audio_html = ngx_pnalloc(r->pool, audio_html_len);

  ngx_sprintf(audio_html, "%*s%*s%*s", audio_prefix, cf->audio_url, audio_suffix);

  ngx_buf_t             *buf;
  ngx_chain_t           *link;

  buf = ngx_calloc_buf(r->pool);

  buf->pos = audio_html;
  buf->last = buf->pos + audio_html_len;
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
