main : server.c client.c ncur.c
	clang server.c -o goodserver
	clang client.c -o badclient -lncurses
	clang ncur.c -o ncuruseDemo -lncurses
