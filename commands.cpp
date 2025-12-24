/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commands.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmedjahe <mmedjahe@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/09 16:15:02 by mmedjahe          #+#    #+#             */
/*   Updated: 2025/12/15 21:18:09 by mmedjahe         ###   ########.fr       */
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

void handle_nick(Server *server, Client *client, std::vector<std::string> &args)
{
	if (!client->hasValidPassword())
	{
		client->send_reply(":" + server->getServerName() + " 464 " + (client->getname().empty() ? "*" : client->getname()) + " :Password incorrect");
		return;
	}
	if (args.empty() || args[0].empty())
	{
		client->send_reply(":" + server->getServerName() + " 431 " + (client->getname().empty() ? "*" : client->getname()) + " :No nickname given");
		return;
	}
	const std::string &nick = args[0];
	if (nick.length() > 9)
	{
		client->send_reply(":" + server->getServerName() + " 432 " + (client->getname().empty() ? "*" : client->getname()) + " " + nick + " :Erroneous nickname");
		return;
	}
	if (server->getClientByNick(nick))
	{
		client->send_reply(":" + server->getServerName() + " 433 " + (client->getname().empty() ? "*" : client->getname()) + " " + nick + " :Nickname is already in use");
		return;
	}
	client->setNickname(nick);
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

        std::string join_msg = ":" + client->getname() + " JOIN " + channel_name;
        channel->broadcast(join_msg);
        if (!channel->getTopic().empty())
            client->send_reply(":" + server->getServerName() + " 332 " + client->getname() + " " + channel_name + " :" + channel->getTopic());
        
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
	if (args.size() < 2)
    {
        client->send_reply(":" + server->getServerName() + " 461 " + client->getname() + " MODE :Not enough parameters");
        return;
    }

    const std::string& channel_name = args[0];
    const std::string& modestring = args[1];
    
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

    if (modestring.empty() || (modestring[0] != '+' && modestring[0] != '-'))
    {
        client->send_reply(":" + server->getServerName() + " 501 " + client->getname() + " :Unknown MODE flag");
        return;
    }

    bool adding_mode = (modestring[0] == '+');
    size_t arg_index = 2;

    for (size_t i = 1; i < modestring.length(); ++i)
    {
        char mode = modestring[i];
        switch (mode)
        {
            case 'i':
                channel->setInviteOnly(adding_mode);
                break;
            case 't':
                channel->setTopicRestricted(adding_mode);
                break;
            case 'k':
                if (adding_mode)
                {
                    if (arg_index < args.size())
                    {
                        channel->setPassword(args[arg_index]);
                        arg_index++;
                    }
                    else
                        client->send_reply(":" + server->getServerName() + " 461 " + client->getname() + " MODE +k :Not enough parameters");
                }
                else
                {
                    channel->setPassword("");
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
                        arg_index++;
                    }
                    else
                        client->send_reply(":" + server->getServerName() + " 461 " + client->getname() + " MODE +l :Not enough parameters");
                }
                else
                {
                    channel->setUserLimit(0);
                }
                break;
            default:
                client->send_reply(":" + server->getServerName() + " 472 " + client->getname() + " " + mode + " :is unknown mode char to me for " + channel_name);
                break;
        }
    }
    std::string mode_msg = ":" + client->getname() + " MODE " + channel_name + " " + modestring;
    for (size_t i = 2; i < arg_index; ++i)
        mode_msg += " " + args[i];
    channel->broadcast(mode_msg);
}
