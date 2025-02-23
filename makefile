CFLAGS = -Wall -Wextra -pedantic
LDFLAGS = 
RELEASE_FLAGS = -march=native -O3
DEBUG_FLAGS = -g3 -O0

SRCDIR = src
SRCS = $(wildcard ${SRCDIR}/*.c)
TARGET = s4ge

default: debug

run: $(TARGET)
	./$(TARGET)

$(TARGET):
	gcc $(SRCS) $(CFLAGS) $(RELEASE_FLAGS) $(LDFLAGS) -o $(TARGET)

debug:
	gcc $(SRCS) $(CFLAGS) $(DEBUG_FLAGS) $(LDFLAGS) -o $(TARGET)

clean:
	rm -f *.o $(TARGET)

.PHONY: clean debug
