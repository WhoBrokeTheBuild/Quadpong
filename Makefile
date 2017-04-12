
CONF ?= debug

ifeq ($(CONF),debug)
  CFLAGS += -g
else
  CFLAGS += -O2 -DNDEBUG
endif

CFLAGS  += -Wall -Wextra -pedantic -Wno-unused-parameter -std=c11 -D_GNU_SOURCE -D_XOPEN_SOURCE=700
LDFLAGS +=
LDLIBS  += -lpng -lz -lm

## SDL2
CFLAGS  += $(shell sdl2-config --cflags)
LDLIBS  += $(shell sdl2-config --libs) -lSDL2_image -lSDL2_ttf

SRC_DIR = src
BIN_DIR = bin
OBJ_DIR = obj
DEP_DIR = .dep

SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC))
OUT = $(BIN_DIR)/quadpong

$(shell mkdir -p $(SRC_DIR) $(BIN_DIR) $(OBJ_DIR) $(DEP_DIR))

all: $(OUT)

clean:
	rm $(OBJ) $(OUT)

run:
	@$(OUT)

$(OUT): $(OBJ)
	$(CC) $(LDFLAGS) -o $@ $(LDLIBS) $^

$(OBJ_DIR)/%.o : $(SRC_DIR)/%.c
$(OBJ_DIR)/%.o : $(SRC_DIR)/%.c $(DEP_DIR)/%.d
	$(CC) $(CFLAGS) -MM -MP -MT $@ -o $(DEP_DIR)/$*.d $<
	$(CC) $(CFLAGS) -c -o $@ $< 

$(DEP_DIR)/%.d: ;
.PRECIOUS: $(DEP_DIR)/%.d
-include $(patsubst $(SRC_DIR)/%.c, $(DEP_DIR)/%.d, $(SRC))
