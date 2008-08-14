GECKO_PATH = /Users/dxoigmn/Downloads/xulrunner-sdk
GECKO_SDK_PATH = $(GECKO_PATH)/sdk

CXX = g++
CC = g++

SRCS =  nymble_user.cpp \
        nymble_user_entry.cpp \
        nymble_pseudonym.cpp \
        nymble_blacklist.cpp \
        nymble_credential.cpp \
        nymble_ticket.cpp \
        nymble_manager.cpp \
        nymble_manager_entry.cpp \
        nymble_linking_token.cpp \
        nymble.cpp \
        nsNymbleUser.cpp \
        nsNymbleUserModule.cpp

OBJS =  nymble_user.o \
        nymble_user_entry.o \
        nymble_pseudonym.o \
        nymble_blacklist.o \
        nymble_credential.o \
        nymble_ticket.o \
        nymble_manager.o \
        nymble_manager_entry.o \
        nymble_linking_token.o \
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

%.o: %.cpp
	$(CXX) -c $(INCFLAGS) $(CXXFLAGS) $(LIBPATH) -c $< -o $@

%.o: %.c
	$(CC) -c $(INCFLAGS) $(CFLAGS) $(CXXFLAGS) $(LIBPATH) -c $< -o $@

generate:
	$(GECKO_PATH)/bin/xpidl -m header -I$(GECKO_PATH)/idl nsINymbleUser.idl
	$(GECKO_PATH)/bin/xpidl -m typelib -I$(GECKO_PATH)/idl nsINymbleUser.idl

clean: 
	rm $(TARGET) *.o
