CXX = g++
CXXFLAGS = -Wall -std=c++11
SFML_LIBS = -lsfml-graphics -lsfml-window -lsfml-system

TARGET = game_menu

all: $(TARGET)

$(TARGET): main.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@ $(SFML_LIBS)

clean:
	rm -f $(TARGET)
