CXX = c++
CPPFLAGS += -g -fno-rtti -fno-exceptions -shared -fPIC -dynamiclib -L/usr/lib
 
GECKO_SDK_PATH = ../gecko/gecko-sdk
 
GECKO_CONFIG_INCLUDE = -include mozilla-config.h 
 
GECKO_DEFINES = -DXPCOM_GLUE
 
GECKO_INCLUDES = -I$(GECKO_SDK_PATH)/include/ 
 
GECKO_LDFLAGS = -L$(GECKO_SDK_PATH)/lib/ -lxpcomglue -lxpcomglue_s -lnspr4 -lplds4 -framework CoreFoundation -lcrypto

GECKO_STUFF = $(GECKO_CONFIG_INCLUDE) $(GECKO_DEFINES) $(GECKO_INCLUDES) $(GECKO_LDFLAGS)

OBJS = NymbleModule.o Nymble.o libnymble/nymble.o libnymble/user.o libnymble/util.o libnymble/nymble_sexp.o libnymble/sexp/sexp-basic.o libnymble/sexp/sexp-input.o libnymble/sexp/sexp-output.o

TARGET = Nymble.dylib

build: $(OBJS)
	$(CXX) -Wall -o $(TARGET) $(GECKO_STUFF) $(CPPFLAGS) $(CXXFLAGS) $^

%.o: %.cpp
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $< -o $@

clean: 
	rm $(TARGET) *.o
