CC		= g++ -std=c++11
CFLAGS	= -Wall -Wextra -pedantic -Iinclude
LFLAGS	= -lreadline

CFORMAT = clang-format

SRC		= $(wildcard ./src/*.cpp)
OBJ		= $(subst .cpp,.o,$(SRC))
EXEC 	= KeplerKV

.PHONY = all clean format, verify_format

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(LFLAGS)

src/%.o: src/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

format:
	$(CFORMAT) -i -style=file src/*.cpp include/*.h

verify_format:
	$(CFORMAT) -i -style=file src/*.cpp include/*.h --dry-run --Werror


clean:
	rm -rf *.o ./src/*.o KeplerKV
