CXX = g++10
BIN_DIR = ../lib
BIN     = $(BIN_DIR)/libthecore.a
INCLUDE = -I../include
INCLUDE += -I../../../Extern/include

GCC_VERSION = $(shell $(CXX) --version 2>&1 | grep "(GCC)" | cut -d' ' -f3  | cut -d'.' -f1)

CFLAGS  = -m32 -Wall -O2 -pipe -g $(INCLUDE)
CFLAGS += -std=c++20

LIBS    =

OBJFILES = socket.o fdwatch.o buffer.o signal.o log.o utils.o \
	kstbl.o hangul.o heart.o main.o tea.o des.o gost.o memcpy.o DES_table.o

default:
	$(MAKE) $(BIN)

$(BIN): $(OBJFILES)
	if [ ! -d $(BIN_DIR) ]; then mkdir $(BIN_DIR); fi
	ar cru $(BIN) $(OBJFILES) $(LIBS)
	ranlib $(BIN)
	chmod 700 $(BIN)

clean:
	rm -f *.o
	rm -f $(BIN)

dep:
	$(CXX) $(CFLAGS) -MM *.c > Depend

$(OBJFILES):
	$(CXX) $(CFLAGS) -c $<

memcpy: memcpy.o utils.o log.o
	$(CXX) $(CFLAGS) -c -D__MAIN__ memcpy.c
	$(CXX) $(CFLAGS) -o memcpy memcpy.o utils.o log.o

include Depend
