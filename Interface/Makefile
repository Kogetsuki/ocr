CC = gcc
CFLAGS =  -Wall -Wextra -Werror -g \
					`pkg-config --cflags gtk+-3.0 --libs cairo`

LDFLAGS = `pkg-config --libs gtk+-3.0`
LDLIBS =  -L../Treatment -ltreatment \
					-L../XOR -lxor \
					-lSDL_image -lSDL -lm

SRCS = ocr.c
OBJS = $(SRCS:.c=.o)

NAME = ocr

all: libs $(NAME)

libs:
	$(MAKE) -C ../Treatment
	$(MAKE) -C ../XOR

$(NAME): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS) $(LDLIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all