/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hamza <hamza@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/17 01:37:33 by hamza             #+#    #+#             */
/*   Updated: 2021/10/17 01:58:35 by hamza            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "macros.hpp"

class CGI
{
public:
   static std::string     exec_file(char* path);
};