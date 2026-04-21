TARGET = jpeg
SOURCE = ckconfig.cpp    jchuff.cpp      jcparam.cpp     jdapistd.cpp    jdhuff.cpp      jdphuff.cpp     jfdctfst.cpp    jmemmgr.cpp \
jcapimin.cpp    jcinit.cpp      jcphuff.cpp     jdatadst.cpp    jdinput.cpp     jdpostct.cpp    jfdctint.cpp    jmemnobs.cpp \
jcapistd.cpp    jcmainct.cpp    jcprepct.cpp    jdatasrc.cpp    jdmainct.cpp    jdsample.cpp    jidctflt.cpp    jquant1.cpp \
jccoefct.cpp    jcmarker.cpp    jcsample.cpp    jdcoefct.cpp    jdmarker.cpp    jdtrans.cpp     jidctfst.cpp    jquant2.cpp \
jccolor.cpp     jcmaster.cpp    jctrans.cpp     jdcolor.cpp     jdmaster.cpp    jerror.cpp      jidctint.cpp    jutils.cpp \
jcdctmgr.cpp    jcomapi.cpp     jdapimin.cpp    jddctmgr.cpp    jdmerge.cpp     jfdctflt.cpp    jidctred.cpp



TARGET_LIB = lib$(TARGET).a

CFLAGS += -I. -I.. -std=c++14 -Wunused-result
CFLAGS += -arch arm64


CFLAGS_DEBUG = -DDEBUG -g3
CFLAGS_RELEASE = -O3 -DRELEASE -funroll-loops -ffast-math -DNDEBUG

SRC = ./
BIN = ../
BIN_O = ../Bin/Linux/Release/$(TARGET)/
INCLUDE = /usr/include/

CC  = gcc
CXX = g++
MD  = mkdir
AR  = ar

OBJECTS=$(addprefix $(BIN_O), $(addsuffix .o, $(basename $(SOURCE))))

all: CFLAGS += $(CFLAGS_RELEASE)
all: $(BIN)
all: $(BIN_O)
all: $(BIN)$(TARGET_LIB)

allNoGL: CFLAGS += $(CFLAGS_RELEASE)
allNoGL: $(BIN)
allNoGL: $(BIN_O)
allNoGL: $(BIN)$(TARGET_LIB)

debug: CFLAGS += $(CFLAGS_DEBUG)
debug: $(BIN)
debug: $(BIN)$(TARGET_LIB)

clean:
	rm -f $(BIN)$(TARGET_LIB)
	rm -f $(OBJECTS)

$(BIN):
	$(MD) -p $(BIN)

$(BIN_O):
	$(MD) -p $(BIN_O)

$(BIN)$(TARGET_LIB): $(OBJECTS)
	$(AR) rcs $@ $(OBJECTS)

$(BIN_O)%.o: $(SRC)%.c
	$(CC) -c -o $@ $(CFLAGS) -I$(INCLUDE) $<

$(BIN_O)%.o: $(SRC)%.cpp
	$(CXX) -c -o $@ $(CFLAGS) -I$(INCLUDE) $<
