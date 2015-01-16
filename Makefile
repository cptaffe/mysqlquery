
# mysql compiler flags
MYSQLCFLAGS = -I/usr/local/mysql/include
MYSQLLNKFLAGS = -L/usr/local/mysql/lib -lmysqlclient -lz

# compiler flags
CFLAGS += $(MYSQLCFLAGS)
LNKFLAGS = $(MYSQLLNKFLAGS)

SRC = main.c
OBJ = $(SRC:.c=.o)
BIN = esql

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) $(LNKFLAGS) -o $(BIN) $(OBJ)

clean:
	$(RM) $(OBJ) $(BIN)
