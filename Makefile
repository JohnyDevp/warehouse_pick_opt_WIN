CC=g++ -std=c++20
CFLAGS=-Wextra -Wall
LIBS=-L. -lwarehouse_pick_opt -lwsock32 -lws2_32 -lgdi32
all: wpo_api.cpp
	$(CC) $(CFLAGS) wpo_api.cpp $(LIBS) -o wpo_api
