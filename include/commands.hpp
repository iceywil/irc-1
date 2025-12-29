/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commands.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmedjahe <mmedjahe@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/23 15:00:00 by you             #+#    #+#             */
/*   Updated: 2025/12/23 15:00:00 by you             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <string>
#include <vector>

class		Server;
class		Client;

void		handle_pass(Server *server, Client *client,
				std::vector<std::string> &args);
void		handle_nick(Server *server, Client *client,
				std::vector<std::string> &args);
void		handle_user(Server *server, Client *client,
				std::vector<std::string> &args);

void		handle_join(Server *server, Client *client,
				std::vector<std::string> &args);
void		handle_part(Server *server, Client *client,
				std::vector<std::string> &args);
void		handle_privmsg(Server *server, Client *client,
				std::vector<std::string> &args);
void		handle_kick(Server *server, Client *client,
				std::vector<std::string> &args);
void		handle_invite(Server *server, Client *client,
				std::vector<std::string> &args);
void		handle_topic(Server *server, Client *client,
				std::vector<std::string> &args);
void		handle_mode(Server *server, Client *client,
				std::vector<std::string> &args);
void		handle_cap(Server *server, Client *client,
				std::vector<std::string> &args);
void		handle_ping(Server *server, Client *client,
				std::vector<std::string> &args);
void		handle_whois(Server *server, Client *client,
				std::vector<std::string> &args);
void		handle_quit(Server *server, Client *client,
				std::vector<std::string> &args);
void		handle_who(Server *server, Client *client,
				std::vector<std::string> &args);
