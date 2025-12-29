/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmedjahe <mmedjahe@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/23 10:00:00 by you             #+#    #+#             */
/*   Updated: 2025/12/23 14:05:00 by you             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/channel.hpp"
#include "../include/client.hpp"
#include <algorithm>
#include <sys/socket.h>

Channel::Channel(std::string const & name, Client *op) : _name(name), 
    _invite_only(false), _topic_restricted(true), _user_limit(0)
{
    addClient(op);
    addOperator(op);
}

Channel::~Channel() {}

void Channel::addClient(Client *client)
{
    _clients.push_back(client);
}

void Channel::removeClient(Client *client)
{
    std::vector<Client*>::iterator it = std::find(_clients.begin(), _clients.end(), client);
    if (it != _clients.end())
        _clients.erase(it);
    
    std::vector<Client*>::iterator op_it = std::find(_operators.begin(), _operators.end(), client);
    if (op_it != _operators.end())
        _operators.erase(op_it);
}

void Channel::broadcast(std::string const & message)
{
    for (size_t i = 0; i < _clients.size(); i++)
    {
        _clients[i]->send_reply(message);
    }
}

void Channel::broadcast(std::string const & message, Client *exclude)
{
    for (size_t i = 0; i < _clients.size(); i++)
    {
        if (_clients[i] != exclude)
        {
            _clients[i]->send_reply(message);
        }
    }
}

std::string const & Channel::getName() const
{
    return _name;
}

std::string const & Channel::getTopic() const
{
    return _topic;
}

const std::vector<Client*>& Channel::getClients() const
{
    return _clients;
}

void Channel::setTopic(std::string const & topic)
{
    _topic = topic;
}

bool Channel::isOperator(Client *client)
{
    return std::find(_operators.begin(), _operators.end(), client) != _operators.end();
}

void Channel::addOperator(Client *client)
{
    if (!isOperator(client))
        _operators.push_back(client);
}

void Channel::removeOperator(Client *client)
{
    std::vector<Client*>::iterator it = std::find(_operators.begin(), _operators.end(), client);
    if (it != _operators.end())
        _operators.erase(it);
}

bool Channel::isClientInChannel(Client *client) const
{
    for (size_t i = 0; i < _clients.size(); ++i)
    {
        if (_clients[i] == client)
            return true;
    }
    return false;
}

void Channel::addInvited(Client* client)
{
    if (!isInvited(client))
        _invited.push_back(client);
}

bool Channel::isInvited(Client* client)
{
    return std::find(_invited.begin(), _invited.end(), client) != _invited.end();
}

void Channel::removeInvited(Client* client)
{
    std::vector<Client*>::iterator it = std::find(_invited.begin(), _invited.end(), client);
    if (it != _invited.end())
        _invited.erase(it);
}

bool Channel::isInviteOnly() const { return _invite_only; }
void Channel::setInviteOnly(bool value) { _invite_only = value; }
bool Channel::isTopicRestricted() const { return _topic_restricted; }
void Channel::setTopicRestricted(bool value) { _topic_restricted = value; }
std::string Channel::getPassword() const { return _password; }
void Channel::setPassword(const std::string& pass) { _password = pass; }
int Channel::getUserLimit() const { return _user_limit; }
void Channel::setUserLimit(int limit) { _user_limit = limit; }
