CXX      := g++
CXXFLAGS := -std=c++20 -Wall -Wextra -pedantic -Ilibs
LDLIBS   := -lcurl -pthread
TARGET   := pagerduty

SRC := $(wildcard *.cpp)
OBJ := $(SRC:.cpp=.o)

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDLIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJ) $(TARGET)
