/*
 * RPC program named SORT_PROC that consists of one version (SORT_VERS)
 * and in that version is a single procedure named SORTPROC with an input
 * argument (struct square_in) and returns a value (struct square_out).
 *
 * compile with rpcgen. It generates
 *	square_clnt.c  square.h  square_svc.c  square.x  square_xdr.c
 */


struct sort_in {		/* input argument */
   int arg1[250];
};

struct sort_out {		/* output argument */
   int	res1[250];
};

program SORT_PROG{
    version SORT_VERS {
        sort_out SORTPROC(sort_in) = 1; /* procedure number 1 */
    } = 1; 	/*version number */
} = 0x31230002; /*program number: 32-bits hexadecimal ranges defined */
		/*by sun: 0x20000000-0x3FFFFFFF for user defined     */
   
