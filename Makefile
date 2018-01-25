.EXPORT_ALL_VARIABLES:
.PHONY: clean all 

BASE = /home/daq/work/seamine2018_daq/kathrin
BIN_DIR = $(BASE)/bin
LIB_DIR = $(BASE)/lib
COMMON_DIR = $(BASE)/common

ROOTCFLAGS   := $(shell root-config --cflags)
ROOTLIBS     := $(shell root-config --libs)
ROOTGLIBS    := $(shell root-config --glibs)
ROOTINC      := -I$(shell root-config --incdir)

CPP             = g++
CFLAGS		= -pedantic -Wall -Wno-long-long -g -O3 $(ROOTCFLAGS) -fPIC --std=c++11

INCLUDES        = -I./inc -I$(COMMON_DIR)
BASELIBS 	= -lm $(ROOTLIBS) $(ROOTGLIBS) -L$(LIB_DIR)
ALLIBS  	=  $(BASELIBS) -lCommandLineInterface -lSeamine
LIBS 		= $(ALLIBS)
LFLAGS		= -g -fPIC -shared

CFLAGS += -Wl,--no-as-needed
LFLAGS += -Wl,--no-as-needed
CFLAGS += -Wno-unused-variable -Wno-write-strings

LIB_O_FILES = build/Event.o build/PHA.o build/Wave.o build/Fragment.o build/Dictionary.o
O_FILES = build/SortHits.o build/ProcessHits.o

USING_ROOT_6 = $(shell expr $(shell root-config --version | cut -f1 -d.) \>= 6)
ifeq ($(USING_ROOT_6),1)
	EXTRAS=lib/Dictionary_rdict.pcm
endif

SRCS = $(wildcard *.cc)
PROG = $(patsubst %.cc,%,$(SRCS))
all: $(PROG) $(EXTRAS)

EventBuild: EventBuild.cc $(LIB_DIR)/libSeamine.so $(O_FILES)
	@echo "Compiling $@"
	@$(CPP) $(CFLAGS) $(INCLUDES) $< $(LIBS) $(O_FILES) -o $(BIN_DIR)/$@

Histos: Histos.cc $(LIB_DIR)/libSeamine.so 
	@echo "Compiling $@"
	@$(CPP) $(CFLAGS) $(INCLUDES) $< $(LIBS) -o $(BIN_DIR)/$@

Histos2: Histos2.cc $(LIB_DIR)/libSeamine.so 
	@echo "Compiling $@"
	@$(CPP) $(CFLAGS) $(INCLUDES) $< $(LIBS) -o $(BIN_DIR)/$@

$(LIB_DIR)/libSeamine.so: $(LIB_O_FILES) 
	@echo "Making $@"
	@$(CPP) $(LFLAGS) -o $@ $^ -lc

build/%.o: src/%.cc inc/%.hh
	@echo "Building $@"
	@mkdir -p $(dir $@)
	@$(CPP) $(CFLAGS) $(INCLUDES) -c $< -o $@ 

build/Dictionary.o: build/Dictionary.cc
	@echo "Compiling $@"
	@mkdir -p $(dir $@)
	@$(CPP) $(CFLAGS) $(INCLUDES) -fPIC -c $< -o $@

build/Dictionary.cc: inc/Event.hh inc/Wave.hh inc/PHA.hh inc/Fragment.hh inc/LinkDef.h
	@echo "Building $@"
	@mkdir -p build
	@rootcint -f $@ -c $(INCLUDES) $(ROOTCFLAGS) $(notdir $^)

build/Dictionary_rdict.pcm: build/Dictionary.cc
	@echo "Confirming $@"
	@touch $@

lib/Dictionary_rdict.pcm: build/Dictionary_rdict.pcm | lib
	@echo "Placing $@"
	@cp $< $@

doc:	doxyconf
	doxygen doxyconf
clean:
	@echo "Cleaning up"
	@for P in $(PROG); do \
	 rm -rf $(BIN_DIR)/$$P ; \
	done
	@rm -rf build doc
	@rm -f inc/*~ src/*~ scripts/*~ *~
