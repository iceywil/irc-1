/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmedjahe <mmedjahe@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/16 18:33:30 by mmedjahe          #+#    #+#             */
/*   Updated: 2025/12/22 19:14:38 by mmedjahe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "server.hpp"

class Client{
   public :
        Client(){};
        Client(int cfd, int cport, const std::string& cip){};
        ~Client(){};
        int getfd();
        int getport();
        std::string getname();
        std::string getip();
    private :
        int fd;
        int port;
        std::string ip;
        std::string nickname;
};

