/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   user.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wscherre <wscherre@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/29 16:33:11 by wscherre          #+#    #+#             */
/*   Updated: 2025/12/29 16:56:54 by wscherre         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/commands.hpp"
#include "../include/client.hpp"
#include "../include/server.hpp"

void handle_user(Server *server, Client *client, std::vector<std::string> &args)
{
	if (!client->hasValidPassword())
	{
		client->send_reply(":" + server->getServerName() + " 464 " + (client->getname().empty() ? "*" : client->getname()) + " :Password incorrect");
		return;
	}
	if (client->isRegistered())
	{
		client->send_reply(":" + server->getServerName() + " 462 " + client->getname() + " :You may not reregister");
		return;
	}
	if (args.size() < 4)
	{
		client->send_reply(":" + server->getServerName() + " 461 " + (client->getname().empty() ? "*" : client->getname()) + " USER :Not enough parameters");
		return;
	}
	client->setUsername(args[0]);
    if (!client->isRegistered() && !client->getname().empty())
    {
        client->setRegistered(true);
        client->send_reply(":" + server->getServerName() + " 001 " + client->getname() + " :Welcome to the " + server->getServerName() + " IRC network " + client->getname());
    }
}