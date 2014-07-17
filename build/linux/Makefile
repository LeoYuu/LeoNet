CC ?= gcc
CX ?= g++

SRC_PATH ?= ../../src
INC_PATH ?= ../../include
LIB_PATH ?= ../../libs/linux

SHARED := -fPIC --shared
CFLAGS = -g -O2 -Wall -I$(INC_PATH) -L$(LIB_PATH)

all : $(LIB_PATH)/leo_net_core.so $(LIB_PATH)/leo_encrypt.so \
	$(LIB_PATH)/leo_protocols.so $(LIB_PATH)/leo_utilities.so \
	$(LIB_PATH)/leo_net.so

$(LIB_PATH)/leo_net_core.so : $(SRC_PATH)/leo_net_service.c
	$(CC) $(CFLAGS) $(SHARED) $^ -o $@ -I$(SRC_PATH) -levent_core

$(LIB_PATH)/leo_encrypt.so : $(SRC_PATH)/leo_des_crypto.c $(SRC_PATH)/leo_dof_crypto.c
	$(CC) $(CFLAGS) $(SHARED) $^ -o $@ -I$(SRC_PATH)

$(LIB_PATH)/leo_protocols.so : $(SRC_PATH)/leo_tcp_game_protocol.c
	$(CX) $(CFLAGS) $(SHARED) $^ -o $@ -I$(SRC_PATH)

$(LIB_PATH)/leo_utilities.so : $(SRC_PATH)/ring_buffer.cpp
	$(CX) $(CFLAGS) $(SHARED) $^ -o $@ -I$(SRC_PATH)

$(LIB_PATH)/leo_net.so : $(SRC_PATH)/leo_net_message.cpp $(SRC_PATH)/leo_net_session.cpp
	$(CX) $(CFLAGS) $(SHARED) $^ -o $@ -I$(SRC_PATH) -lleo_net_core

clean :
	rm -f $(LIB_PATH)/*.so
