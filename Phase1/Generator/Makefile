# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -Wall -g

# Include directory
INCLUDES = -I./figures

# Source files
SOURCES = main.cpp figures/plane.cpp figures/box.cpp

# Object files
OBJECTS = $(SOURCES:.cpp=.o)

# Executable name
EXECUTABLE = generator

# Default target
all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^

# Rule to make object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Clean target
clean:
	rm -f $(OBJECTS) $(EXECUTABLE)

.PHONY: all clean

# Enquanto nao funciona:
# g++ -c ./figures/plane.cpp -o plane.o
# g++ -c ./figures/box.cpp -o box.o
# g++ -c ./figures/cone.cpp -o cone.o Ainda n existe
# g++ -c ./figures/sphere.cpp -o sphere.o  
# g++ -c main.cpp -o main.o
# g++ main.o plane.o box.o -o generator