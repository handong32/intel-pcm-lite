CXX=g++
CFLAGS=-O4 -Wall

.PHONEY: clean all

# SRC DIRECTORY
SRC_DIR=src
# SRC FILES  
SRC_FILES=$(wildcard $(SRC_DIR)/*.cpp)
# BUILD DIRECTORIES
BUILD_DIR=build
# HEADERS INCLUDE
HEADER_DIR=include

# Obj list, put them in build dir.    
OBJS = $(patsubst src/%.cpp,$(BUILD_DIR)/%.o,$(SRC_FILES))

DYNAM_LIB=libintelpcmlite.so
STATIC_LIB=libintelpcmlite.a

all: $(BUILD_DIR)/$(DYNAM_LIB) $(BUILD_DIR)/$(STATIC_LIB)

$(DYNAM_LIB): build
	$(CXX) $(CFLAGS) -shared -fPIC -I $(HEADER_DIR) $(SRC_FILES) -o $(BUILD_DIR)/$@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp build
	$(CXX) $(CFLAGS) -c $< -o $@ -I $(HEADER_DIR)

$(BUILD_DIR)/$(STATIC_LIB): $(OBJS)
	ar rcs $@ $^

build:
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)





