/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmedjahe <mmedjahe@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/09 16:15:02 by mmedjahe          #+#    #+#             */
/*   Updated: 2025/12/15 21:18:09 by mmedjahe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"

void	parsing(char *v1, char *v2)
{

	std::string p = v1;
	for (int i = 0; p[i]; ++i)
	{
		if (!isdigit(p[i]))
			throw std::runtime_error("Port should be between 1 and 65535");
	}
	int port = atoi(v1);
	if (port < 1 || port > 65535)
		throw std::runtime_error("Port should be between 1 and 65535");
	std::string pass = v2;
	if (pass.empty() || std::find_if(pass.begin(), pass.end(), ::isspace) != pass.end())
		throw std::runtime_error("Password error");
}

int	main(int c, char **v)
{
	if (c != 3)
	{
		std::cerr << "ERROR, program only works with 3 args : ./ircserv <port> <password> " << std::endl;
		return (1);
	}
	try
	{
		parsing(v[1], v[2]);
		Server s(atoi(v[1]), v[2]);
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		return (1);
	}
}
