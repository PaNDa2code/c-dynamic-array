CC=gcc
CF=-Iinclude -Ofast -lpthread -fsanitize=address -g
TARGET=bin/excutable
OBJ_DIR=obj/
SRC_DIR=src/

OBJECT_FILES:=$(subst .c,.o,$(wildcard $(SRC_DIR)*.c))
OBJECT_FILES:=$(subst $(SRC_DIR),$(OBJ_DIR), $(OBJECT_FILES))

all: $(TARGET) 

clean:
	@rm -f bin/* obj/*

.PHONY: all clean

obj/%.o: src/%.c 
	@echo "[*] Compiling $<"
	@$(CC) -c $(CF) -o $@ $<

$(TARGET): $(OBJECT_FILES)
	@echo "[*] Linking all together"
	@$(CC) $(CF) -o $@ $^
