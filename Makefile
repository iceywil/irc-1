# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: wscherre <wscherre@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/11/06 18:17:33 by wscherre          #+#    #+#              #
#    Updated: 2025/12/29 17:00:04 by wscherre         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = ircserv

CC = c++
CFLAGS = -Wall -Wextra -Werror -std=c++98

SRCS =  src/main.cpp \
        src/server.cpp \
        src/client.cpp \
        src/channel.cpp \
		commands/cap.cpp \
		commands/join.cpp \
		commands/nick.cpp \
		commands/pass.cpp \
		commands/privmsg.cpp \
		commands/quit.cpp \
		commands/user.cpp \
		commands/ping.cpp \
		commands/invite.cpp \
		commands/topic.cpp \
		commands/kick.cpp \
		commands/mode.cpp \
		commands/whois.cpp \
		commands/part.cpp \
		commands/who.cpp

		
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
