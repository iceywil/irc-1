/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wscherre <wscherre@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/29 16:33:11 by wscherre          #+#    #+#             */
/*   Updated: 2025/12/29 16:56:35 by wscherre         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/channel.hpp"
#include "../include/client.hpp"
#include "../include/commands.hpp"
#include "../include/server.hpp"

void	handle_ping(Server *server, Client *client,
		std::vector<std::string> &args)
{
	if (args.empty())
	{
		client->send_reply(":" + server->getServerName() + " PONG "
			+ server->getServerName());
	}
	else
	{
		client->send_reply(":" + server->getServerName() + " PONG "
			+ server->getServerName() + " :" + args[0]);
	}
}
