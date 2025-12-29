/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   part.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wscherre <wscherre@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/29 16:33:11 by wscherre          #+#    #+#             */
/*   Updated: 2025/12/29 16:56:26 by wscherre         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/channel.hpp"
#include "../include/client.hpp"
#include "../include/commands.hpp"
#include "../include/server.hpp"
#include <sstream>

void	handle_part(Server *server, Client *client,
		std::vector<std::string> &args)
{
	Channel	*channel;

	if (!client->isRegistered())
	{
		client->send_reply(":" + server->getServerName() + " 451 "
			+ client->getname() + " :You have not registered");
		return ;
	}
	if (args.empty())
	{
		client->send_reply(":" + server->getServerName() + " 461 "
			+ client->getname() + " PART :Not enough parameters");
		return ;
	}
	std::string channel_list = args[0];
	std::string reason = (args.size() > 1) ? args[1] : client->getname();
	std::stringstream ss(channel_list);
	std::string channel_name;
	while (std::getline(ss, channel_name, ','))
	{
		channel = server->getChannel(channel_name);
		if (!channel)
		{
			client->send_reply(":" + server->getServerName() + " 403 "
				+ client->getname() + " " + channel_name + " :No such channel");
			continue ;
		}
		if (!channel->isClientInChannel(client))
		{
			client->send_reply(":" + server->getServerName() + " 442 "
				+ client->getname() + " " + channel_name
				+ " :You're not on that channel");
			continue ;
		}
		std::string part_msg = ":" + client->getname() + "!"
			+ client->getUsername() + "@" + client->getip() + " PART "
			+ channel_name + " :" + reason;
		channel->broadcast(part_msg);
		channel->removeClient(client);
	}
}
