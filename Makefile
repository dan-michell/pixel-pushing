main: src/main.cpp
	clang++ --std=c++17 -g -Wall src/main.cpp src/window.cpp src/shader/shader.cpp  src/glad.c -I/opt/homebrew/include -L/opt/homebrew/lib -lglfw -Wno-deprecated -o build/main
