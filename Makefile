
shell: shell.o
	clang++-9 -std=c++2a -Werror shell.o -o shell

shell.o: shell.cpp
	clang++-9 -std=c++2a -Werror --compile -g3 shell.cpp -o shell.o
