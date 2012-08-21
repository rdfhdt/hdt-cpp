OS := $(shell uname)

LIBCDSPATH = ../libcds-v1.0.12

CPP=g++
FLAGS=-O3 -Wno-deprecated
INCLUDES=-I $(LIBCDSPATH)/includes/ -I /usr/local/include -I ./include -I /opt/local/include -I /usr/include
LDFLAGS=
DOXYGEN=doxygen
DEFINES=-DUSE_RAPTOR #-DUSE_KYOTO
LIB=$(LIBCDSPATH)/lib/libcds.a -L/usr/local/lib -lstdc++ -lraptor2 #-lkyotocabinet

ifeq ($(OS), Darwin)
CPP=clang++
FLAGS:=$(FLAGS) -arch x86_64 -msse4.2 
endif

ifeq ($(CPP), g++)
FLAGS:=$(FLAGS) -fopenmp
endif

SRCS=$(wildcard src/*.cpp) $(wildcard src/**/*.cpp) $(wildcard src/**/**/*.cpp)
OBJECTS=$(patsubst %.cpp,%.o, $(SRCS) )
HEADERS=$(wildcard include/*.hpp) $(wildcard include/*.h) $(wildcard src/*.hpp) $(wildcard src/*.h) $(wildcard src/**/*.hpp)
TOOLS=$(patsubst %.cpp,%, $(wildcard tools/*.cpp))
TESTS=$(patsubst %.cpp,%, $(wildcard tests/*.cpp))

all:
	@$(MAKE) showflags 
	@$(MAKE) tools tests

re:
	@$(MAKE) clean
	@$(MAKE) all 

showflags:
	@echo " [CFG] OS= $(OS)"
	@echo " [CFG] CPP = $(CPP)"
	@echo " [CFG] FLAGS = $(FLAGS)"
	@echo " [CFG] LDFLAGS = $(LDFLAGS)"
	@echo " [CFG] DEFINES = $(DEFINES)"
	@echo " [CFG] INCLUDES = $(INCLUDES)"
	@echo " [CFG] LIB= $(LIB)"

tools/%: tools/%.cpp libhdt.a
	@echo " [HDT] Compiling tool $<"
	@$(CPP) $(INCLUDES) $(FLAGS) $(DEFINES) $(LDFLAGS) $< -o $(patsubst %.cpp, % ,$<) libhdt.a ${LIB}

tools: libhdt.a $(TOOLS)

tests/%: tests/%.cpp libhdt.a
	@echo " [HDT] Compiling test $<"
	@$(CPP) $(INCLUDES) $(FLAGS) $(DEFINES) $(LDFLAGS) $< -o $(patsubst %.cpp, % ,$<) libhdt.a ${LIB}

tests: libhdt.a $(TESTS)

%.o: %.cpp $(HEADERS) Makefile
	@echo " [HDT] Compiling $<"
	@$(CPP) $(INCLUDES) $(FLAGS) $(DEFINES) -c $< -o $@

libhdt.a: $(OBJECTS) $(HEADERS)
	@echo " [HDT] Linking libhdt.a"
	@rm -Rf libhdt.a
	@ar rcs libhdt.a $(OBJECTS)

doc:	
	@echo " [HDT] Generating documentation"
	$(DOXYGEN)
	
clean:
	@echo " [CLN] Removing object files"
	@rm -f $(OBJECTS) $(TOOLS) $(TESTS) libhdt.a *~
	@rm -Rf tools/*dSYM tests/*dSYM

