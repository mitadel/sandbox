PYRE_DIR=/usr/local/pyre
PYRE_LIB_DIR=$(PYRE_DIR)/lib
PYRE_INC_DIR=$(PYRE_DIR)/include
PYRE_LIB=-lpyre -ljournal

main: main.cc
	g++ -o main main.cc -I$(PYRE_INC_DIR) -std=c++20 $(PYRE_LIB) -L$(PYRE_LIB_DIR)
