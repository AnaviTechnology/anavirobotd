CC=gcc
CFLAGS=-I. -Wall
DEPS = 
OBJ = motors.o robot.o
EXTRA_LIBS=-lwiringPi -lpthread -lpaho-mqtt3c

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

robot: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(EXTRA_LIBS)

.PHONY: clean

clean:
	rm -f robot $(OBJ)
