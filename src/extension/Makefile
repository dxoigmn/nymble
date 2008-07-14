GECKO_PATH = /Users/dxoigmn/Downloads/xulrunner-sdk
GECKO_SDK_PATH = $(GECKO_PATH)/sdk

CXX = g++
CC = gcc

SRCS = user.c util.c nymble_sexp.c sexp-basic.c sexp-input.c sexp-output.c nymble.c NymbleExModule.cpp NymbleEx.cpp
OBJS = user.o util.o nymble_sexp.o sexp-basic.o sexp-input.o sexp-output.o nymble.o NymbleEx.o

INCFLAGS = -I../libnymble/ -I../libnymble/sexp/ -I/opt/local/include/ -I$(GECKO_SDK_PATH)/include/
LIBPATH =  -L"." -L/opt/local/lib/ -L$(GECKO_SDK_PATH)/lib/
LIBS = -lxpcomglue -lxpcomglue_s -lnspr4 -lplds4 -framework CoreFoundation -lcrypto
DEFINES = -DXPCOM_GLUE

CFLAGS += -std=c99
CXXFLAGS += -shared -fPIC -dynamiclib

VPATH = .:../libnymble/:../libnymble/sexp/
TARGET = Nymble.dylib

build: $(OBJS)
	$(CXX) -o $(TARGET) $(DEFINES) $(CXXFLAGS) $(LIBPATH) $(LIBS) $^

%.o: %.cpp
	$(CXX) -c $(INCFLAGS) $(CXXFLAGS) $(LIBPATH) -c $< -o $@

%.o: %.c
	$(CC) -c $(INCFLAGS) $(CFLAGS) $(CXXFLAGS) $(LIBPATH) -c $< -o $@

generate:
	$(GECKO_PATH)/bin/xpidl -m header -I$(GECKO_PATH)/idl INymble.idl
	$(GECKO_PATH)/bin/xpidl -m typelib -I$(GECKO_PATH)/idl INymble.idl

clean: 
	rm $(TARGET) *.o
