IDIR=include
CXX=g++
CFLAGS=-I$(IDIR) -O3 --std=c++17 -g

OBJDIR=obj
SRCDIR=src
LDIR=lib
LDFLAGS=-lm -pg

DEPS = $(wildcard $(IDIR)/*.h)

SRCS=$(wildcard $(SRCDIR)/*.cc)
OBJ = $(patsubst $(SRCDIR)/%.cc,$(OBJDIR)/%.o,$(SRCS))


$(OBJDIR)/%.o: $(SRCDIR)/%.cc $(DEPS)
	@mkdir -p $(@D)
	$(CXX) -c -o $@ $< $(CFLAGS)

engine: $(OBJ)
	$(CXX) -o $@ $^ $(LDFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(OBJDIR)/*.o *~ core $(INCDIR)/*~
