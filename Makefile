
shell: shell.o
	clang++-9 -std=c++2a -Werror -lpthread shell.o -o shell

shell.o: shell.cpp
	clang++-9 -std=c++2a -Werror --compile -g3 shell.cpp -o shell.o 

server: server.o
	clang++-9 -std=c++2a -Werror server.o -o server

server.o: server.cpp
	clang++-9 -std=c++2a -Werror --compile -g3 server.cpp -o server.o