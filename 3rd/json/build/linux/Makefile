CC := gcc
CX := g++

SRC_PATH := ../../src
INC_PATH := ../../include
LIB_PATH := ../../../libs/linux

SHARED = -fPIC --shared
CFLAGS = -g -O2 -Wall -I$(INC_PATH) -I$(SRC_PATH)

all : $(LIB_PATH)/libjson.so

$(LIB_PATH)/libjson.so : $(SRC_PATH)/lib_json/json_reader.cpp $(SRC_PATH)/lib_json/json_value.cpp $(SRC_PATH)/lib_json/json_writer.cpp
	$(CX) $(CFLAGS) $(SHARED) $^ -o $@

clean :
	rm -f $(LIB_PATH)/libjson.so
