# SHELL SECTION
RM := rm -rf

# PRETTY PRINTS SECTION
define print_cc
	$(if $(Q), @echo "[CC]        $(1)")
endef

define print_bin
	$(if $(Q), @echo "[BIN]       $(1)")
endef

define print_rm
    $(if $(Q), @echo "[RM]        $(1)")
endef


# PROJECT TREE
SDIR := ./src
IDIR := ./inc
ADIR := ./app
STD :=--std=c11

# FILES
SRC := $(wildcard $(SDIR)/*.c)

ASRC := $(SRC) $(wildcard $(ADIR)/*.c)
AOBJ := $(ASRC:%.c=%.o)
OBJ := $(AOBJ)

DEPS := $(OBJ:%.o=%.d)

# EXEC
EXEC := main.out

# COMPILATOR SECTION

# By default use gcc
CC ?= gcc

C_FLAGS := -Wall -Wextra

L_FLAGS := -lpthread -lrt -pthread

DEP_FLAGS := -MMD -MP -D_POSIX_C_SOURCE=200112L

H_INC := $(foreach d, $(IDIR), -I$d)

ifeq ($(CC),clang)
	C_FLAGS += -Weverything
else ifeq ($(CC),gcc)
	C_FLAGS += -Wpedantic
endif

ifeq ("$(origin O)", "command line")
	OPT := -O$(O)
else
	OPT := -O3
endif

ifeq ("$(origin G)", "command line")
	GGDB := -ggdb$(G)
else
	GGDB :=
endif

C_FLAGS += $(OPT) $(GGDB) $(DEP_FLAGS)

all: $(EXEC)

$(EXEC): $(AOBJ)
	$(call print_bin,$@)
	$(CC) $(C_FLAGS) $(H_INC) $(L_FLAGS) $(AOBJ) $(STD) -o $@

%.o:%.c %.d
	$(call print_cc,$<)
	$(CC) $(C_FLAGS) $(H_INC) $(STD) -c $< -o $@

clean:
	$(call print_rm,EXEC)
	$(RM) $(EXEC)
	$(call print_rm,OBJ)
	$(RM) $(OBJ)
	$(call print_rm,DEPS)
	$(RM) $(DEPS)

$(DEPS):

include $(wildcard $(DEPS))
