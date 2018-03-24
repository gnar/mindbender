SRC += src/value/clobject.cpp
SRC += src/value/clstring.cpp
SRC += src/value/cltable.cpp
SRC += src/value/clvalue.cpp
SRC += src/value/clfunction.cpp
SRC += src/value/clexternalfunction.cpp
SRC += src/value/clarray.cpp
SRC += src/value/cluserdata.cpp
SRC += src/compiler/cllexer.cpp
SRC += src/compiler/clcompiler.cpp
SRC += src/compiler/clifunction.cpp
SRC += src/compiler/cliinstruction.cpp
SRC += src/vm/clcontext.cpp
SRC += src/vm/clthread.cpp
SRC += src/vm/clmodule.cpp
SRC += src/vm/clmathmodule.cpp
SRC += src/vm/clsysmodule.cpp
SRC += src/vm/clcollectable.cpp
SRC += src/serialize/clserialsaver.cpp
SRC += src/serialize/clserialloader.cpp
SRC += src/clopcode.cpp

# SRC += src/opt/clnamespace.cpp

OBJS = $(SRC:.cpp=.o)

#OPTIM = -O6 -ffast-math -funroll-loops -pipe -fomit-frame-pointer
OPTIM = -O3 -DDEBUG

CFLAGS = -Isrc -Wall $(OPTIM) $(PROF)
LFLAGS = 
EXE = bin/cl2
CC = g++
LL = g++

all: $(OBJS) src/test2.o
	$(LL) $(OBJS) src/test2.o $(LFLAGS) -o $(EXE)

lib: $(OBJS)
	ar rcs bin/cl2.a $(OBJS)

clean:
	-rm $(OBJS)
	-rm src/test.o
	-rm $(EXE)
	-rm bin/cl2.a

.SUFFIXES: .cpp .o
.cpp.o:
	$(CC) $(CFLAGS) -c $< -o $(<:.cpp=.o)

