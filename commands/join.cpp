/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   join.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wscherre <wscherre@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/29 16:33:11 by wscherre          #+#    #+#             */
/*   Updated: 2025/12/29 16:56:02 by wscherre         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/client.hpp"
#include "../include/commands.hpp"
#include "../include/server.hpp"
#include "../include/channel.hpp"
#include <sstream>

void handle_join(Server *server, Client *client, std::vector<std::string> &args)
{
	if (!client->isRegistered())
	{
		client->send_reply(":" + server->getServerName() + " 451 " + client->getname() + " :You have not registered");
		return;
	}

    if (args.empty())
    {
        client->send_reply(":" + server->getServerName() + " 461 " + client->getname() + " JOIN :Not enough parameters");
        return;
    }
    
    std::string channel_list = args[0];
    std::string key_list = (args.size() > 1) ? args[1] : "";
    std::stringstream ss_channels(channel_list);
    std::stringstream ss_keys(key_list);
    std::string channel_name;
    std::string key;

    while (std::getline(ss_channels, channel_name, ','))
    {
        std::getline(ss_keys, key, ',');

        if (channel_name[0] != '#')
        {
            client->send_reply(":" + server->getServerName() + " 403 " + client->getname() + " " + channel_name + " :No such channel");
            continue;
        }

        Channel* channel = server->getChannel(channel_name);
        if (!channel)
        {
            server->createChannel(channel_name, client);
            channel = server->getChannel(channel_name);
        }
        else
        {
            if (channel->isClientInChannel(client))
                return;
            if (channel->getUserLimit() > 0 && channel->getClients().size() >= static_cast<size_t>(channel->getUserLimit()))
            {
                client->send_reply(":" + server->getServerName() + " 471 " + client->getname() + " " + channel_name + " :Cannot join channel (+l)");
                continue;
            }
            if (channel->isInviteOnly() && !channel->isInvited(client))
            {
                client->send_reply(":" + server->getServerName() + " 473 " + client->getname() + " " + channel_name + " :Cannot join channel (+i)");
                continue;
            }
            if (!channel->getPassword().empty() && channel->getPassword() != key)
            {
                client->send_reply(":" + server->getServerName() + " 475 " + client->getname() + " " + channel_name + " :Cannot join channel (+k)");
                continue;
            }
            channel->addClient(client);
        }

        std::string join_msg = ":" + client->getname() + "!" + client->getUsername() + "@" + client->getip() + " JOIN " + channel_name;
        channel->broadcast(join_msg);
        
        if (!channel->getTopic().empty())
        {
            client->send_reply(":" + server->getServerName() + " 332 " + client->getname() + " " + channel_name + " :" + channel->getTopic());
        }
        
        std::string nicks;
        const std::vector<Client*>& clients = channel->getClients();
        for (size_t i = 0; i < clients.size(); ++i)
        {
            if (channel->isOperator(clients[i]))
                nicks += "@";
            nicks += clients[i]->getname() + " ";
        }
        if (!nicks.empty())
            nicks.erase(nicks.length() - 1);
        client->send_reply(":" + server->getServerName() + " 353 " + client->getname() + " = " + channel_name + " :" + nicks);
        client->send_reply(":" + server->getServerName() + " 366 " + client->getname() + " " + channel_name + " :End of /NAMES list");
    }
}
