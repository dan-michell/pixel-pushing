main: main.cpp
	 clang++ -Wall main.cpp window.cpp shader/shader.cpp stb/stb_image.cpp glad.c -I/opt/homebrew/include -L/opt/homebrew/lib -lglfw -Wno-deprecated -o build/main
