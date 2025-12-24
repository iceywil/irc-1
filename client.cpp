/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmedjahe <mmedjahe@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/16 18:36:08 by mmedjahe          #+#    #+#             */
/*   Updated: 2025/12/23 12:00:00 by you             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "client.hpp"

Client::Client(int cfd, int cport, const std::string& cip) : fd(cfd), port(cport), ip(cip), is_registered(false), has_valid_password(false)
{
}

Client::~Client(){}

int Client::getfd() const{
    return fd;
}
int Client::getport() const{
    return port;
}
std::string Client::getname() const{
    return nickname;
}

std::string Client::getUsername() const{
    return username;
}

std::string Client::getip() const{
    return ip;
}

std::string &Client::getBuffer()
{
    return buffer;
}

bool Client::isRegistered() const
{
    return is_registered;
}

bool Client::hasValidPassword() const
{
    return has_valid_password;
}

void Client::setHasValidPassword(bool value)
{
    has_valid_password = value;
}

void Client::setNickname(const std::string &nick)
{
    nickname = nick;
}

void Client::setUsername(const std::string &user)
{
    username = user;
}

void Client::setRegistered(bool value)
{
    is_registered = value;
}

void Client::send_reply(const std::string &message)
{
    std::string full_message = message + "\r\n";
    send(fd, full_message.c_str(), full_message.length(), 0);
}

void Client::setFd(int new_fd)
{
    fd = new_fd;
}
