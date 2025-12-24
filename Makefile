# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: wscherre <wscherre@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/11/06 18:17:33 by wscherre          #+#    #+#              #
#    Updated: 2025/12/24 10:30:00 by you              ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = ircserv

CC = c++
CFLAGS = -Wall -Wextra -Werror -std=c++98

SRCS =  main.cpp \
        server.cpp \
        client.cpp \
        channel.cpp \
        commands.cpp

OBJS = $(SRCS:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
