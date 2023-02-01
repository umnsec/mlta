INSTALL_DIR=/path/to/clang-6-install
INSTALL_BIN=$(INSTALL_DIR)/bin
INSTALL_LIB=$(INSTALL_DIR)/lib
INSTALL_INC=$(INSTALL_DIR)/include

CC = $(INSTALL_BIN)/clang
CFLAGS = -I $(INSTALL_INC) -I . -Wall -fno-rtti -Wfatal-errors
LDFLAGS = -L $(INSTALL_LIB)

all: InstrumentPTW IRDumper

InstrumentPTW: InstrumentPTW.cpp
	$(CC) $(CFLAGS) -c InstrumentPTW.cpp -O3 -fPIC -o InstrumentPTW.o
	$(CC) -shared -fPIC -rdynamic -o $(INSTALL_LIB)/InstrumentPTW.so InstrumentPTW.o $(LDFLAGS)

clean:
	rm -f *.o *.so
