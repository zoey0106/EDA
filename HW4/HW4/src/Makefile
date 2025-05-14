CC=g++
CFLAGS=-Wall -std=c++17 -O0 -g
INCLUDES=
HEADERS=$(wildcard *.hpp)
LFLAGS=
LIBS=
SOURCES=$(wildcard *.cpp)
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=../bin/hw4
OBJ_DIR=""
all: $(SOURCES) $(EXECUTABLE)
$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) $(INCLUDES) $(OBJECTS) -o $@ $(LFLAGS) $(LIBS)

%.o: %.cpp $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@
clean:
	rm -rf $(OBJECTS) $(EXECUTABLE)