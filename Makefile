main: main.cpp
	clang++ -Wall -Werror main.cpp hello_triangle/hello_triangle.cpp glad.c -I/opt/homebrew/include -L/opt/homebrew/lib -lglfw -Wno-deprecated -o build/main
