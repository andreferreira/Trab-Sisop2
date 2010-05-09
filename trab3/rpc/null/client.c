
/*
 * Client side (written by user)
 *
 * compile with (after rpcgen):
 *        gcc -o client client.c square_clnt.c square_xdr.c -lrpcsvc
 */

#include	"null.h"

int main(int argc, char **argv) {
    CLIENT *cl;     		/* client handle */

    if (argc != 2 ) {
       printf("usage: client <hostname>");
       exit(0);
    }

    cl = clnt_create(argv[1], NIL_PROG, NIL_VERS, "tcp");

    if ( (nilproc_1(NULL,cl)) == NULL) {
       printf("%s", clnt_sperror(cl, argv[1]));
       exit(0);
    }

    printf("done\n");
    exit(0);
}



