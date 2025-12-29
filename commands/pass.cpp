/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pass.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wscherre <wscherre@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/29 16:33:11 by wscherre          #+#    #+#             */
/*   Updated: 2025/12/29 16:56:31 by wscherre         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/commands.hpp"
#include "../include/client.hpp"
#include "../include/server.hpp"

void handle_pass(Server *server, Client *client, std::vector<std::string> &args)
{
	if (client->isRegistered())
	{
		client->send_reply(":" + server->getServerName() + " 462 " + client->getname() + " :You may not reregister");
		return;
	}

	if (args.empty())
	{
		client->send_reply(":" + server->getServerName() + " 461 " + (client->getname().empty() ? "*" : client->getname()) + " PASS :Not enough parameters");
		return;
	}

	if (server->getPassword() == args[0])
		client->setHasValidPassword(true);
	else
		client->send_reply(":" + server->getServerName() + " 464 " + (client->getname().empty() ? "*" : client->getname()) + " :Password incorrect");
}
