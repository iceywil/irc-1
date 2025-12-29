/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   nick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wscherre <wscherre@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/29 16:33:11 by wscherre          #+#    #+#             */
/*   Updated: 2025/12/29 16:56:21 by wscherre         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/client.hpp"
#include "../include/commands.hpp"
#include "../include/server.hpp"

static bool	isValidNickname(const std::string &nick)
{
	char	c;

	if (nick.empty() || nick.length() > 9)
		return (false);
	if (nick[0] == '#' || nick[0] == ':' || nick[0] == ' ')
		return (false);
	if (isdigit(nick[0]))
		return (false);
	for (size_t i = 0; i < nick.length(); ++i)
	{
		c = nick[i];
		if (!isalnum(c) && c != '[' && c != ']' && c != '{' && c != '}'
			&& c != '\\' && c != '|' && c != '-' && c != '_')
			return (false);
		if (c == ' ')
			return (false);
	}
	return (true);
}

void	handle_nick(Server *server, Client *client,
		std::vector<std::string> &args)
{
	Client	*existingClient;

	if (!client->hasValidPassword())
	{
		client->send_reply(":" + server->getServerName() + " 464 "
			+ (client->getname().empty() ? "*" : client->getname())
			+ " :Password incorrect");
		return ;
	}
	if (args.empty())
	{
		client->send_reply(":" + server->getServerName() + " 431 "
			+ (client->getname().empty() ? "*" : client->getname())
			+ " :No nickname given");
		return ;
	}
	std::string nick = args[0];
	if (nick.empty())
	{
		client->send_reply(":" + server->getServerName() + " 431 "
			+ (client->getname().empty() ? "*" : client->getname())
			+ " :No nickname given");
		return ;
	}
	if (!isValidNickname(nick))
	{
		client->send_reply(":" + server->getServerName() + " 432 "
			+ (client->getname().empty() ? "*" : client->getname()) + " " + nick
			+ " :Erroneous nickname");
		return ;
	}
	existingClient = server->getClientByNick(nick);
	if (existingClient && existingClient != client)
	{
		client->send_reply(":" + server->getServerName() + " 433 "
			+ (client->getname().empty() ? "*" : client->getname()) + " " + nick
			+ " :Nickname is already in use");
		return ;
	}
	std::string oldNick = client->getname();
	client->setNickname(nick);
	if (client->isRegistered() && !oldNick.empty())
	{
		std::string nickChangeMsg = ":" + oldNick + "!" + client->getUsername()
			+ "@" + client->getip() + " NICK :" + nick;
		client->send_reply(nickChangeMsg);
	}
	if (!client->isRegistered() && !client->getUsername().empty())
	{
		client->setRegistered(true);
		client->send_reply(":" + server->getServerName() + " 001 "
			+ client->getname() + " :Welcome to the " + server->getServerName()
			+ " IRC network " + client->getname());
	}
}
