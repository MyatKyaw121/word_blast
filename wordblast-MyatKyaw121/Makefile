
# ROOTNAME should be set you your lastname_firstname_HW where
# lastname = your last name
# firstname= your first name
# To Use the Makefile - Edit as above
# then from the command line run:  make
# That command will build your program, and the program will be named the same
# as your main c file without an extension.
# You can then execute from the command line: make run
# This will actually run your program
#
# Using the command: make clean
# will delete the executable and any object files in your directory.
#

FIRSTNAME=myat
LASTNAME=kyaw
HW=4

ROOTNAME=$(LASTNAME)_$(FIRSTNAME)_HW

FOPTION=_main
RUNOPTIONS=WarAndPeace.txt 2
CC=gcc
CFLAGS= -g -I.
LIBS =-l pthread
DEPS = 
OBJ = $(ROOTNAME)$(HW)$(FOPTION).o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) 

$(ROOTNAME)$(HW)$(FOPTION): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

clean:
	rm *.o $(ROOTNAME)$(HW)$(FOPTION)

run: $(ROOTNAME)$(HW)$(FOPTION)
	./$(ROOTNAME)$(HW)$(FOPTION) $(RUNOPTIONS)
