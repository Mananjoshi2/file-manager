# Makefile for File Manager Application
# Uses wx-config to get compiler and linker flags for wxWidgets

CXX = g++
CXXFLAGS = -std=c++17 -Wall -g
TARGET = filemanager
SOURCES = FileManagerApp.cpp FileManagerFrame.cpp
OBJECTS = $(SOURCES:.cpp=.o)

# Get wxWidgets configuration
WXCONFIG = wx-config
WXFLAGS = $(shell $(WXCONFIG) --cxxflags)
WXLIBS = $(shell $(WXCONFIG) --libs)

# Default target
all: $(TARGET)

# Link the executable
$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET) $(WXLIBS)

# Compile source files to object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(WXFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -f $(OBJECTS) $(TARGET)

# Phony targets
.PHONY: all clean

