/*
 * Server side (written by user): procedure name is that given on null.x
 * appended version number (_1) and _svc in this order. This function prototype
 * is used on files generated by rpcgen.
 *
 * compile with gcc (after rpcgen):
 *      gcc -o server server.c square_svc.c square_xdr.c -lrpcsvc
 */

#include	"null.h"

void* nilproc_1_svc(void* args,struct svc_req *rqstp) {
    printf("received\n");
    return;
}
