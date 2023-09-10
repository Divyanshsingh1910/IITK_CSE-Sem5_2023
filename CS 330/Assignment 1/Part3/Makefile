SRC := Testcases

SOURCES := $(wildcard $(SRC)/*.c)
OBJ = $(patsubst %.c,%.o,$(SOURCES))
EXEC = $(patsubst %.c,%,$(SOURCES))

all: $(OBJ) mylib.o $(EXEC)

%: %.o mylib.o
	$(CC) $< mylib.o -o $@

%.o: %.c
	$(CC) -c $< -o $@

mylib.o: mylib.c
	$(CC) -c mylib.c

clean:
	rm mylib.o
	rm $(OBJ)
	rm $(EXEC)
