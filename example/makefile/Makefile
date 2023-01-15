INC=../..
SRC_DIR=..
LIB=

ifeq ($(OS),Windows_NT)
    CCFLAGS += -D WIN32
    ifeq ($(PROCESSOR_ARCHITEW6432),AMD64)
        CCFLAGS += -D AMD64
    else
        ifeq ($(PROCESSOR_ARCHITECTURE),AMD64)
            CCFLAGS += -D AMD64
        endif
        ifeq ($(PROCESSOR_ARCHITECTURE),x86)
            CCFLAGS += -D IA32
        endif
    endif
else
    UNAME_S := $(shell uname -s)
	LIB += -lpthread
    ifeq ($(UNAME_S),Linux)
        CCFLAGS += -D LINUX
    endif
    ifeq ($(UNAME_S),Darwin)
        CCFLAGS += -D OSX
    endif
    UNAME_P := $(shell uname -p)
    ifeq ($(UNAME_P),x86_64)
        CCFLAGS += -D AMD64
    endif
    ifneq ($(filter %86,$(UNAME_P)),)
        CCFLAGS += -D IA32
    endif
    ifneq ($(filter arm%,$(UNAME_P)),)
        CCFLAGS += -D ARM
    endif
endif

TARGETS += basic
TARGETS += basic_e
TARGETS += basic_c
TARGETS += extend
TARGETS += extend_e
TARGETS += extend_c
TARGETS += format
TARGETS += format_e
TARGETS += format_c
TARGETS += threading
TARGETS += threading_e
TARGETS += threading_c
TARGETS += logfile
TARGETS += logfile_e
TARGETS += logfile_c
TARGETS += color
TARGETS += color_e
TARGETS += color_c
TARGETS += multidir
TARGETS += multidir_e
TARGETS += multidir_c

all: $(TARGETS)

%.eo: $(SRC_DIR)/%.c
	gcc -g -DLOGGING_AS_HEADER -std=c99 -I$(INC) -c $^ -o $@
%.eo: $(SRC_DIR)/%.cpp
	g++ -g -DLOGGING_AS_HEADER -std=gnu++11 -I$(INC) $(LIB) -c $^ -o $@
%.co: $(SRC_DIR)/%.c
	gcc -g -DLOGGING_AS_HEADER -DLOGGING_LOGGER_ADD_CUSTOM_FORMAT_AS_FUNCTION -std=c99 -I$(INC) -c $^ -o $@
%.co: $(SRC_DIR)/%.cpp
	g++ -g -DLOGGING_AS_HEADER -DLOGGING_LOGGER_ADD_CUSTOM_FORMAT_AS_FUNCTION -std=gnu++11 -I$(INC) $(LIB) -c $^ -o $@

%_e: logging.eo %.eo
	g++ $(LIB) $^ -o $@
%_c: custom.co %.co
	g++ $(LIB) $^ -o $@
%: $(SRC_DIR)/%.c ../../Logging.h
	gcc -std=c99 -I$(INC) $< -o $@
%: $(SRC_DIR)/%.cpp ../../Logging.h
	g++ -std=gnu++11 -I$(INC) $< $(LIB) -o $@

clean:
	-rm $(TARGETS) *.eo *.exe *.txt

.PHONY: FORCE
FORCE:
