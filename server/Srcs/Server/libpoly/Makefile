CXX = g++10
BIN = ./libpoly.a

GCC_VERSION = $(shell $(CC) --version 2>&1 | grep "(GCC)" | cut -d' ' -f3  | cut -d'.' -f1)

INCLUDE =
# Include directory for external libraries. This path was previously
# relative to "server/Srcs", which resulted in a missing header error
# for <msl/random.h>.  Adjust the path so it points to the repository
# level "Extern" directory when building from this subdirectory.
INCLUDE += -I../../../../Extern/include
LIBS =

CFLAGS  = -m32 -Wall -O2 -pipe -fno-exceptions -fno-rtti $(INCLUDE)
CFLAGS += -std=c++20

OBJFILES = Base.o Poly.o SymTable.o Symbol.o

default:
	$(MAKE) $(BIN)

$(BIN): $(OBJFILES)
	ar cru $(BIN) $(OBJFILES) $(LIBS)
	ranlib $(BIN)
	chmod 700 $(BIN)

clean:
	rm -f *.o
	rm -f $(BIN)

dep:
	touch Depend
	$(CXX) $(CFLAGS) -MM *.cc > Depend

$(OBJFILES):
	$(CXX) $(CFLAGS) -c $<

include Depend

