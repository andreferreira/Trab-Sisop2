
/*
 * Client side (written by user)
 *
 * compile with (after rpcgen):
 *        gcc -o client client.c sort_clnt.c sort_xdr.c -lrpcsvc
 */

#include	"sort.h"

int main(int argc, char **argv) {
    CLIENT *cl;     		/* client handle */
    sort_in in;		
    sort_out *outp;

    if (argc != 2 ) {
       printf("usage: client <hostname>");
       exit(0);
    }

    cl = clnt_create(argv[1], SORT_PROG, SORT_VERS, "tcp");

	int i;
	for (i=0;i<250;i++)
		in.arg1[i] = i;
    if ( (outp = sortproc_1(&in, cl)) == NULL) {
       printf("%s", clnt_sperror(cl, argv[1]));
       exit(0);
    }
	
	for (i=0;i<250;i++)
		printf("%d\n", outp->res1[i]);
    exit(0);
}



