CC		= g++ -std=c++17
CFLAGS	= -Wall -Wextra -pedantic -Iinclude
LFLAGS	=

SRC		= $(wildcard ./src/*.cpp)
OBJ		= $(subst .cpp,.o,$(SRC))
EXEC 	= KeplerKV

.PHONY = all clean

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(LFLAGS)

src/%.o: src/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf *.o ./src/*.o KeplerKV
