linux:
	echo ""
	g++ -I. Main.cpp GameObject.cpp GLSL_helper.cpp MStackHelp.cpp GeometryCreator.cpp libglfw.a -lX11 -lGL -lGLU -lrt -lm -lXrandr -pthread -DGL_GLEXT_PROTOTYPES -o GeomDemo
	echo ""

glfw:
	(cd glfw-2.7.7 && make x11 && cp lib/x11/libglfw* ..)


#	g++ Main.cpp GLSL_helper.cpp MStackHelp.cpp GeometryCreator.cpp -DGL_GLEXT_PROTOTYPES -lGL -lGLU -lglut -o GeomDemo

clean:
	rm -f *~
	rm -f GeomDemo
	rm -f *.o
