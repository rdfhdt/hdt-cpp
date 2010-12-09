CPP=g++
CPPFLAGS=
FLAGS=-O9 -I libcds/includes/ -I /usr/local/include
LDFLAGS=-lstdc++
LIB=-lraptor libcds/lib/libcds.a -L/usr/local/lib 

OBJECTS=$(patsubst %.cpp,%.o,$(wildcard *.cpp))
HEADERS=*.h
BIN=hdt

all: libcdsMake hdt 

libcdsMake:
	@echo " [MSG] Building libcds"
	@make -C libcds/
		
%.o: %.cpp $(HEADERS)
	@echo " [HDT] Compiling $<"
	@$(CPP) $(CPPFLAGS) $(FLAGS) -c $< -o $@

hdt:	$(OBJECTS)	
	@echo " [HDT] Linking HDT"
	@$(CPP) $(CPPFLAGS) $(FLAGS) $(LDFLAGS) -o hdt $(OBJECTS) ${LIB} 

clean:
	@echo " [MSG] Cleaning libcds"
	@make -C libcds/ clean
	@echo " [CLN] Removing object files"
	@rm -f $(OBJECTS) $(BIN) *~

