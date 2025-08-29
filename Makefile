main: main.cpp
	 clang++ -Wall main.cpp context.cpp hello_triangle/hello_triangle.cpp shaders/shaders.cpp glad.c -I/opt/homebrew/include -L/opt/homebrew/lib -lglfw -Wno-deprecated -o build/main
