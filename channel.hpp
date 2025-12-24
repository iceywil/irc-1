/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmedjahe <mmedjahe@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/23 10:00:00 by you             #+#    #+#             */
/*   Updated: 2025/12/23 14:00:00 by you             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <vector>
#include <map>

class Client;

class Channel
{
public:
    Channel(std::string const & name, Client *op);
    ~Channel();

    void addClient(Client *client);
    void removeClient(Client *client);
    void broadcast(std::string const & message);
    void broadcast(std::string const & message, Client *exclude);

    std::string const &getName() const;
    std::string const &getTopic() const;
    const std::vector<Client*>& getClients() const;
    void setTopic(std::string const & topic);

    bool isOperator(Client *client);
    void addOperator(Client *client);
    void removeOperator(Client *client);

    bool isClientInChannel(Client *client) const;
    void addInvited(Client* client);
    bool isInvited(Client* client);
    void removeInvited(Client* client);

    // Modes
    bool isInviteOnly() const;
    void setInviteOnly(bool value);
    bool isTopicRestricted() const;
    void setTopicRestricted(bool value);
    std::string getPassword() const;
    void setPassword(const std::string& pass);
    int getUserLimit() const;
    void setUserLimit(int limit);


private:
    std::string _name;
    std::string _topic;
    std::vector<Client*> _clients;
    std::vector<Client*> _operators;
    std::vector<Client*> _invited;
    
    bool        _invite_only;
    bool        _topic_restricted;
    std::string _password;
    int         _user_limit;
};
