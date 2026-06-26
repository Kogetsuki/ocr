NAME = libxor.a

SRCS = XOR.c launch.c

OBJS = $(SRCS:.c=.o)

all: $(NAME)

$(NAME): $(OBJS)
	ar rcs $@ $^

%.o: %.c
	gcc -Wall -Wextra -Werror -c $< -o $@

clean:
	rm -f $(OBJS) *.d *.a

fclean: clean
	rm -f $(NAME)