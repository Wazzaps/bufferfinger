C_SRC += c/bufferfinger.c

C_EXAMPLE_SRC += c_example/main.c

CC := gcc

## -------

C_OBJ := $(patsubst %.c,build/%.o,$(C_SRC))
C_EXAMPLE_OBJ := $(patsubst %.c,build/%.o,$(C_EXAMPLE_SRC))

all: build/bufferfinger

build/bufferfinger: $(C_OBJ) $(C_EXAMPLE_OBJ)
	@mkdir -p $(@D)
	$(CC) $^ -o $@

build/%.o: %.c
	@mkdir -p $(@D)
	$(CC) -I./c/ -c $< -o $@

.PHONY: clean
clean:
	rm -rfv build