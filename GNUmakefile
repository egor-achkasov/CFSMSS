# Compiler to use
CXX := g++
# Flags for the compiler
CXXFLAGS := -std=c++17 -O0 -Wall -Werror -pedantic -g -gpubnames
# Flags for the linker
LDLIBS :=

# Source files directory
SRCDIR := src
# Include files directory
INCDIR := include
# Object files directory
OBJDIR := obj

# Find all cpp files in SRC directory
SRCFILES := $(wildcard $(SRCDIR)/*.cpp)
# Generate list of object files from SRC files
OBJFILES := $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SRCFILES))
# Add INCDIR to include search path
CPPFLAGS := -I$(INCDIR)

# Name of the final binary file
PROGRAM := cfsmss

# Rule to build program from object files
$(PROGRAM): $(OBJFILES)
	$(CXX) $(CXXFLAGS) $(LDLIBS) $^ -o $@

# Rule to build object files from cpp files
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

# Create object directory if it does not exist
$(OBJDIR):
	mkdir -p $(OBJDIR)

# Clean rule to remove all generated files
clean:
	rm -f $(OBJFILES) $(PROGRAM)

.PHONY: clean

