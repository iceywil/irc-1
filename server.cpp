/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmedjahe <mmedjahe@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/09 17:01:36 by mmedjahe          #+#    #+#             */
/*   Updated: 2025/12/23 15:45:00 by you              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"
#include "client.hpp"
#include "commands.hpp"
#include "channel.hpp"
#include "colors.hpp"
#include <sstream>
#include <vector>
#include <algorithm>

Server::Server(int port, const std::string& password) : _port(port), _password(password), _serverName("ircserv")
{}

Server::~Server(){}

void Server::start(){
    _socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if(_socketfd < 0)
        throw std::runtime_error("socket error");
    
    int opt = 1;
    if (setsockopt(_socketfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
        throw std::runtime_error("setsockopt error");

    if (fcntl(_socketfd, F_SETFL, O_NONBLOCK) < 0)
        throw std::runtime_error("fcntl error");
    sockaddr_in sockad;
    memset(&sockad, 0, sizeof(sockad));
    sockad.sin_family = AF_INET;
    sockad.sin_port = htons(_port);
    sockad.sin_addr.s_addr = INADDR_ANY;
    if(bind(_socketfd, (const sockaddr*)&sockad, sizeof(sockad)) < 0)
        throw std::runtime_error("bind error");
    if(listen(_socketfd, 5) < 0)
        throw std::runtime_error("listen error");
    struct pollfd pfd;
    pfd.fd = _socketfd;
    pfd.events = POLLIN;
    pfd.revents = 0;
    pollfds.push_back(pfd);
    loop();
}

void Server::acceptclients(){
    sockaddr_in clientaddr;
    socklen_t lenclient = sizeof(clientaddr);

    int clientfd = accept(_socketfd, (sockaddr*)&clientaddr, &lenclient);
    if(clientfd < 0)
        throw std::runtime_error("accept error");
    if (fcntl(clientfd, F_SETFL, O_NONBLOCK) < 0)
        throw std::runtime_error("fcntl error");
    
    Client *C = new Client (clientfd, ntohs(clientaddr.sin_port), inet_ntoa(clientaddr.sin_addr));
    pollfd c;
    c.fd = C->getfd();
    c.events = POLLIN;
    c.revents = 0;
    pollfds.push_back(c);
    listClients.push_back(C);
    std::cout << GREEN << "New client connected: " << C->getip() << ":" << C->getport() << RESET << std::endl << std::endl;
}

void Server::loop(){
    while (true){
        if(poll(pollfds.data(), pollfds.size(), -1) == - 1)
            throw std::runtime_error("poll error");
        
        if(pollfds[0].revents & POLLIN)
            acceptclients();
        
        for (size_t i = 1; i < pollfds.size(); ++i)
        {
            if(pollfds[i].revents & POLLIN)
            {
                handle_client_data(pollfds[i].fd);
            }
        }
    }
}

void Server::disconnectClient(int client_fd)
{
    std::cout << RED << "Client " << client_fd << " has disconnected." << RESET << std::endl << std::endl;

    // Find and remove the pollfd struct
    for (size_t i = 1; i < pollfds.size(); ++i)
    {
        if (pollfds[i].fd == client_fd)
        {
            pollfds.erase(pollfds.begin() + i);
            break;
        }
    }

    // Find and remove the Client object
    for (size_t i = 0; i < listClients.size(); ++i)
    {
        if (listClients[i]->getfd() == client_fd)
        {
            // Remove the client from all channels
            for (std::map<std::string, Channel*>::iterator it = listChannels.begin(); it != listChannels.end(); ++it)
            {
                it->second->removeClient(listClients[i]);
            }

            delete listClients[i];
            listClients.erase(listClients.begin() + i);
            break;
        }
    }
    
    close(client_fd);
}

void Server::handle_client_data(int client_fd)
{
    char buf[512];
    memset(buf, 0, 512);
    
    Client *client = getClientByFd(client_fd);
    if (!client) return;

    int n = recv(client_fd, buf, 512, 0);
    if (n <= 0)
    {
        disconnectClient(client_fd);
        return;
    }

    client->getBuffer().append(buf, n);
    
    size_t pos;
    while ((pos = client->getBuffer().find("\r\n")) != std::string::npos)
    {
        std::string msg = client->getBuffer().substr(0, pos);
        client->getBuffer().erase(0, pos + 2);
        if (!msg.empty())
            process_command(client, msg);
    }
}

void Server::process_command(Client *client, const std::string &message)
{
    std::cout << YELLOW << "Processing command from client " << client->getfd() << ": " << message << RESET << std::endl << std::endl;
    std::stringstream ss(message);
    std::string command;
    ss >> command;

    std::vector<std::string> args;
    std::string arg;
    
    std::string rest;
    std::getline(ss, rest);
    if (!rest.empty() && rest[0] == ' ')
        rest = rest.substr(1);
        
    size_t colon_pos = rest.find(':');
    if (colon_pos != std::string::npos)
    {
        std::string before_colon = rest.substr(0, colon_pos);
        std::string after_colon = rest.substr(colon_pos + 1);
        
        std::stringstream before_ss(before_colon);
        while(before_ss >> arg)
        {
            args.push_back(arg);
        }
        args.push_back(after_colon);
    }
    else
    {
        std::stringstream rest_ss(rest);
        while(rest_ss >> arg)
        {
            args.push_back(arg);
        }
    }

    if (command == "PASS")
        handle_pass(this, client, args);
    else if (command == "NICK")
        handle_nick(this, client, args);
    else if (command == "USER")
        handle_user(this, client, args);
    else if (command == "JOIN")
        handle_join(this, client, args);
    else if (command == "PRIVMSG")
        handle_privmsg(this, client, args);
    else if (command == "KICK")
        handle_kick(this, client, args);
    else if (command == "INVITE")
        handle_invite(this, client, args);
    else if (command == "TOPIC")
        handle_topic(this, client, args);
    else if (command == "MODE")
        handle_mode(this, client, args);
    else
    {
        std::cout << RED << "Unknown command from client " << client->getfd() << ": " << command << RESET << std::endl << std::endl;
    }
}

const std::string& Server::getPassword() const
{
    return _password;
}

Client* Server::getClientByFd(int fd)
{
    for (size_t i = 0; i < listClients.size(); ++i)
    {
        if (listClients[i]->getfd() == fd)
            return listClients[i];
    }
    return NULL;
}

Client* Server::getClientByNick(const std::string& nick)
{
    for (size_t i = 0; i < listClients.size(); ++i)
    {
        if (listClients[i]->getname() == nick)
            return listClients[i];
    }
    return NULL;
}

Channel* Server::getChannel(const std::string& name)
{
    std::map<std::string, Channel*>::iterator it = listChannels.find(name);
    if (it != listChannels.end())
        return it->second;
    return NULL;
}

void Server::createChannel(const std::string& name, Client* op)
{
    Channel *newChannel = new Channel(name, op);
    listChannels.insert(std::make_pair(name, newChannel));
}

const std::string& Server::getServerName() const
{
    return _serverName;
}
