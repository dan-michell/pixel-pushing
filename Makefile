main: main.cpp
	clang++ -Wall -Werror main.cpp glad.c -I/opt/homebrew/include -L/opt/homebrew/lib -lglfw -Wno-deprecated -o build/main
