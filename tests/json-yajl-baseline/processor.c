/* Test-only observer for the unchanged ModSecurity v2 JSON processor.
 * This supplements Apache integration with controlled actual parser chunks.
 * Output is hex so NUL/invalid UTF-8 and duplicate entries remain lossless.
 */
#include "msc_json.h"
#include <stdio.h>
#include <stdlib.h>

void msr_log(modsec_rec *msr, int level, const char *fmt, ...) {
    (void) msr; (void) level; (void) fmt;
}
static void hex(const char *s, size_t n) {
    size_t i; for (i=0;i<n;i++) printf("%02x",(unsigned char)s[i]);
}
int main(int argc, char **argv) {
    apr_pool_t *pool; modsec_rec msr; directory_config cfg;
    char *error=NULL; unsigned char *body; size_t n=0,cap=1024,step,off;
    int parse=1,complete; const apr_array_header_t *arr; apr_table_entry_t *entries; int i;
    if(argc!=4) return 2;
    step=(size_t)strtoul(argv[1],NULL,10); if(step==0) return 2;
    body=malloc(cap); if(!body) return 2;
    for (;;) { int ch=getchar(); if(ch==EOF) break; if(n==cap) {unsigned char *next;cap*=2; if(cap>1048576)return 2;next=realloc(body,cap);if(!next)return 2;body=next;}body[n++]=(unsigned char)ch; }
    if(apr_initialize()!=APR_SUCCESS)return 2;
    if(apr_pool_create(&pool,NULL)!=APR_SUCCESS)return 2;
    memset(&msr,0,sizeof(msr));memset(&cfg,0,sizeof(cfg));
    msr.mp=pool;msr.txcfg=&cfg;msr.arguments=apr_table_make(pool,16);
    cfg.reqbody_json_depth_limit=strtol(argv[2],NULL,10);cfg.arguments_limit=strtol(argv[3],NULL,10);
    if(json_init(&msr,&error)!=1)return 2;
    for(off=0;off<n;off+=step) {size_t len=n-off<step?n-off:step;parse=json_process_chunk(&msr,(char*)body+off,(unsigned int)len,&error);if(parse<0)break;}
    complete=parse<0?0:json_complete(&msr,&error);
    printf("state %d %d %d %d\n",parse,complete,msr.msc_reqbody_error,msr.json->depth_limit_exceeded);
    arr=apr_table_elts(msr.arguments);entries=(apr_table_entry_t*)arr->elts;
    for(i=0;i<arr->nelts;i++) {msc_arg *arg=(msc_arg*)entries[i].val;printf("arg ");hex(arg->name,arg->name_len);printf(" ");hex(arg->value,arg->value_len);printf("\n");}
    json_cleanup(&msr);apr_pool_destroy(pool);apr_terminate();free(body);return 0;
}
