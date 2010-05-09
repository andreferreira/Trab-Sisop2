all:client server

client:client.c null.h
	gcc -o client client.c null_clnt.c -lrpcsvc

server:server.c null.h
	gcc -o server server.c null_svc.c -lrpcsvc

null.h:null.x
	rpcgen null.x

clean:
	rm -f client server null.h null_clnt.c null_xdr.c null_svc.c
