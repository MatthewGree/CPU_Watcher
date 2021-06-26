SDIR := ./src
IDIR := ./inc
ADIR := ./app

SRC := $(wildcard $(SDIR)/*c)
ASRC := $(SRC) $(wildcard $(ADIR)/*.c)
AOBJ := $(ASRC:%.c=%.o)

EXEC := main.out

CC ?= clang
C_FLAGS := -Wall -Wextra -Wpedantic

H_INC := $(foreach d, $(IDIR), -I$d)

ifeq ($(CC), clang)
	C_FLAGS += -Weverything
endif

all: $(EXEC)
	./$(EXEC)

$(EXEC): $(AOBJ)
	$(CC) $(CFLAGS) $(H_INC) $(AOBJ) -o $(EXEC)

clean:
	rm $(EXEC) $(AOBJ)

