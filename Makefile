CXX = g++
CXXFLAGS = -Wall -std=c++11
SFML_LIBS = -lsfml-graphics -lsfml-window -lsfml-system

TARGET = arcade

all: $(TARGET)

$(TARGET): main.cpp snake.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@ $(SFML_LIBS)

fclean:
	rm -f $(TARGET)

re: fclean all