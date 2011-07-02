
LIBCDSPATH = ../libcds-v1.0.7

CPP=g++
CPPFLAGS=
#FLAGS=-O9 -Wno-deprecated
FLAGS=-g -Wno-deprecated #-Wall
INCLUDES=-I $(LIBCDSPATH)/includes/ -I /usr/local/include -I ./include -I /opt/local/include
LDFLAGS=-lstdc++ -lraptor2 -lz
DOXYGEN=doxygen
DEFINES= -DUSE_RAPTOR

LIB=$(LIBCDSPATH)/lib/libcds.a -L/usr/local/lib -L/opt/local/lib

OBJECTS=$(patsubst %.cpp,%.o, $(wildcard src/*.cpp) $(wildcard src/**/*.cpp))
HEADERS=$(wildcard include/*.hpp) $(wildcard include/*.h) $(wildcard src/*.hpp) $(wildcard src/*.h) $(wildcard src/**/*.hpp)
TOOLS=$(patsubst %.cpp,%, $(wildcard tools/*.cpp))

all: showflags tools

showflags:
	@echo " [CFG] FLAGS = $(FLAGS)"
	@echo " [CFG] DEFINES = $(DEFINES)"

tools/%: tools/%.cpp libhdt.a
	@echo " [HDT] Compiling tool $<"
	@$(CPP) $(INCLUDES) $(CPPFLAGS) $(FLAGS) $(DEFINES) $(LDFLAGS) $< -o $(patsubst %.cpp, % ,$<) libhdt.a ${LIB}

tools: libhdt.a $(TOOLS)
	@echo " [HDT] Compiling tools"

%.o: %.cpp $(HEADERS) Makefile
	@echo " [HDT] Compiling $<"
	@$(CPP) $(INCLUDES) $(CPPFLAGS) $(FLAGS) $(DEFINES) -c $< -o $@

libhdt.a: $(OBJECTS) $(HEADERS)
	@echo " [HDT] Linking libhdt.a"
	@ar rcs libhdt.a $(OBJECTS)

doc:	
	@echo " [HDT] Generating documentation"
	$(DOXYGEN)
	
clean:
	@echo " [CLN] Removing object files"
	@rm -f $(OBJECTS) $(TOOLS) libhdt.a *~
	@rm -Rf tools/*dSYM

