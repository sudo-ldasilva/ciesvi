EXEC=build/ciesvi
C_FILES=src/main.c

all: $(EXEC)

$(EXEC): $(C_FILES)
	mkdir -p build
	gcc $^ -o $@
