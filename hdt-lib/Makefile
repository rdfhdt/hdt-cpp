
LIBCDSPATH = ../libcds

CPP=g++
CPPFLAGS=
#FLAGS=-O9
FLAGS=-g -Wno-deprecated
INCLUDES=-I $(LIBCDSPATH)/includes/ -I /usr/local/include -I ./include -I /opt/local/include
LDFLAGS=-lstdc++ -lraptor
DOXYGEN=doxygen

LIB=$(LIBCDSPATH)/lib/libcds.a -L/usr/local/lib -L/opt/local/lib

OBJECTS=$(patsubst %.cpp,%.o, $(wildcard src/*.cpp))
HEADERS=$(wildcard include/*.hpp) $(wildcard src/*.hpp) $(wildcard include/*.h) $(wildcard src/*.h)
EXAMPLES=$(patsubst %.cpp,%, $(wildcard examples/*.cpp))

all: libhdt examples

examples/%: examples/%.cpp $(OBJECTS) $(HEADERS)
	@echo " [HDT] Compiling example $<"
	@$(CPP) $(INCLUDES) $(CPPFLAGS) $(FLAGS) $(LDFLAGS) $< -o $(patsubst %.cpp, % ,$<) ${LIB} libhdt.a 

examples: libhdt $(EXAMPLES)
	@echo " [HDT] Compiling examples"

%.o: %.cpp $(HEADERS)
	@echo " [HDT] Compiling $<"
	@$(CPP) $(INCLUDES) $(CPPFLAGS) $(FLAGS) -c $< -o $@

libhdt: $(OBJECTS)
	@echo " [HDT] Linking libhdt.a"
	@ar rcs  libhdt.a $(OBJECTS)

doc:	
	@echo " [HDT] Generating documentation"
	@cd ..; $(DOXYGEN)
	
clean:
	@echo " [CLN] Removing object files"
	@rm -f $(OBJECTS) $(EXAMPLES) libhdt.a *~

