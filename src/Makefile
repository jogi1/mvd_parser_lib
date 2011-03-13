CC=gcc -g

CFLAGS_LIB=-fPIC -c -Wall

CFLAGS_BIN=-DLIB_TEST -Wall

CFLAGS=

SO_NAME=libmvdparse.so

BIN_NAME=mvd_parse

LIB_COMPILE_DIR=objects/lib
LIB_RELEASE_DIR=lib

BIN_COMPILE_DIR=objects/bin
BIN_RELEASE_DIR=bin

ODIR=

OBJS= \
	  msg_functions.o \
	  new_parser.o \
	  parse_functions.o \
	  readablechars.o \
	  tokenize_string.o \
	  events.o \
	  stats_gather.o \
	  tools.o

LIB_OBJS=$(OBS)

BASEDIR=$(shell pwd)


all: lib bin

lib_setup:
	$(eval CFLAGS := $(CFLAGS_LIB))

lib_link:
	$(CC) -shared -Wl,-soname,$(SO_NAME) -o $(BASEDIR)/$(LIB_RELEASE_DIR)/$(SO_NAME) $(OBJS) -lc -lm

lib_compile: $(OBJS)

lib: lib_setup
	mkdir -p $(LIB_COMPILE_DIR) $(LIB_RELEASE_DIR)
	cd $(LIB_COMPILE_DIR)
	$(MAKE) -f $(BASEDIR)/Makefile lib_compile
	$(MAKE) -f $(BASEDIR)/Makefile lib_link
	cp $(BASEDIR)/common.h $(BASEDIR)/$(LIB_RELEASE_DIR)/mvdparse.h
	cat $(BASEDIR)/mvd_parser.h >> $(BASEDIR)/$(LIB_RELEASE_DIR)/mvdparse.h
	cat $(BASEDIR)/mvd_lib_functions.h >> $(BASEDIR)/$(LIB_RELEASE_DIR)/mvdparse.h


bin: 
	mkdir -p $(BIN_COMPILE_DIR) $(BIN_RELEASE_DIR)
	cd $(BIN_COMPILE_DIR)
	$(CC) -Wall $(BASEDIR)/test_program.c -I$(BASEDIR)/$(LIB_RELEASE_DIR) -L$(BASEDIR)/lib -lmvdparse -o $(BASEDIR)/$(LIB_RELEASE_DIR)/test_program

clean:
	rm -rf objects
	rm -rf $(LIB_RELEASE_DIR)
	rm -rf $(BIN_RELEASE_DIR)
	rm -rf *.o




