/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hamza <hamza@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/15 17:43:29 by hamza             #+#    #+#             */
/*   Updated: 2021/10/15 22:46:22 by hamza            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Server.hpp"
class Request
{

private:
    int         _method;
    std::string _path;
    std::string _host;

public:
    Request(std::string buffer)
    {
        // parse(buffer);
        _path = "tests/s_web/index.html";
    }

    std::string getPath() const
    {
        return _path;   
    }

    int getMethod() const
    {
        return _method;   
    }
    
    void parse(std::string &buffer)
    {
        // char **str = 
        // int i;
        // while (1)
        // {
        //     getline(std::cin, buffer);
        //     if (buffer.empty())
        //         break;
        //     if (!i)
        //     {
        //         // split(buffer, " ");
        //         // _host = ;
        //         // while (getline(std::cin, buffer))
        //     }
        // }
    }
};