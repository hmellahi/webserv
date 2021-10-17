/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hamza <hamza@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/15 17:43:29 by hamza             #+#    #+#             */
/*   Updated: 2021/10/17 05:16:42 by hamza            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Server.hpp"
#include "str_utils.hpp"

class Request
{
private:
    std::map<std::string, std::string> _headers;

public:
    Request(std::string buffer)
    {
        parse(buffer);
    }

    void parse(std::string &buffer)
    {
        std::vector<std::string> tokens;
        split(buffer, ' ', tokens);
        if (tokens[1].size() > 2)
            tokens[1].erase(0, 1);
        else
            tokens[1] = "."; // temp (brikol) will be removed
        _headers["url"] = tokens[1];
        _headers["http-version"] = tokens[2];
        // _headers["method"] = getMethodIndex(tokens[0]);
        _headers["method"] = tokens[0];
    }

    std::string getHeader(std::string header_name)
    {
        return _headers[header_name];
    }

    void setHeader(std::string header_name, std::string value)
    {
        _headers[header_name] = value;
    }
};

