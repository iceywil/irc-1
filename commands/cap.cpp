/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cap.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wscherre <wscherre@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/29 16:33:11 by wscherre          #+#    #+#             */
/*   Updated: 2025/12/29 16:55:52 by wscherre         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/channel.hpp"
#include "../include/client.hpp"
#include "../include/commands.hpp"
#include "../include/server.hpp"
#include <sstream>

void	handle_cap(Server *server, Client *client,
		std::vector<std::string> &args)
{
	if (args.empty())
		return ;
	std::string subcmd = args[0];
	if (subcmd == "LS")
	{
		client->send_reply(":" + server->getServerName() + " CAP * LS :");
	}
	else if (subcmd == "END")
	{
		return ;
	}
	else if (subcmd == "REQ")
	{
		if (args.size() > 1)
			client->send_reply(":" + server->getServerName() + " CAP * NAK :"
				+ args[1]);
	}
}
