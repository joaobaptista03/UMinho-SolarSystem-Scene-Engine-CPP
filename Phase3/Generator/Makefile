# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -Wall -g

# Include directory
INCLUDES = -I./figures

# Source files
SOURCES = main.cpp figures/plane.cpp figures/box.cpp figures/sphere.cpp figures/cone.cpp figures/bezier.cpp

# Object files directory
OBJDIR = .

# Object files
OBJECTS = $(SOURCES:.cpp=.o)
OBJECTS := $(addprefix $(OBJDIR)/, $(notdir $(OBJECTS)))

# Executable name
EXECUTABLE = generator

# Default target
all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^

# Rule to make object files
$(OBJDIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(OBJDIR)/%.o: figures/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Clean target
clean:
	rm -f $(OBJECTS) $(EXECUTABLE)

.PHONY: all clean