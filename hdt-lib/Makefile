OS := $(shell uname)

LIBCDSPATH = ../libcds-v1.0.12

# Uncomment here to enable optional dependencies
#RAPTOR_SUPPORT=true
#KYOTO_SUPPORT=true
#LIBZ_SUPPORT=true

CPP=g++
FLAGS=-O3 -Wno-deprecated
INCLUDES=-I $(LIBCDSPATH)/includes/ -I /usr/local/include -I ./include -I /opt/local/include -I /usr/include
LDFLAGS=
DOXYGEN=doxygen
DEFINES=
LIB=$(LIBCDSPATH)/lib/libcds.a -L/usr/local/lib -lstdc++

ifeq ($(RAPTOR_SUPPORT), true)
DEFINES:=$(DEFINES) -DUSE_RAPTOR
LIB:=$(LIB) -lraptor2
endif

ifeq ($(KYOTO_SUPPORT), true)
DEFINES:=$(DEFINES) -DUSE_KYOTO
LIB:=$(LIB) -lkyotocabinet
endif

ifeq ($(LIBZ_SUPPORT), true)
DEFINES:=$(DEFINES) -DUSE_LIBZ
LIB:=$(LIB) -lz
endif

ifeq ($(OS), Darwin)
CPP=clang++
FLAGS:=$(FLAGS) -arch x86_64 #-msse4.2 
endif

ifeq ($(CPP), g++)
FLAGS:=$(FLAGS) -fopenmp
endif

SRCS=$(wildcard src/*.cpp) $(wildcard src/**/*.cpp) $(wildcard src/**/**/*.cpp)
THIRD=$(wildcard third/*.cpp) $(wildcard third/**/*.cpp)
OBJECTS=$(patsubst %.cpp,%.o, $(SRCS) $(THIRD))
HEADERS=$(wildcard include/*.hpp) $(wildcard include/*.h) $(wildcard src/*.hpp) $(wildcard src/*.h) $(wildcard src/**/*.hpp)
TOOLS=$(patsubst %.cpp,%, $(wildcard tools/*.cpp))
TESTS=$(patsubst %.cpp,%, $(wildcard tests/*.cpp))
EXAMPLES=$(patsubst %.cpp,%, $(wildcard examples/*.cpp))

all:
	@$(MAKE) showflags 
	@$(MAKE) -C $(LIBCDSPATH)
	@$(MAKE) tools
	@$(MAKE) examples 

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

examples/%: examples/%.cpp libhdt.a
	@echo " [HDT] Compiling example $<"
	@$(CPP) $(INCLUDES) $(FLAGS) $(DEFINES) $(LDFLAGS) $< -o $(patsubst %.cpp, % ,$<) libhdt.a ${LIB}

examples: libhdt.a $(EXAMPLES)

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
	@rm -f $(OBJECTS) $(TOOLS) $(TESTS) $(EXAMPLES) libhdt.a *~
	@rm -Rf tools/*dSYM tests/*dSYM examples/*dSYM

