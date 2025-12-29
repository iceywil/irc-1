/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   invite.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wscherre <wscherre@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/29 16:33:11 by wscherre          #+#    #+#             */
/*   Updated: 2025/12/29 16:55:57 by wscherre         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/channel.hpp"
#include "../include/client.hpp"
#include "../include/commands.hpp"
#include "../include/server.hpp"

void	handle_invite(Server *server, Client *client,
		std::vector<std::string> &args)
{
	Channel	*channel;
	Client	*target;

	if (!client->isRegistered())
	{
		client->send_reply(":" + server->getServerName() + " 451 "
			+ client->getname() + " :You have not registered");
		return ;
	}
	if (args.size() < 2)
	{
		client->send_reply(":" + server->getServerName() + " 461 "
			+ client->getname() + " INVITE :Not enough parameters");
		return ;
	}
	const std::string &target_nick = args[0];
	const std::string &channel_name = args[1];
	channel = server->getChannel(channel_name);
	if (!channel)
	{
		client->send_reply(":" + server->getServerName() + " 403 "
			+ client->getname() + " " + channel_name + " :No such channel");
		return ;
	}
	if (!channel->isOperator(client))
	{
		client->send_reply(":" + server->getServerName() + " 482 "
			+ client->getname() + " " + channel_name
			+ " :You're not channel operator");
		return ;
	}
	target = server->getClientByNick(target_nick);
	if (!target)
	{
		client->send_reply(":" + server->getServerName() + " 401 "
			+ client->getname() + " " + target_nick + " :No such nick/channel");
		return ;
	}
	if (channel->isClientInChannel(target))
	{
		client->send_reply(":" + server->getServerName() + " 443 "
			+ client->getname() + " " + target_nick + " " + channel_name
			+ " :is already on channel");
		return ;
	}
	channel->addInvited(target);
	client->send_reply(":" + server->getServerName() + " 341 "
		+ client->getname() + " " + target_nick + " " + channel_name);
	target->send_reply(":" + client->getname() + " INVITE " + target_nick + " :"
		+ channel_name);
}
