SHELL := bash

# Name of the final library, to be put in $(LIBDIR)
PRODUCT := libchess.a

TEST_EXECUTABLE := tests

# For the final .a file
LIBDIR  := lib/
# For any executable files (possibly from tests)
BINDIR  := bin/
# For public header files
INCDIR  := include/
# For source files an internal header files
SRCDIR  := src/
# For all object files
OBJDIR  := obj/
# For test code
TESTDIR := test/

# CXX := em++ or g++ or clang++
INCFLAGS := -I include
CXXFLAGS := -std=c++14 -Wall -Wextra -O3

# GNU Make wildcard function generates list of .cpp files
SRCFILES := $(wildcard $(SRCDIR)*.cpp)
TESTFILES := $(wildcard $(TESTDIR)*.cpp)
# GNU Make patsubst creates list of .o files from source files
OBJFILES := $(patsubst $(SRCDIR)%.cpp,$(OBJDIR)%.o,$(SRCFILES))
# Here, the same thing is done to get a list of dependency files
DEPFILES := $(patsubst $(SRCDIR)%.cpp,$(OBJDIR)%.d,$(SRCFILES))

# First target is the default - links executable files together
# $^ refers to all prerequisites, $@ to the target of the rule
$(LIBDIR)$(PRODUCT): $(OBJFILES)
	mkdir -p $(LIBDIR)
	ar rcs $@ $^

lookup_tables:
	$(CXX) $(CXXFLAGS) $(INCFLAGS) meta/gen.cpp -o $(BINDIR)gen -L$(LIBDIR) -lchess
	$(BINDIR)gen p > src/lookup_tables.h

test: $(BINDIR)$(TEST_EXECUTABLE) run_tests

run_tests:
	$(BINDIR)$(TEST_EXECUTABLE)

$(BINDIR)$(TEST_EXECUTABLE): $(TESTFILES) $(LIBDIR)$(PRODUCT)
	$(CXX) $(CXXFLAGS) $(INCFLAGS) $(TESTFILES) -o $(BINDIR)$(TEST_EXECUTABLE) -L$(LIBDIR) -lchess -lboost_unit_test_framework -lpthread

# Clean the project by removing all object files and executable
clean:
	rm -f $(OBJDIR)* $(LIBDIR)$(PRODUCT) $(BINDIR)*

# Remove dependency files and rebuild all dependencies
depends:
	rm -f $(OBJDIR)*.d
	$(MAKE) $(DEPFILES)

# How to build a .o file from a .cpp file
# $< refers to the first prerequisite only
$(OBJDIR)%.o: $(SRCDIR)%.cpp
	mkdir -p $(OBJDIR)
	$(CXX) $(CXXFLAGS) $(INCFLAGS) -c $< -o $@

# How to build .d files
# -MM option says to generate dependencies (though not for, say, <iostream>)
#  sed is then used to change lines like:
#      main.o: src/main.cpp inc/dice.h inc/report.h
#  to lines like:
#      obj/main.d obj/main.o: src/main.cpp inc/dice.h inc/report.h
#  tr is used to remove backslashes and EOL characters generated by GCC
$(OBJDIR)%.d: $(SRCDIR)/%.cpp
	mkdir -p $(OBJDIR)
	$(CXX) $(INCFLAGS) -MM $< \
	    | tr '\n\r\\' ' ' \
	    | sed -e 's%^%$@ %' -e 's% % $(OBJDIR)%'\
	    > $@

# Pastes in the dependency files here, which read as simple rules
# There can be multiple rules for one target, make will merge prerequisites
# If the files don't exist, make will produce them using the last rule
-include $(DEPFILES)

