/*
 * RPC program named NULL_PROC that consists of one version (NULL_VERS)
 * and in that version is a single procedure named NULLPROC with no input
 * or output.
 *
 * compile with rpcgen. It generates
 *	null_clnt.c  null.h  null_svc.c null_xdr.c
 */

program NIL_PROG{
    version NIL_VERS {
        void NILPROC(void) = 1; /* procedure number 1 */
    } = 1; 	/*version number */
} = 0x31230001; /*program number: 32-bits hexadecimal ranges defined */
		/*by sun: 0x20000000-0x3FFFFFFF for user defined     */
   
