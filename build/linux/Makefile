CC := gcc
CX := g++

SRC_PATH := ../../src
INC_PATH := ../../include
LIB_PATH := ../../libs/linux
BIN_PATH := ../../bin

SHARED = -fPIC --shared
CFLAGS = -g -O2 -Wall -I$(INC_PATH) -L$(LIB_PATH)

all : $(LIB_PATH)/libnet.so $(LIB_PATH)/libencrypt.so \
	$(LIB_PATH)/libprotocols.so $(LIB_PATH)/libutilities.so \
	$(BIN_PATH)/leo_server_test $(BIN_PATH)/leo_client_test

$(LIB_PATH)/libnet.so : $(SRC_PATH)/leo_net_service.c
	$(CC) $(CFLAGS) $(SHARED) $^ -o $@ -I$(SRC_PATH) -levent_core -lpthread

$(LIB_PATH)/libencrypt.so : $(SRC_PATH)/leo_des_crypto.c $(SRC_PATH)/leo_dof_crypto.c
	$(CC) $(CFLAGS) $(SHARED) $^ -o $@ -I$(SRC_PATH)

$(LIB_PATH)/libprotocols.so : $(SRC_PATH)/leo_tcp_game_protocol.c
	$(CX) $(CFLAGS) $(SHARED) $^ -o $@ -I$(SRC_PATH)

$(LIB_PATH)/libutilities.so : $(SRC_PATH)/ring_buffer.cpp
	$(CX) $(CFLAGS) $(SHARED) $^ -o $@ -I$(SRC_PATH)

$(BIN_PATH)/leo_server_test : $(SRC_PATH)/leo_net_message.cpp $(SRC_PATH)/leo_net_session.cpp $(SRC_PATH)/leo_server_test.cpp
	$(CX) $(CFLAGS) $^ -o $@ -I$(SRC_PATH) -levent_core -lnet -lencrypt -lprotocols -lutilities

$(BIN_PATH)/leo_client_test : $(SRC_PATH)/leo_net_message.cpp $(SRC_PATH)/leo_client_test.cpp
	$(CX) $(CFLAGS) $^ -o $@ -I$(SRC_PATH) -levent_core -lnet -lencrypt -lprotocols -lutilities

clean :
	rm -f $(LIB_PATH)/libnet.so $(LIB_PATH)/libencrypt.so $(LIB_PATH)/libprotocols.so \
		$(LIB_PATH)/libutilities.so $(BIN_PATH)/leo_server_test $(BIN_PATH)/leo_client_test
