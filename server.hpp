/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmedjahe <mmedjahe@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/09 16:57:08 by mmedjahe          #+#    #+#             */
/*   Updated: 2025/12/17 16:54:57 by mmedjahe         ###   ########.fr       */
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
#include "client.hpp"
 #include <poll.h>
 
class Client;

class Server
{
  public:
	Server(int port, const std::string &password);
	~Server();
	void start();
	
  private:
	int _port;
	std::string _password;
	int _socketfd;
	std::vector<Client*> listClients;
	std::vector<struct pollfd> pollfds;
	void acceptclients();
	void loop();
};