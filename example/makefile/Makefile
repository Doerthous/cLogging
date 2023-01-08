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

all: basic extend format threading logfile color logfile2

basic: $(SRC_DIR)/basic.c
	gcc -std=c99 -I$(INC) $^ -o $@

extend: $(SRC_DIR)/extend.c
	gcc -std=c99 -I$(INC) $^ -o $@

format: $(SRC_DIR)/format.c
	gcc -std=c99 -I$(INC) $^ -o $@

threading: $(SRC_DIR)/threading.cpp
	g++ -std=gnu++11 -I$(INC) $^ $(LIB) -o $@

logfile: $(SRC_DIR)/logfile.cpp
	g++ -std=gnu++11 -I$(INC) $^ $(LIB) -o $@
logfile2: $(SRC_DIR)/logfile2.cpp
	g++ -std=gnu++11 -I$(INC) $^ $(LIB) -o $@

color: $(SRC_DIR)/color.cpp
	g++ -std=gnu++11 -I$(INC) $^ $(LIB) -o $@

clean:
	-rm basic extend format threading logfile color logfile2 *.exe *.txt

.PHONY: FORCE
FORCE:
