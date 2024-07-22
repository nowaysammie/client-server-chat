.PHONY: all clean

CC = g++

CFLAGS = -g -Wall
CFLAGS = -std=c++2a

SRC_FILES = src/main.cpp
SRC_FILES += src/NetworkModule.cpp
SRC_FILES += src/PackageManager.cpp
SRC_FILES += src/Server.cpp
SRC_FILES += src/Storage.cpp

INCLUDES = -I./include/
TARGET_BIN = server
BIN_DIR = bin
OBJECTS=$(SRC_FILES:.cpp=.o)

all: build_prep build linking

build_prep:
	@mkdir -p $(BIN_DIR)

build: $(OBJECTS)

$(OBJECTS): $(SRC_FILES)
	$(CC) $(CFLAGS) $(INCLUDES) -c $(@:.o=.cpp) -o $@

linking: $(BIN_DIR)/$(TARGET_BIN)

$(BIN_DIR)/$(TARGET_BIN): $(OBJECTS)
	@echo "linking started ..."
	$(CC) $(CFLAGS) $(INCLUDES) -o $(BIN_DIR)/$(TARGET_BIN) $(OBJECTS)

clean:
	rm src/*.o
	rm bin/*