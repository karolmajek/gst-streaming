GCC ?= g++
CCFLAGS     := -pthread -I/usr/include/gstreamer-1.0 -I/usr/include/glib-2.0 -I/usr/lib/arm-linux-gnueabihf/glib-2.0/include 
LDFLAGS     :=
INCLUDES  := -I..
LIBRARIES := -lgstreamer-1.0 -lgobject-2.0 -lglib-2.0 -lgstapp-1.0 -L/usr/lib/x86_64-linux-gnu -lGL -lglut -lGLU -lX11 -lm -lrt
BUILD_PATH = ./


# Target rules
all: build

build: viewer

viewer: viewer.cpp
	mkdir -p BUILD
	$(GCC) $(INCLUDES)  $(CCFLAGS) $(LDFLAGS) -o $(BUILD_PATH)$@ $+ $(LIBRARIES)
.PHONY : clean
clean:
	rm -f $(BUILD_PATH)viewer *.o
