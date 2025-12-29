/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   topic.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wscherre <wscherre@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/29 16:33:11 by wscherre          #+#    #+#             */
/*   Updated: 2025/12/29 16:56:49 by wscherre         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/channel.hpp"
#include "../include/client.hpp"
#include "../include/commands.hpp"
#include "../include/server.hpp"

void	handle_topic(Server *server, Client *client,
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
			+ client->getname() + " TOPIC :Not enough parameters");
		return ;
	}
	const std::string &channel_name = args[0];
	channel = server->getChannel(channel_name);
	if (!channel)
	{
		client->send_reply(":" + server->getServerName() + " 403 "
			+ client->getname() + " " + channel_name + " :No such channel");
		return ;
	}
	if (args.size() == 1)
	{
		if (channel->getTopic().empty())
			client->send_reply(":" + server->getServerName() + " 331 "
				+ client->getname() + " " + channel_name + " :No topic is set");
		else
			client->send_reply(":" + server->getServerName() + " 332 "
				+ client->getname() + " " + channel_name + " :"
				+ channel->getTopic());
	}
	else
	{
		if (channel->isTopicRestricted() && !channel->isOperator(client))
		{
			client->send_reply(":" + server->getServerName() + " 482 "
				+ client->getname() + " " + channel_name
				+ " :You're not channel operator");
			return ;
		}
		const std::string &new_topic = args[1];
		channel->setTopic(new_topic);
		std::string topic_msg = ":" + client->getname() + " TOPIC "
			+ channel_name + " :" + new_topic;
		channel->broadcast(topic_msg);
	}
}
