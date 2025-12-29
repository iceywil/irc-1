/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wscherre <wscherre@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/09 16:57:08 by mmedjahe          #+#    #+#             */
/*   Updated: 2025/12/28 17:54:32 by wscherre         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <algorithm>
#include <cctype>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <map>
#include <netinet/in.h>
#include <set>
#include <stdexcept>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

#include <arpa/inet.h>
 #include <poll.h>
#include <map>
 
class Client;
class Channel;

class Server
{
  public:
	Server(int port, const std::string &password);
	~Server();
	void start();
    void process_command(Client *client, const std::string &message);
    const std::string& getPassword() const;
    void disconnectClient(int client_fd);
    Client* getClientByFd(int fd);
    Client* getClientByNick(const std::string& nick);
    Channel* getChannel(const std::string& name);
    const std::map<std::string, Channel*>& getChannels() const;
    void createChannel(const std::string& name, Client* op);
    const std::string& getServerName() const;
	
  private:
	int _port;
	std::string _password;
    std::string _serverName;
	int _socketfd;
	std::vector<Client*> listClients;
    std::map<std::string, Channel*> listChannels;
	std::vector<struct pollfd> pollfds;
	void acceptclients();
	void loop();
    void handle_client_data(int client_fd);
};