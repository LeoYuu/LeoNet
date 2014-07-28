CC := gcc
CX := g++

SRC_PATH := ../../src
LIB_PATH := ../../../libs/linux

SHARED = -fPIC --shared
CFLAGS = -g -O2 -Wall -I$(INC_PATH) -I$(SRC_PATH)

all : $(LIB_PATH)/libxml.so

$(LIB_PATH)/libxml.so : $(SRC_PATH)/pugixml.cpp
	$(CX) $(CFLAGS) $(SHARED) $^ -o $@

clean :
	rm -f $(LIB_PATH)/libxml.so
