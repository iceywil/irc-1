/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmedjahe <mmedjahe@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/16 18:36:08 by mmedjahe          #+#    #+#             */
/*   Updated: 2025/12/22 19:14:32 by mmedjahe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "client.hpp"

Client::Client(){}

Client::Client(int cfd, int cport, const std::string& cip) : fd(cfd), port(cport), ip(cip)
{
}

Client::~Client(){}

int Client::getfd(){
    return fd;
}
int Client::getport(){
    return port;
}
std::string Client::getname(){
    return nickname;
}
std::string Client::getip(){
    return ip;
}