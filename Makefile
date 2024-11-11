# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -std=c++11

# Libraries
LDFLAGS = -lncurses

# Source files
SRC = GameEngine.cpp

# Object files
OBJ = GameEngine.o

# Header files (dependencies)
DEPS = GameBoard.h BombLayer.h VisualBoard.h

# Output executable
EXEC = game

# Default rule
all: $(EXEC)

# Linking the object file to create the executable with ncurses
$(EXEC): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(EXEC) $(OBJ) $(LDFLAGS)

# Rule for generating object files
%.o: %.cpp $(DEPS)
	$(CXX) $(CXXFLAGS) -c $<

# Clean rule to remove generated files
clean:
	rm -f $(OBJ) $(EXEC)

