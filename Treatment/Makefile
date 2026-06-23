NAME = libtreatment.a

SRCS = toBlackWhite.c pixel_operations.c new_try.c line_lib.c word_lib.c seg.c

OBJS = $(SRCS:.c=.o)

CFLAGS = -Wall -Wextra -Werror `sdl-config --cflags`

all: $(NAME)

$(NAME): $(OBJS)
	ar rcs $@ $^

%.o: %.c
	gcc $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) *.d

fclean: clean
	rm -f $(NAME)