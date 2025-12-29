/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   who.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wscherre <wscherre@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/09 16:15:02 by mmedjahe          #+#    #+#             */
/*   Updated: 2025/12/29 16:57:16 by wscherre         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/channel.hpp"
#include "../include/client.hpp"
#include "../include/commands.hpp"
#include "../include/server.hpp"
#include <sstream>

void	handle_who(Server *server, Client *client,
		std::vector<std::string> &args)
{
	Channel	*channel;
	Client	*member;
	Client	*target_client;

	if (!client->isRegistered())
	{
		client->send_reply(":" + server->getServerName() + " 451 "
			+ client->getname() + " :You have not registered");
		return ;
	}
	if (args.empty())
	{
		client->send_reply(":" + server->getServerName() + " 461 "
			+ client->getname() + " WHO :Not enough parameters");
		return ;
	}
	std::string target = args[0];
	if (target[0] == '#')
	{
		channel = server->getChannel(target);
		if (!channel)
		{
			client->send_reply(":" + server->getServerName() + " 315 "
				+ client->getname() + " " + target + " :End of WHO list");
			return ;
		}
		const std::vector<Client *> &clients = channel->getClients();
		for (size_t i = 0; i < clients.size(); ++i)
		{
			member = clients[i];
			std::string prefix = channel->isOperator(member) ? "@" : "";
			client->send_reply(":" + server->getServerName() + " 352 "
				+ client->getname() + " " + target + " " + member->getUsername()
				+ " " + member->getip() + " " + server->getServerName() + " "
				+ member->getname() + " H" + prefix + " :0 "
				+ member->getUsername());
		}
		client->send_reply(":" + server->getServerName() + " 315 "
			+ client->getname() + " " + target + " :End of WHO list");
	}
	else
	{
		target_client = server->getClientByNick(target);
		if (target_client)
		{
			client->send_reply(":" + server->getServerName() + " 352 "
				+ client->getname() + " * " + target_client->getUsername() + " "
				+ target_client->getip() + " " + server->getServerName() + " "
				+ target_client->getname() + " H :0 "
				+ target_client->getUsername());
		}
		client->send_reply(":" + server->getServerName() + " 315 "
			+ client->getname() + " " + target + " :End of WHO list");
	}
}
