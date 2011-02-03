CPP=g++
CPPFLAGS=
FLAGS=-O9 -I libcds/includes/ -I /usr/local/include
LDFLAGS=-lstdc++ -lGL -lGLU -lglut -lraptor
DOXYGEN=doxygen

LIB=libcds/lib/libcds.a -L/usr/local/lib

# Flags for compiler and linker
ifeq (Darwin,$(shell uname -s))
LDFLAGS=-lstdc++ -framework OpenGL -framework GLUT
endif


OBJECTS=$(patsubst %.cpp,%.o,$(filter-out HDT.cpp rdfvis.cpp, $(wildcard *.cpp)))
HEADERS=*.h
BIN=hdt rdfvis

all: libcdsMake hdt rdfvis doc

libcdsMake:
	@echo " [MSG] Building libcds"
	@make -C libcds/
		
%.o: %.cpp $(HEADERS)
	@echo " [HDT] Compiling $<"
	@$(CPP) $(CPPFLAGS) $(FLAGS) -c $< -o $@

libhdt: $(OBJECTS)
	ar rcs  libhdt.a $(OBJECTS)

hdt:	$(OBJECTS) HDT.o
	@echo " [HDT] Linking HDT"
	@$(CPP) $(CPPFLAGS) $(FLAGS) $(LDFLAGS) -o hdt $(OBJECTS) HDT.o ${LIB} 

rdfvis:	$(OBJECTS) rdfvis.o
	@echo " [HDT] Linking rdfvis"
	@$(CPP) $(CPPFLAGS) $(FLAGS) $(LDFLAGS) -o rdfvis $(OBJECTS) rdfvis.o ${LIB}
	
doc:	
	@echo " [HDT] Generating documentation"
	@cd ..; $(DOXYGEN)
	
clean:
	@echo " [MSG] Cleaning libcds"
	@make -C libcds/ clean
	@echo " [CLN] Removing object files"
	@rm -f $(OBJECTS) $(BIN) *~

