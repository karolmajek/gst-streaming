GCC ?= g++
CCFLAGS     :=  
LDFLAGS     := 
INCLUDES  := -pthread  -I.. -I/usr/local/include/SDL2 -I/usr/include/GL -I/usr/include/gstreamer-1.0 -I/usr/include/glib-2.0 -I/usr/lib/arm-linux-gnueabihf/glib-2.0/include 
LIBRARIES := -L/usr/lib/x86_64-linux-gnu -lgstreamer-1.0 -lgobject-2.0 -lglib-2.0 -lgstapp-1.0 -L/usr/lib/x86_64-linux-gnu -lGL -lglut -lGLU -lGLEW -lX11 -lm -lrt -lSDL2
BUILD_PATH = ./


# Target rules
all: build

build: renderer

renderer: renderer.cpp
	mkdir -p BUILD
	$(GCC) $(INCLUDES)  $(CCFLAGS) $(LDFLAGS) -o $(BUILD_PATH)$@ $+ $(LIBRARIES)
.PHONY : clean
clean:
	rm -f $(BUILD_PATH)renderer *.o
