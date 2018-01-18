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
ALLIBS  	=  $(BASELIBS) -lCommandLineInterface -lWave
LIBS 		= $(ALLIBS)
LFLAGS		= -g -fPIC -shared

SWITCH = -UWRITE_WAVE
CFLAGS += $(SWITCH)
LFLAGS += $(SWITCH)

CFLAGS += -Wl,--no-as-needed
LFLAGS += -Wl,--no-as-needed
CFLAGS += -Wno-unused-variable -Wno-write-strings

LIB_O_FILES = build/Wave.o build/WaveDictionary.o
O_FILES = build/Wave.o 

USING_ROOT_6 = $(shell expr $(shell root-config --version | cut -f1 -d.) \>= 6)
ifeq ($(USING_ROOT_6),1)
	EXTRAS=lib/WaveDictionary_rdict.pcm
endif

SRCS = $(wildcard *.cc)
PROG = $(patsubst %.cc,%,$(SRCS)) 
all: $(PROG) $(EXTRAS)

%:	%.cc $(LIB_DIR)/libWave.so 
	@echo "Compiling $@"
	@$(CPP) $(CFLAGS) $(INCLUDES) $< $(LIBS) -o $(BIN_DIR)/$@

#%:	%.cc $(O_FILES)
#	@echo "Compiling $@"
##	$(CPP) $(CFLAGS) $(INCLUDES) $< $(LIBS) $(O_FILES) -o $(BIN_DIR)/$@
#	$(CPP) $(CFLAGS) $(INCLUDES) $^ $(LIBS) -o $(BIN_DIR)/$@

$(LIB_DIR)/libWave.so: $(LIB_O_FILES) 
	@echo "Making $@"
	@$(CPP) $(LFLAGS) -o $@ $^ -lc

build/%.o: src/%.cc inc/%.hh
	@echo "Building $@"
	@mkdir -p $(dir $@)
	@$(CPP) $(CFLAGS) $(INCLUDES) -c $< -o $@ 

build/WaveDictionary.o: build/WaveDictionary.cc
	@echo "Compiling $@"
	@mkdir -p $(dir $@)
	@$(CPP) $(CFLAGS) $(INCLUDES) -fPIC -c $< -o $@

build/WaveDictionary.cc: inc/Wave.hh inc/WaveLinkDef.h
	@echo "Building $@"
	@mkdir -p build
	@rootcint -f $@ -c $(INCLUDES) $(ROOTCFLAGS) $(SWITCH) $(notdir $^)

build/WaveDictionary_rdict.pcm: build/WaveDictionary.cc
	@echo "Confirming $@"
	@touch $@

lib/WaveDictionary_rdict.pcm: build/WaveDictionary_rdict.pcm | lib
	@echo "Placing $@"
	@cp $< $@

clean:
	@echo "Cleaning up"
	@rm -f $(BIN_DIR)/$(PROG)
	@rm -rf build doc
	@rm -f inc/*~ src/*~ scripts/*~ *~
