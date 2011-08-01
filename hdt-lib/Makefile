OS := $(shell uname)

LIBCDSPATH = ../libcds-v1.0.7

CPP=g++
#CPP=clang++
CPPFLAGS=
#FLAGS=-O2 -Wno-deprecated
FLAGS=-O2 -Wno-deprecated #-Wall
INCLUDES=-I $(LIBCDSPATH)/includes/ -I /usr/local/include -I ./include -I /opt/local/include -I /usr/include
LDFLAGS=-lstdc++ -lraptor2
DOXYGEN=doxygen
#DEFINES= -DUSE_RAPTOR -DUSE_FOQ
DEFINES= -DUSE_FOQ #-DOLD_BITMAP
LIB=$(LIBCDSPATH)/lib/libcds.a -L/usr/local/lib -L/opt/local/lib

ifeq ($(OS), Darwin)
FLAGS=-arch i386 -arch x86_64 -O2 -Wno-deprecated
endif

SRCS=$(wildcard src/*.cpp) $(wildcard src/**/*.cpp)
OBJECTS=$(patsubst %.cpp,%.o, $(SRCS) )
HEADERS=$(wildcard include/*.hpp) $(wildcard include/*.h) $(wildcard src/*.hpp) $(wildcard src/*.h) $(wildcard src/**/*.hpp)
TOOLS=$(patsubst %.cpp,%, $(wildcard tools/*.cpp))
TESTS=$(patsubst %.cpp,%, $(wildcard tests/*.cpp))

all: showflags tools tests

clall: clean all

showflags:
	@echo " [CFG] OS= $(OS)"
	@echo " [CFG] CPP = $(CPP)"
	@echo " [CFG] FLAGS = $(FLAGS)"
	@echo " [CFG] DEFINES = $(DEFINES)"
	@echo " [CFG] INCLUDES = $(INCLUDES)"
	@echo " [CFG] LIB= $(LIB)"

tools/%: tools/%.cpp libhdt.a
	@echo " [HDT] Compiling tool $<"
	@$(CPP) $(INCLUDES) $(CPPFLAGS) $(FLAGS) $(DEFINES) $(LDFLAGS) $< -o $(patsubst %.cpp, % ,$<) libhdt.a ${LIB}

tools: libhdt.a $(TOOLS)

tests/%: tests/%.cpp libhdt.a
	@echo " [HDT] Compiling test $<"
	@$(CPP) $(INCLUDES) $(CPPFLAGS) $(FLAGS) $(DEFINES) $(LDFLAGS) $< -o $(patsubst %.cpp, % ,$<) libhdt.a ${LIB}

tests: libhdt.a $(TESTS)

%.o: %.cpp $(HEADERS) Makefile
	@echo " [HDT] Compiling $<"
	@$(CPP) $(INCLUDES) $(CPPFLAGS) $(FLAGS) $(DEFINES) -c $< -o $@

libhdt.a: $(OBJECTS) $(HEADERS)
	@echo " [HDT] Linking libhdt.a"
	@rm -Rf libhdt.a
	@ar rcs libhdt.a $(OBJECTS)

doc:	
	@echo " [HDT] Generating documentation"
	$(DOXYGEN)
	
clean:
	@echo " [CLN] Removing object files"
	@rm -f $(OBJECTS) $(TOOLS) libhdt.a *~
	@rm -Rf tools/*dSYM

