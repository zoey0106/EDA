CC=g++
CFLAGS=-Wall -std=c++11 -Ofast
INCLUDES=
HEADERS=$(wildcard src/*.h)
LFLAGS=
LIBS=
SOURCES=$(wildcard src/*.cpp)
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=bin/hw2
OBJ_DIR=src
all: $(SOURCES) $(EXECUTABLE)
$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) $(INCLUDES) $(OBJECTS) -o $@ $(LFLAGS) $(LIBS)

%.o: %.cpp $(HEADERS)
	$(CC) $(FLAGS) -c $< -o $@
clean:
	rm -f $(OBJ_DIR)/*.o $(EXECUTABLE)