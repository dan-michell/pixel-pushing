CPPC   = clang++
FLAGS  = -std=c++23 -g -Wall -Wextra 
RM     = rm -rf
OPENGL_SRCDIR = opengl/
MINIRAY_SRCDIR = miniray/
BUILDDIR = build/
 
opengl: $(OPENGL_SRCDIR)main.cpp
	$(CPPC) $(FLAGS) $(OPENGL_SRCDIR)main.cpp $(OPENGL_SRCDIR)window.cpp $(OPENGL_SRCDIR)shader/shader.cpp  $(OPENGL_SRCDIR)glad.c -I/opt/homebrew/include -L/opt/homebrew/lib -lglfw -Wno-deprecated -o $(BUILDDIR)opengl

miniray: $(MINIRAY_SRCDIR)main.cpp
	$(CPPC) $(FLAGS) $(MINIRAY_SRCDIR)main.cpp -o $(BUILDDIR)miniray


