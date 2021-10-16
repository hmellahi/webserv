/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   str_utils.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hamza <hamza@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/16 04:02:41 by hamza             #+#    #+#             */
/*   Updated: 2021/10/16 04:07:23 by hamza            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "str_utils.hpp"

std::string GetFileExtension(const std::string& fileName)
{
    if(fileName.find_last_of(".") != std::string::npos)
        return fileName.substr(fileName.find_last_of(".") + 1);
    return "";
}