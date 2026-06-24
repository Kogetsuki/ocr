NAME = libtreatment.a

SRCS = toBlackWhite.c pixel_operations.c new_try.c line_lib.c word_lib.c seg.c

OBJS = $(SRCS:.c=.o)

SDL_CFLAGS := $(shell pkg-config --cflags sdl SDL_image 2>/dev/null || sdl-config --cflags 2>/dev/null || echo "-I/usr/include/SDL")
CFLAGS = -Wall -Wextra -Werror $(SDL_CFLAGS)

all: $(NAME)

$(NAME): $(OBJS)
	ar rcs $@ $^

%.o: %.c
	gcc $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) *.d

fclean: clean
	rm -f $(NAME)