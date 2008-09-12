GECKO_PATH = /Users/dxoigmn/Downloads/xulrunner-sdk
GECKO_SDK_PATH = $(GECKO_PATH)/sdk

CXX = g++
CC = g++

SRCS =  nymble_user.cc \
        nymble_user_entry.cc \
        nymble_pseudonym.cc \
        nymble_blacklist.cc \
        nymble_credential.cc \
        nymble_ticket.cc \
        nymble_manager.cc \
        nymble_manager_entry.cc \
        nymble_token.cc \
        nymble_util.cc \
        nymble.cc \
        nsNymbleUser.cc \
        nsNymbleUserModule.cc

OBJS =  nymble_user.o \
        nymble_user_entry.o \
        nymble_pseudonym.o \
        nymble_blacklist.o \
        nymble_credential.o \
        nymble_ticket.o \
        nymble_manager.o \
        nymble_manager_entry.o \
        nymble_token.o \
        nymble_util.o \
        nymble.o \
        nsNymbleUser.o \
        nsNymbleUserModule.o


INCFLAGS = -I../libnymble++/ -I../libnymble++/libjson/ -I/opt/local/include/ -I$(GECKO_SDK_PATH)/include/
LIBPATH =  -L"." -L/opt/local/lib/ -L$(GECKO_SDK_PATH)/lib/
LIBS = -lxpcomglue -lxpcomglue_s -lnspr4 -lplds4 -framework CoreFoundation -lcrypto -ljson
DEFINES = -DXPCOM_GLUE

CFLAGS += -std=c99
CXXFLAGS += -shared -fPIC -dynamiclib

VPATH = .:../libnymble++/:../libnymble++/libjson/
TARGET = nsNymbleUser.dylib

build: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) -o $(TARGET) $(DEFINES) $(CXXFLAGS) $(LIBPATH) $(LIBS) $^

%.o: %.cc
	$(CXX) -c $(INCFLAGS) $(CXXFLAGS) $(LIBPATH) -c $< -o $@

%.o: %.c
	$(CC) -c $(INCFLAGS) $(CFLAGS) $(CXXFLAGS) $(LIBPATH) -c $< -o $@

generate:
	$(GECKO_PATH)/bin/xpidl -m header -I$(GECKO_PATH)/idl nsINymbleUser.idl
	$(GECKO_PATH)/bin/xpidl -m typelib -I$(GECKO_PATH)/idl nsINymbleUser.idl

clean: 
	rm $(TARGET) *.o
