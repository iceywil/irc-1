/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmedjahe <mmedjahe@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/09 17:01:36 by mmedjahe          #+#    #+#             */
/*   Updated: 2025/12/22 19:35:24 by mmedjahe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"

Server::Server(int port, const std::string& password) : _port(port), _password(password)
{}

Server::~Server(){}

void Server::start(){
    _socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if(_socketfd < 0)
        throw std::runtime_error("socket error");
    sockaddr_in sockad;
    memset(&sockad, 0, sizeof(sockad));
    sockad.sin_family = AF_INET;
    sockad.sin_port = htons(_port);
    sockad.sin_addr.s_addr = INADDR_ANY;
    if(bind(_socketfd, (const sockaddr*)&sockad, sizeof(sockad)) < 0)
        throw std::runtime_error("bind error");
    if(listen(_socketfd, 5) < 0)
        throw std::runtime_error("listen error");
    pollfds.push_back({_socketfd, POLLIN, 0});
    loop();
}

void Server::acceptclients(){
    sockaddr_in clientaddr;
    socklen_t lenclient = sizeof(clientaddr);

    int clientfd = accept(_socketfd, (sockaddr*)&clientaddr, &lenclient);
    if(clientfd < 0)
        throw std::runtime_error("accept error");
    int fd = clientfd;
    int port = ntohs(clientaddr.sin_port);
    char buffer[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &clientaddr.sin_addr, buffer, sizeof(buffer));
    std::string ip = buffer;
    Client *C = new Client (fd, port, ip);
    pollfd c;
    c.fd = C->getfd();
    c.events = POLLIN;
    c.revents = 0;
    pollfds.push_back(c);
    listClients.push_back(C);
}

void Server::loop(){

    while (true){
        if(poll(pollfds.data(), pollfds.size(), 0) == - 1)
            throw std::runtime_error("poll error");
        if(pollfds[0].revents & POLLIN)
            acceptclients();
        std::vector<struct pollfd>::iterator  it;
        it = pollfds.begin();
        ++it;
        for (; it != pollfds.end(); ++it)
        {
            if(it->revents & POLLIN)
            {
                char buf[512];
                int n = recv(it->fd, buf, 512, 0);
                
            }
                
        }
               
    }  
}
