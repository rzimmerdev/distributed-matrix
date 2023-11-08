CC=mpicc
CFLAGS=-fopenmp
TARGET=hybrid_program

all: $(TARGET)

$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).c

clean:
	rm -f $(TARGET)
