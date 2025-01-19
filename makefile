CFLAGS = -Wall -Wextra -pedantic
LDFLAGS = 
RELEASE_FLAGS = -march=native -O3
DEBUG_FLAGS = -g3 -O0

SRCDIR = src
LIBDIR = lib
SRCS = $(wildcard ${SRCDIR}/*.c)
TARGET = s4ge

default: debug

run: $(TARGET)
	./$(TARGET)

$(TARGET): $(LIBARCS)
	gcc $(SRCS) $(CFLAGS) $(RELEASE_FLAGS) $(LDFLAGS) -o $(TARGET)

debug: $(TARGET)
	gcc $(SRCS) $(CFLAGS) $(DEBUG_FLAGS) $(LDFLAGS) -o $(TARGET)

clean:
	rm -f *.o $(TARGET)
	$(MAKE) -C ${TBDIR} clean

.PHONY: clean debug
