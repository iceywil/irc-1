/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commands.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wscherre <wscherre@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/09 16:15:02 by mmedjahe          #+#    #+#             */
/*   Updated: 2025/12/28 19:17:12 by wscherre         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "commands.hpp"
#include "server.hpp"
#include "client.hpp"
#include "channel.hpp"
#include <sstream>

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

static bool isValidNickname(const std::string &nick)
{
	if (nick.empty() || nick.length() > 9)
		return false;
	
	if (nick[0] == '#' || nick[0] == ':' || nick[0] == ' ')
		return false;
	
	if (isdigit(nick[0]))
		return false;
	
	for (size_t i = 0; i < nick.length(); ++i)
	{
		char c = nick[i];
		if (!isalnum(c) && c != '[' && c != ']' && c != '{' && c != '}' && c != '\\' && c != '|' && c != '-' && c != '_')
			return false;
		if (c == ' ')
			return false;
	}
	return true;
}

void handle_nick(Server *server, Client *client, std::vector<std::string> &args)
{
	if (!client->hasValidPassword())
	{
		client->send_reply(":" + server->getServerName() + " 464 " + (client->getname().empty() ? "*" : client->getname()) + " :Password incorrect");
		return;
	}
	if (args.empty())
	{
		client->send_reply(":" + server->getServerName() + " 431 " + (client->getname().empty() ? "*" : client->getname()) + " :No nickname given");
		return;
	}
	
	std::string nick = args[0];
	
	if (nick.empty())
	{
		client->send_reply(":" + server->getServerName() + " 431 " + (client->getname().empty() ? "*" : client->getname()) + " :No nickname given");
		return;
	}
	
	if (!isValidNickname(nick))
	{
		client->send_reply(":" + server->getServerName() + " 432 " + (client->getname().empty() ? "*" : client->getname()) + " " + nick + " :Erroneous nickname");
		return;
	}
	
	Client* existingClient = server->getClientByNick(nick);
	if (existingClient && existingClient != client)
	{
		client->send_reply(":" + server->getServerName() + " 433 " + (client->getname().empty() ? "*" : client->getname()) + " " + nick + " :Nickname is already in use");
		return;
	}
	
	std::string oldNick = client->getname();
	
	client->setNickname(nick);
	
	if (client->isRegistered() && !oldNick.empty())
	{
		std::string nickChangeMsg = ":" + oldNick + "!" + client->getUsername() + "@" + client->getip() + " NICK :" + nick;
		client->send_reply(nickChangeMsg);
	}
	
    if (!client->isRegistered() && !client->getUsername().empty())
    {
        client->setRegistered(true);
        client->send_reply(":" + server->getServerName() + " 001 " + client->getname() + " :Welcome to the " + server->getServerName() + " IRC network " + client->getname());
    }
}

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

void handle_privmsg(Server *server, Client *client, std::vector<std::string> &args)
{
	if (!client->isRegistered())
	{
		client->send_reply(":" + server->getServerName() + " 451 " + client->getname() + " :You have not registered");
		return;
	}

	if (args.size() < 2)
    {
        if (args.empty())
            client->send_reply(":" + server->getServerName() + " 411 " + client->getname() + " :No recipient given (PRIVMSG)");
        else
            client->send_reply(":" + server->getServerName() + " 412 " + client->getname() + " :No text to send");
        return;
    }

    std::string target_name = args[0];
    std::string message = args[1];
    
    std::string full_message = ":" + client->getname() + " PRIVMSG " + target_name + " :" + message;

    if (target_name[0] == '#')
    {
        Channel* channel = server->getChannel(target_name);
        if (!channel)
            client->send_reply(":" + server->getServerName() + " 403 " + client->getname() + " " + target_name + " :No such channel");
        else if (!channel->isClientInChannel(client))
            client->send_reply(":" + server->getServerName() + " 404 " + client->getname() + " " + target_name + " :Cannot send to channel");
        else
            channel->broadcast(full_message, client);
    }
    else
    {
        Client* target_client = server->getClientByNick(target_name);
        if (!target_client)
            client->send_reply(":" + server->getServerName() + " 401 " + client->getname() + " " + target_name + " :No such nick/channel");
        else
            target_client->send_reply(full_message + "\r\n");
    }
}

void handle_kick(Server *server, Client *client, std::vector<std::string> &args)
{
	if (!client->isRegistered())
	{
		client->send_reply(":" + server->getServerName() + " 451 " + client->getname() + " :You have not registered");
		return;
	}
    if (args.size() < 2)
    {
        client->send_reply(":" + server->getServerName() + " 461 " + client->getname() + " KICK :Not enough parameters");
        return;
    }
    const std::string& channel_name = args[0];
    const std::string& target_nick = args[1];
    std::string reason = (args.size() > 2) ? args[2] : "Kicked by operator";

    Channel* channel = server->getChannel(channel_name);
    if (!channel)
    {
        client->send_reply(":" + server->getServerName() + " 403 " + client->getname() + " " + channel_name + " :No such channel");
        return;
    }
    if (!channel->isOperator(client))
    {
        client->send_reply(":" + server->getServerName() + " 482 " + client->getname() + " " + channel_name + " :You're not channel operator");
        return;
    }
    Client* target = server->getClientByNick(target_nick);
    if (!target)
    {
        client->send_reply(":" + server->getServerName() + " 401 " + client->getname() + " " + target_nick + " :No such nick/channel");
        return;
    }
    if (!channel->isClientInChannel(target))
    {
        client->send_reply(":" + server->getServerName() + " 441 " + client->getname() + " " + target_nick + " " + channel_name + " :They aren't on that channel");
        return;
    }

    std::string kick_msg = ":" + client->getname() + " KICK " + channel_name + " " + target_nick + " :" + reason;
    channel->broadcast(kick_msg);
    channel->removeClient(target);
}

void handle_invite(Server *server, Client *client, std::vector<std::string> &args)
{
	if (!client->isRegistered())
	{
		client->send_reply(":" + server->getServerName() + " 451 " + client->getname() + " :You have not registered");
		return;
	}
    if (args.size() < 2)
    {
        client->send_reply(":" + server->getServerName() + " 461 " + client->getname() + " INVITE :Not enough parameters");
        return;
    }
    
    const std::string& target_nick = args[0];
    const std::string& channel_name = args[1];

    Channel* channel = server->getChannel(channel_name);
    if (!channel)
    {
        client->send_reply(":" + server->getServerName() + " 403 " + client->getname() + " " + channel_name + " :No such channel");
        return;
    }
    if (!channel->isOperator(client))
    {
        client->send_reply(":" + server->getServerName() + " 482 " + client->getname() + " " + channel_name + " :You're not channel operator");
        return;
    }
    Client* target = server->getClientByNick(target_nick);
    if (!target)
    {
        client->send_reply(":" + server->getServerName() + " 401 " + client->getname() + " " + target_nick + " :No such nick/channel");
        return;
    }
    if (channel->isClientInChannel(target))
    {
        client->send_reply(":" + server->getServerName() + " 443 " + client->getname() + " " + target_nick + " " + channel_name + " :is already on channel");
        return;
    }
    
    channel->addInvited(target);
    client->send_reply(":" + server->getServerName() + " 341 " + client->getname() + " " + target_nick + " " + channel_name);
    target->send_reply(":" + client->getname() + " INVITE " + target_nick + " :" + channel_name);
}

void handle_topic(Server *server, Client *client, std::vector<std::string> &args)
{
	if (!client->isRegistered())
	{
		client->send_reply(":" + server->getServerName() + " 451 " + client->getname() + " :You have not registered");
		return;
	}
    if (args.empty())
    {
        client->send_reply(":" + server->getServerName() + " 461 " + client->getname() + " TOPIC :Not enough parameters");
        return;
    }
    
    const std::string& channel_name = args[0];
    Channel* channel = server->getChannel(channel_name);

    if (!channel)
    {
        client->send_reply(":" + server->getServerName() + " 403 " + client->getname() + " " + channel_name + " :No such channel");
        return;
    }

    if (args.size() == 1)
    {
        if (channel->getTopic().empty())
            client->send_reply(":" + server->getServerName() + " 331 " + client->getname() + " " + channel_name + " :No topic is set");
        else
            client->send_reply(":" + server->getServerName() + " 332 " + client->getname() + " " + channel_name + " :" + channel->getTopic());
    }
    else
    {
        if (channel->isTopicRestricted() && !channel->isOperator(client))
        {
            client->send_reply(":" + server->getServerName() + " 482 " + client->getname() + " " + channel_name + " :You're not channel operator");
            return;
        }
        const std::string& new_topic = args[1];
        channel->setTopic(new_topic);
        std::string topic_msg = ":" + client->getname() + " TOPIC " + channel_name + " :" + new_topic;
        channel->broadcast(topic_msg);
    }
}

void handle_mode(Server *server, Client *client, std::vector<std::string> &args)
{
    if (!client->isRegistered())
	{
		client->send_reply(":" + server->getServerName() + " 451 " + client->getname() + " :You have not registered");
		return;
	}
	if (args.empty())
    {
        client->send_reply(":" + server->getServerName() + " 461 " + client->getname() + " MODE :Not enough parameters");
        return;
    }

    const std::string& target = args[0];
    
    if (target[0] != '#')
    {
        if (args.size() >= 2 && target == client->getname())
        {
            return;
        }
        client->send_reply(":" + server->getServerName() + " 221 " + client->getname() + " +");
        return;
    }
    
    const std::string& channel_name = target;
    
    Channel* channel = server->getChannel(channel_name);
    if (!channel)
    {
        client->send_reply(":" + server->getServerName() + " 403 " + client->getname() + " " + channel_name + " :No such channel");
        return;
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
        
        client->send_reply(":" + server->getServerName() + " 324 " + client->getname() + " " + channel_name + " " + modes + params);
        return;
    }
    
    const std::string& modestring = args[1];

    if (!channel->isOperator(client))
    {
        client->send_reply(":" + server->getServerName() + " 482 " + client->getname() + " " + channel_name + " :You're not channel operator");
        return;
    }

    if (modestring.empty() || (modestring[0] != '+' && modestring[0] != '-'))
    {
        client->send_reply(":" + server->getServerName() + " 501 " + client->getname() + " :Unknown MODE flag");
        return;
    }

    bool adding_mode = (modestring[0] == '+');
    size_t arg_index = 2;
    std::string applied_modes = "";
    std::string applied_params = "";
    bool mode_applied = false;

    for (size_t i = 1; i < modestring.length(); ++i)
    {
        char mode = modestring[i];
        switch (mode)
        {
            case 'i':
                channel->setInviteOnly(adding_mode);
                applied_modes += mode;
                mode_applied = true;
                mode_applied = true;
                break;
            case 't':
                channel->setTopicRestricted(adding_mode);
                applied_modes += mode;
                mode_applied = true;
                break;
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
                        client->send_reply(":" + server->getServerName() + " 461 " + client->getname() + " MODE +k :Not enough parameters");
                }
                else
                {
                    channel->setPassword("");
                    applied_modes += mode;
                    mode_applied = true;
                }
                break;
            case 'o':
                if (arg_index < args.size())
                {
                    Client* target = server->getClientByNick(args[arg_index]);
                    if (!target)
                        client->send_reply(":" + server->getServerName() + " 401 " + client->getname() + " " + args[arg_index] + " :No such nick/channel");
                    else if (!channel->isClientInChannel(target))
                        client->send_reply(":" + server->getServerName() + " 441 " + client->getname() + " " + args[arg_index] + " " + channel_name + " :They aren't on that channel");
                    else
                    {
                        if (adding_mode) channel->addOperator(target);
                        else channel->removeOperator(target);
                        applied_modes += mode;
                        applied_params += " " + args[arg_index];
                        mode_applied = true;
                    }
                    arg_index++;
                }
                else
                    client->send_reply(":" + server->getServerName() + " 461 " + client->getname() + " MODE +/-o :Not enough parameters");
                break;
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
                        client->send_reply(":" + server->getServerName() + " 461 " + client->getname() + " MODE +l :Not enough parameters");
                }
                else
                {
                    channel->setUserLimit(0);
                    applied_modes += mode;
                    mode_applied = true;
                }
                break;
            default:
                client->send_reply(":" + server->getServerName() + " 472 " + client->getname() + " " + mode + " :is unknown mode char to me for " + channel_name);
                break;
        }
    }
    
    // Only broadcast if at least one mode was successfully applied
    if (mode_applied && !applied_modes.empty())
    {
        std::string mode_prefix = adding_mode ? "+" : "-";
        std::string mode_msg = ":" + client->getname() + " MODE " + channel_name + " " + mode_prefix + applied_modes + applied_params;
        channel->broadcast(mode_msg);
    }
}

void handle_cap(Server *server, Client *client, std::vector<std::string> &args)
{
	if (args.empty())
		return;
	
	std::string subcmd = args[0];
	
	if (subcmd == "LS")
	{
		client->send_reply(":" + server->getServerName() + " CAP * LS :");
	}
	else if (subcmd == "END")
	{
		return;
	}
	else if (subcmd == "REQ")
	{
		if (args.size() > 1)
			client->send_reply(":" + server->getServerName() + " CAP * NAK :" + args[1]);
	}
}

void handle_part(Server *server, Client *client, std::vector<std::string> &args)
{
	if (!client->isRegistered())
	{
		client->send_reply(":" + server->getServerName() + " 451 " + client->getname() + " :You have not registered");
		return;
	}

	if (args.empty())
	{
		client->send_reply(":" + server->getServerName() + " 461 " + client->getname() + " PART :Not enough parameters");
		return;
	}

	std::string channel_list = args[0];
	std::string reason = (args.size() > 1) ? args[1] : client->getname();
	std::stringstream ss(channel_list);
	std::string channel_name;

	while (std::getline(ss, channel_name, ','))
	{
		Channel* channel = server->getChannel(channel_name);
		if (!channel)
		{
			client->send_reply(":" + server->getServerName() + " 403 " + client->getname() + " " + channel_name + " :No such channel");
			continue;
		}

		if (!channel->isClientInChannel(client))
		{
			client->send_reply(":" + server->getServerName() + " 442 " + client->getname() + " " + channel_name + " :You're not on that channel");
			continue;
		}

		std::string part_msg = ":" + client->getname() + "!" + client->getUsername() + "@" + client->getip() + " PART " + channel_name + " :" + reason;
		channel->broadcast(part_msg);

		channel->removeClient(client);
	}
}

void handle_ping(Server *server, Client *client, std::vector<std::string> &args)
{
	if (args.empty())
	{
		client->send_reply(":" + server->getServerName() + " PONG " + server->getServerName());
	}
	else
	{
		client->send_reply(":" + server->getServerName() + " PONG " + server->getServerName() + " :" + args[0]);
	}
}

void handle_whois(Server *server, Client *client, std::vector<std::string> &args)
{
	if (!client->isRegistered())
	{
		client->send_reply(":" + server->getServerName() + " 451 " + client->getname() + " :You have not registered");
		return;
	}

	if (args.empty())
	{
		client->send_reply(":" + server->getServerName() + " 431 " + client->getname() + " :No nickname given");
		return;
	}

	std::string target_nick = args[0];
	Client* target = server->getClientByNick(target_nick);

	if (!target)
	{
		client->send_reply(":" + server->getServerName() + " 401 " + client->getname() + " " + target_nick + " :No such nick/channel");
		client->send_reply(":" + server->getServerName() + " 318 " + client->getname() + " " + target_nick + " :End of WHOIS list");
		return;
	}

	client->send_reply(":" + server->getServerName() + " 311 " + client->getname() + " " + target->getname() + " " + target->getUsername() + " " + target->getip() + " * :" + target->getUsername());

	client->send_reply(":" + server->getServerName() + " 312 " + client->getname() + " " + target->getname() + " " + server->getServerName() + " :IRC Server");

	client->send_reply(":" + server->getServerName() + " 318 " + client->getname() + " " + target->getname() + " :End of WHOIS list");
}

void handle_quit(Server *server, Client *client, std::vector<std::string> &args)
{
	std::string quit_msg = (args.empty()) ? "Client quit" : args[0];
	
	std::string quit_notification = ":" + client->getname() + "!" + client->getUsername() + "@" + client->getip() + " QUIT :" + quit_msg;
	
	const std::map<std::string, Channel*>& channels = server->getChannels();
	for (std::map<std::string, Channel*>::const_iterator it = channels.begin(); it != channels.end(); ++it)
	{
		Channel* channel = it->second;
		if (channel->isClientInChannel(client))
		{
			channel->broadcast(quit_notification, client);
			channel->removeClient(client);
		}
	}
	
	client->send_reply("ERROR :Closing connection: " + quit_msg);
}

void handle_who(Server *server, Client *client, std::vector<std::string> &args)
{
	if (!client->isRegistered())
	{
		client->send_reply(":" + server->getServerName() + " 451 " + client->getname() + " :You have not registered");
		return;
	}

	if (args.empty())
	{
		client->send_reply(":" + server->getServerName() + " 461 " + client->getname() + " WHO :Not enough parameters");
		return;
	}

	std::string target = args[0];

	// WHO query for a channel
	if (target[0] == '#')
	{
		Channel* channel = server->getChannel(target);
		if (!channel)
		{
			// RPL_ENDOFWHO (315) - send even if channel doesn't exist
			client->send_reply(":" + server->getServerName() + " 315 " + client->getname() + " " + target + " :End of WHO list");
			return;
		}

		const std::vector<Client*>& clients = channel->getClients();
		for (size_t i = 0; i < clients.size(); ++i)
		{
			Client* member = clients[i];
			std::string prefix = channel->isOperator(member) ? "@" : "";
			
			// RPL_WHOREPLY (352): <channel> <user> <host> <server> <nick> <H|G>[*][@|+] :<hopcount> <real name>
			// H = here, G = gone (away), * = IRC operator, @ = channel operator, + = voiced
			client->send_reply(":" + server->getServerName() + " 352 " + client->getname() + " " + 
			                  target + " " + member->getUsername() + " " + member->getip() + " " + 
			                  server->getServerName() + " " + member->getname() + " H" + prefix + " :0 " + member->getUsername());
		}
		
		// RPL_ENDOFWHO (315)
		client->send_reply(":" + server->getServerName() + " 315 " + client->getname() + " " + target + " :End of WHO list");
	}
	else
	{
		// WHO query for a user
		Client* target_client = server->getClientByNick(target);
		if (target_client)
		{
			// RPL_WHOREPLY (352)
			client->send_reply(":" + server->getServerName() + " 352 " + client->getname() + " * " + 
			                  target_client->getUsername() + " " + target_client->getip() + " " + 
			                  server->getServerName() + " " + target_client->getname() + " H :0 " + target_client->getUsername());
		}
		
		// RPL_ENDOFWHO (315)
		client->send_reply(":" + server->getServerName() + " 315 " + client->getname() + " " + target + " :End of WHO list");
	}
}
