
CPP=g++

INCL=-I../includes/

## OBJECTS FROM cds_utils PACKAGE
UTILS_OBJ=utils/Array.o utils/BitString.o utils/cppUtils.o

# OBJECTS FROM cds_static PACKAGE
STATIC_BITSEQ_OBJ= static/bitsequence/BitSequence.o static/bitsequence/BitSequenceRG.o static/bitsequence/BitSequenceSDArray.o static/bitsequence/sdarraySadakane.o static/bitsequence/BitSequenceRRR.o static/bitsequence/BitSequenceBuilderRG.o static/bitsequence/BitSequenceBuilderRRR.o static/bitsequence/BitSequenceBuilderSDArray.o static/bitsequence/TableOffsetRRR.o

STATIC_OBJ=$(STATIC_BITSEQ_OBJ)

OBJECTS=$(UTILS_OBJ) $(STATIC_OBJ)

%.o: %.cpp
	@echo " [C++] Compiling $<"
	@$(CPP) $(CPPFLAGS) $(INCL) -c $< -o $@

all: pre lib 

clean:
	@echo " [CLN] Removing object files"
	@rm -f $(OBJECTS)

lib: pre $(OBJECTS)
	@echo " [LIB] Packing the object files"
	@ar rcs ../lib/libcds.a $(OBJECTS) 

pre:
	@echo " [HDR] Populating the includes folder"
	@echo " [CFG] CPPFLAGS = $(CPPFLAGS)"
	@make --no-print-directory -C utils
	@make --no-print-directory -C static/bitsequence

