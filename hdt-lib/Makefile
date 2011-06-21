
LIBCDSPATH = ../libcds-v1.0.7

CPP=g++
CPPFLAGS=
FLAGS=-O9 -Wno-deprecated 
#FLAGS=-g -Wno-deprecated  #-Wall
INCLUDES=-I $(LIBCDSPATH)/includes/ -I /usr/local/include -I ./include -I /opt/local/include
LDFLAGS=-lstdc++ -lraptor2 -lz
DOXYGEN=doxygen

LIB=$(LIBCDSPATH)/lib/libcds.a -L/usr/local/lib -L/opt/local/lib

OBJECTS=$(patsubst %.cpp,%.o, $(wildcard src/*.cpp) $(wildcard src/**/*.cpp))
HEADERS=$(wildcard include/*.hpp) $(wildcard include/*.h) $(wildcard src/*.hpp) $(wildcard src/*.h)
TOOLS=$(patsubst %.cpp,%, $(wildcard tools/*.cpp))

all: libhdt tools

tools/%: tools/%.cpp $(OBJECTS) $(HEADERS)
	@echo " [HDT] Compiling tool $<"
	@$(CPP) $(INCLUDES) $(CPPFLAGS) $(FLAGS) $(LDFLAGS) $< -o $(patsubst %.cpp, % ,$<) libhdt.a ${LIB}

tools: libhdt $(TOOLS)
	@echo " [HDT] Compiling tools"

%.o: %.cpp $(HEADERS)
	@echo " [HDT] Compiling $<"
	@$(CPP) $(INCLUDES) $(CPPFLAGS) $(FLAGS) -c $< -o $@

libhdt: $(OBJECTS) $(HEADERS)
	@echo " [HDT] Linking libhdt.a"
	@ar rcs libhdt.a $(OBJECTS)

doc:	
	@echo " [HDT] Generating documentation"
	$(DOXYGEN)
	
clean:
	@echo " [CLN] Removing object files"
	@rm -f $(OBJECTS) $(TOOLS) libhdt.a *~
	@rm -Rf tools/*dSYM

