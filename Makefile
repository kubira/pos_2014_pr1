################################################################################
#  Soubor:    Makefile                                                         #
#  Autor:     Radim KUBIŠ, xkubis03                                            #
#  Vytvořeno: 2. dubna 2014                                                    #
#                                                                              #
#  Projekt č. 1 do předmětu Pokročilé operační systémy (POS).                  #
################################################################################

CC=gcc
CFLAGS=-Wall -g -O -std=c99 -pedantic
NAME=proj01

all:
	$(CC) $(CFLAGS) -o $(NAME) $(NAME).c

run:
	./$(NAME)

clean:
	rm -f $(NAME)
