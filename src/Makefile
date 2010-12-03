CPP=g++
FLAGS=-O9 -I libcds/includes/ 
LDFLAGS=-lstdc++
#LIB=libcds.a
LIB=-lraptor libcds/lib/libcds.a -L/usr/local/lib

OBJECTS=RDFSyntaxMediator.o HDTLoader.o RDFParser.o MiniHashTable.o HashTable.o Header.o Dictionary.o PlainDictionary.o Triples.o PlainTriples.o CompactTriples.o BitmapTriples.o HDT.o


all: clean libcdsMake build

libcdsMake:
	@echo " [MSG] Entering directory libcds"
	@cd libcds;make;cd ..
	@echo " [HDT] Building HDT"
		
BIN=hdt

%.o: %.cpp
	@echo " [HDT] Compiling $<"
	@$(CPP) $(CPPFLAGS) $(LDFLAGS) $(FLAGS) -c $< -o $@

build: 	$(OBJECTS) $(BIN)

hdt:	
	$(CPP) $(CPPFLAGS) $(LDFLAGS) $(FLAGS) -o hdt $(OBJECTS) ${LIB}

clean:
	@echo " [MSG] Entering directory libcds"
	@cd libcds;make clean;cd ..
	@echo " [CLN] Removing object files"
	@rm -f $(OBJECTS) $(BIN) *~

