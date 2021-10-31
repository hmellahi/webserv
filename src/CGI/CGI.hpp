/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malaoui <malaoui@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/17 01:37:33 by malaoui           #+#    #+#             */
/*   Updated: 2021/10/17 04:33:13 by malaoui          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include "macros.hpp"
# include "Request.hpp"

class CGI
{
public:
   static std::string     exec_file(std::string path);
   static std::string     exec_file(std::string path, Request req);
};
