/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mode.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wscherre <wscherre@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/29 16:33:11 by wscherre          #+#    #+#             */
/*   Updated: 2025/12/29 16:56:17 by wscherre         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/channel.hpp"
#include "../include/client.hpp"
#include "../include/commands.hpp"
#include "../include/server.hpp"
#include <sstream>

void	handle_mode(Server *server, Client *client,
		std::vector<std::string> &args)
{
	Channel	*channel;
	bool	adding_mode;
	size_t	arg_index;
	bool	mode_applied;
	char	mode;
	Client	*target;

	if (!client->isRegistered())
	{
		client->send_reply(":" + server->getServerName() + " 451 "
			+ client->getname() + " :You have not registered");
		return ;
	}
	if (args.empty())
	{
		client->send_reply(":" + server->getServerName() + " 461 "
			+ client->getname() + " MODE :Not enough parameters");
		return ;
	}
	const std::string &target_name = args[0];
	if (target_name[0] != '#')
	{
		if (args.size() >= 2 && target_name == client->getname())
		{
			return ;
		}
		client->send_reply(":" + server->getServerName() + " 221 "
			+ client->getname() + " +");
		return ;
	}
	const std::string &channel_name = target_name;
	channel = server->getChannel(channel_name);
	if (!channel)
	{
		client->send_reply(":" + server->getServerName() + " 403 "
			+ client->getname() + " " + channel_name + " :No such channel");
		return ;
	}
	if (args.size() < 2)
	{
		std::string modes = "+";
		std::string params = "";
		if (channel->isInviteOnly())
			modes += "i";
		if (channel->isTopicRestricted())
			modes += "t";
		if (!channel->getPassword().empty())
		{
			modes += "k";
			params += " " + channel->getPassword();
		}
		if (channel->getUserLimit() > 0)
		{
			modes += "l";
			std::ostringstream oss;
			oss << channel->getUserLimit();
			params += " " + oss.str();
		}
		client->send_reply(":" + server->getServerName() + " 324 "
			+ client->getname() + " " + channel_name + " " + modes + params);
		return ;
	}
	const std::string &modestring = args[1];
	if (!channel->isOperator(client))
	{
		client->send_reply(":" + server->getServerName() + " 482 "
			+ client->getname() + " " + channel_name
			+ " :You're not channel operator");
		return ;
	}
	if (modestring.empty() || (modestring[0] != '+' && modestring[0] != '-'))
	{
		client->send_reply(":" + server->getServerName() + " 501 "
			+ client->getname() + " :Unknown MODE flag");
		return ;
	}
	adding_mode = (modestring[0] == '+');
	arg_index = 2;
	std::string applied_modes = "";
	std::string applied_params = "";
	mode_applied = false;
	for (size_t i = 1; i < modestring.length(); ++i)
	{
		mode = modestring[i];
		switch (mode)
		{
		case 'i':
			channel->setInviteOnly(adding_mode);
			applied_modes += mode;
			mode_applied = true;
			mode_applied = true;
			break ;
		case 't':
			channel->setTopicRestricted(adding_mode);
			applied_modes += mode;
			mode_applied = true;
			break ;
		case 'k':
			if (adding_mode)
			{
				if (arg_index < args.size())
				{
					channel->setPassword(args[arg_index]);
					applied_modes += mode;
					applied_params += " " + args[arg_index];
					mode_applied = true;
					arg_index++;
				}
				else
					client->send_reply(":" + server->getServerName() + " 461 "
						+ client->getname() + " MODE +k :Not enough parameters");
			}
			else
			{
				channel->setPassword("");
				applied_modes += mode;
				mode_applied = true;
			}
			break ;
		case 'o':
			if (arg_index < args.size())
			{
				target = server->getClientByNick(args[arg_index]);
				if (!target)
					client->send_reply(":" + server->getServerName() + " 401 "
						+ client->getname() + " " + args[arg_index]
						+ " :No such nick/channel");
				else if (!channel->isClientInChannel(target))
					client->send_reply(":" + server->getServerName() + " 441 "
						+ client->getname() + " " + args[arg_index] + " "
						+ channel_name + " :They aren't on that channel");
				else
				{
					if (adding_mode)
						channel->addOperator(target);
					else
						channel->removeOperator(target);
					applied_modes += mode;
					applied_params += " " + args[arg_index];
					mode_applied = true;
				}
				arg_index++;
			}
			else
				client->send_reply(":" + server->getServerName() + " 461 "
					+ client->getname() + " MODE +/-o :Not enough parameters");
			break ;
		case 'l':
			if (adding_mode)
			{
				if (arg_index < args.size())
				{
					channel->setUserLimit(atoi(args[arg_index].c_str()));
					applied_modes += mode;
					applied_params += " " + args[arg_index];
					mode_applied = true;
					arg_index++;
				}
				else
					client->send_reply(":" + server->getServerName() + " 461 "
						+ client->getname() + " MODE +l :Not enough parameters");
			}
			else
			{
				channel->setUserLimit(0);
				applied_modes += mode;
				mode_applied = true;
			}
			break ;
		default:
			client->send_reply(":" + server->getServerName() + " 472 "
				+ client->getname() + " " + mode
				+ " :is unknown mode char to me for " + channel_name);
			break ;
		}
	}
	if (mode_applied && !applied_modes.empty())
	{
		std::string mode_prefix = adding_mode ? "+" : "-";
		std::string mode_msg = ":" + client->getname() + " MODE " + channel_name
			+ " " + mode_prefix + applied_modes + applied_params;
		channel->broadcast(mode_msg);
	}
}
