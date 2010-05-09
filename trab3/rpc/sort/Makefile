all:client server

client:client.c sort.h
	gcc -o client client.c sort_clnt.c sort_xdr.c -lrpcsvc

server:server.c sort.h
	gcc -o server server.c sort_svc.c sort_xdr.c -lrpcsvc

sort.h:sort.x
	rpcgen sort.x

clean:
	rm -f client server sort.h sort_clnt.c sort_xdr.c sort_svc.c
