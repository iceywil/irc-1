/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   whois.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wscherre <wscherre@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/29 16:33:11 by wscherre          #+#    #+#             */
/*   Updated: 2025/12/29 16:57:20 by wscherre         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/channel.hpp"
#include "../include/client.hpp"
#include "../include/commands.hpp"
#include "../include/server.hpp"

void	handle_whois(Server *server, Client *client,
		std::vector<std::string> &args)
{
	Client	*target;

	if (!client->isRegistered())
	{
		client->send_reply(":" + server->getServerName() + " 451 "
			+ client->getname() + " :You have not registered");
		return ;
	}
	if (args.empty())
	{
		client->send_reply(":" + server->getServerName() + " 431 "
			+ client->getname() + " :No nickname given");
		return ;
	}
	std::string target_nick = args[0];
	target = server->getClientByNick(target_nick);
	if (!target)
	{
		client->send_reply(":" + server->getServerName() + " 401 "
			+ client->getname() + " " + target_nick + " :No such nick/channel");
		client->send_reply(":" + server->getServerName() + " 318 "
			+ client->getname() + " " + target_nick + " :End of WHOIS list");
		return ;
	}
	client->send_reply(":" + server->getServerName() + " 311 "
		+ client->getname() + " " + target->getname() + " "
		+ target->getUsername() + " " + target->getip() + " * :"
		+ target->getUsername());
	client->send_reply(":" + server->getServerName() + " 312 "
		+ client->getname() + " " + target->getname() + " "
		+ server->getServerName() + " :IRC Server");
	client->send_reply(":" + server->getServerName() + " 318 "
		+ client->getname() + " " + target->getname() + " :End of WHOIS list");
}
