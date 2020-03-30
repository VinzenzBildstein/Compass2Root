.EXPORT_ALL_VARIABLES:

.SUFFIXES:

.PHONY: clean all

# := is only evaluated once

SHELL 		= /bin/sh

NAME 		= Compass2Root

ROOTLIBS     	:= $(shell root-config --libs)
ROOTINC      	:= -I$(shell root-config --incdir)

INCLUDES        = -I.

CC		= gcc
CXX   = g++
CPPFLAGS	= $(ROOTINC) $(INCLUDES) -fPIC
CXXFLAGS	= -std=c++11 -pedantic -Wall -g -O3

LDFLAGS		= -g -fPIC

LDLIBS 		= $(ROOTLIBS)

# for root: add a $(NAME)Dictionary.o
#LOADLIBES = \
#	$(NAME)Dictionary.o

# -------------------- implicit rules --------------------
# n.o made from n.c by 		$(CC) -c $(CPPFLAGS) $(CFLAGS)
# n.o made from n.cc by 	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS)
# n made from n.o by 		$(CC) $(LDFLAGS) n.o $(LOADLIBES) $(LDLIBS)

# -------------------- rules --------------------

all:  $(NAME) 
	@echo Done

# -------------------- pattern rules --------------------
# this rule sets the name of the .cxx file at the beginning of the line (easier to find)

%.o: %.cxx %.hh
	$(CXX) $< -c $(CPPFLAGS) $(CXXFLAGS) -o $@

# -------------------- default rule for executables --------------------

%: %.cxx $(LOADLIBES)
	$(CXX) $< $(CXXFLAGS) $(CPPFLAGS) $(LOADLIBES) $(LDLIBS) -o $@

# -------------------- clean --------------------

clean:
	rm  -f $(NAME) *.o
