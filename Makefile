
PROGRAM = btr

CSOURCES = byteticker.c
COBJECTS = $(CSOURCES:.c=.o)
CFLAGS   = $(CDEFINES) $(CINCLUDE)
CDEFINES = 
CINCLUDE = 

LDFLAGS  = $(LDDIRS) $(LDLIBS)
LDDIRS   = 
LDLIBS   = -lrt

all: $(PROGRAM)

$(PROGRAM): $(COBJECTS)
	$(CC) -o $@ $< $(LDFLAGS)

clean: objclean execlean

objclean:
	$(RM) $(COBJECTS)

execlean:
	$(RM) $(PROGRAM)

relink: execlean all

test: $(PROGRAM)
	cat /dev/zero | ./btr > /dev/null

test%: $(PROGRAM)
	cat /dev/zero | ./btr $(shell echo $@ | sed "s/test//g") > /dev/null
