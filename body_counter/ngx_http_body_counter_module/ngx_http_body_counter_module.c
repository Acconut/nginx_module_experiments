#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>

typedef struct {
    off_t  count;
} ngx_http_body_counter_filter_ctx_t;


static ngx_int_t ngx_http_body_counter_body_filter(ngx_http_request_t *r,
    ngx_chain_t *in);
static ngx_int_t ngx_http_body_counter_variable(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data);
static ngx_int_t ngx_http_body_counter_add_variables(ngx_conf_t *cf);
static ngx_int_t ngx_http_body_counter_filter_init(ngx_conf_t *cf);


static ngx_http_module_t  ngx_http_body_counter_module_ctx = {
    ngx_http_body_counter_add_variables,        /* preconfiguration */
    ngx_http_body_counter_filter_init,          /* postconfiguration */

    NULL,                                  /* create main configuration */
    NULL,                                  /* init main configuration */

    NULL,                                  /* create server configuration */
    NULL,                                  /* merge server configuration */

    NULL,                                  /* create location configuration */
    NULL                                   /* merge location configuration */
};


ngx_module_t  ngx_http_body_counter_module = {
    NGX_MODULE_V1,
    &ngx_http_body_counter_module_ctx,   /* module context */
    NULL,                                  /* module directives */
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


static ngx_http_output_body_filter_pt  ngx_http_next_request_body_filter;

static ngx_str_t  ngx_http_body_counter_name = ngx_string("counter");


static ngx_int_t
ngx_http_body_counter_body_filter(ngx_http_request_t *r, ngx_chain_t *in)
{
    ngx_chain_t                    *cl;
    ngx_http_body_counter_filter_ctx_t  *ctx;

    ctx = ngx_http_get_module_ctx(r, ngx_http_body_counter_module);
    if (ctx == NULL) {
        ctx = ngx_pcalloc(r->pool, sizeof(ngx_http_body_counter_filter_ctx_t));
        if (ctx == NULL) {
            return NGX_ERROR;
        }

        ngx_http_set_ctx(r, ctx, ngx_http_body_counter_module);
    }

    for (cl = in; cl; cl = cl->next) {
        ctx->count += ngx_buf_size(cl->buf);
    }

    printf("Count: %lld\n", ctx->count);

    return ngx_http_next_request_body_filter(r, in);
}


static ngx_int_t
ngx_http_body_counter_variable(ngx_http_request_t *r, ngx_http_variable_value_t *v,
    uintptr_t data)
{
    u_char                         *p;
    ngx_http_body_counter_filter_ctx_t  *ctx;

    ctx = ngx_http_get_module_ctx(r, ngx_http_body_counter_module);
    if (ctx == NULL) {
        v->not_found = 1;
        return NGX_OK;
    }

    p = ngx_pnalloc(r->pool, NGX_OFF_T_LEN);
    if (p == NULL) {
        return NGX_ERROR;
    }

    v->data = p;
    v->len = ngx_sprintf(p, "%O", ctx->count) - p;
    v->valid = 1;
    v->no_cacheable = 0;
    v->not_found = 0;

    return NGX_OK;
}


static ngx_int_t
ngx_http_body_counter_add_variables(ngx_conf_t *cf)
{
    ngx_http_variable_t  *var;

    var = ngx_http_add_variable(cf, &ngx_http_body_counter_name, 0);
    if (var == NULL) {
        return NGX_ERROR;
    }

    var->get_handler = ngx_http_body_counter_variable;

    return NGX_OK;
}


static ngx_int_t
ngx_http_body_counter_filter_init(ngx_conf_t *cf)
{
    ngx_http_next_request_body_filter = ngx_http_top_request_body_filter;
    ngx_http_top_request_body_filter = ngx_http_body_counter_body_filter;

    return NGX_OK;
}
