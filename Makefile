CXX = g++
CXXFLAGS = -std=c++23 -Wall -Wextra -O2

TARGET = raytracing
SRC = ./src/raytracing.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)