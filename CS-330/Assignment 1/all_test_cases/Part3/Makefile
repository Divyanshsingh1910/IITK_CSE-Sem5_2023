SRC := Testcases

SOURCES := $(wildcard $(SRC)/*.c)
OBJ = $(patsubst %.c,%.o,$(SOURCES))
EXEC = $(patsubst %.c,%,$(SOURCES))

all: $(OBJ) mylib.o $(EXEC)

%: %.o mylib.o
	$(CC) $< mylib.o -lm -o $@

%.o: %.c
	$(CC) -c $< -lm -o $@

mylib.o: mylib.c
	$(CC) -c mylib.c

clean:
	rm mylib.o
	rm $(OBJ)
	rm $(EXEC)
