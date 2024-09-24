SRC =  file_explorer.c \
       libc.c \
       main.c \
       resolve_apis.c \
       shellcode.c \
       utils.c
OBJ = $(SRC:.c=.o)
NAME = Laika.exe
CC = C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\Llvm\x64\bin\clang
CFLAGS = -Os -fno-asynchronous-unwind-tables -DUNICODE -D_UNICODE -m32 -nostdlib -DLAIKA -fwritable-strings -D_WIN32_WINNT=0x0501
BIN_DIR = ../bin

all: $(BIN_DIR)/$(NAME)

$(BIN_DIR)/$(NAME): $(OBJ)
	$(MAKE) -C ../pasm -f Makefile_Windows lib
	@if not exist $(BIN_DIR) (mkdir $(BIN_DIR))
	$(CC) $(CFLAGS) -o $(BIN_DIR)/$(NAME) $(OBJ) -L../pasm/bin/ -lpasm

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@if exist *.o (del /q *.o)
	$(MAKE) -C ../pasm -f Makefile_Windows clean

fclean: clean
	@if exist ..\bin (del /q ..\bin\Laika.*)
	$(MAKE) -C ../pasm -f Makefile_Windows fclean

.PHONY: all clean fclean
