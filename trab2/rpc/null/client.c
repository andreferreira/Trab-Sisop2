
/*
 * Client side (written by user)
 *
 * compile with (after rpcgen):
 *        gcc -o client client.c square_clnt.c square_xdr.c -lrpcsvc
 */
#include <time.h>
#include	"null.h"
#define ITERMAX 5000

int main(int argc, char **argv) {
    CLIENT *cl;     		/* client handle */

    if (argc != 2 ) {
       printf("usage: client <hostname>");
       exit(0);
    }

    cl = clnt_create(argv[1], SORT_PROG, SORT_VERS, "tcp");
	clock_t start, end;

	int n;
	start = clock();
	for(n=0; n<ITERMAX; n++){
		/*cl = clnt_create(argv[1], SORT_PROG, SORT_VERS, "tcp"); /* erro de timeout sem o destroy, segfault com*/
		if ( (nilproc_1(NULL,cl)) == NULL) {
		   printf("%s", clnt_sperror(cl, argv[1]));
		   exit(0);
		}
		/*clnt_destroy(cl); /*segmentation fault*/
	}
	end = clock();
    
   /* Print out the difference */
	printf ( "%f\n",
		(double)( end - start ) / (double)CLOCKS_PER_SEC);
    exit(0);
}



