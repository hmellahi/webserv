/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   str_utils.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hamza <hamza@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/16 04:07:16 by hamza             #+#    #+#             */
/*   Updated: 2021/10/18 00:28:39 by hamza            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "macros.hpp"

std::string     GetFileExtension(const std::string& fileName);
void            split (std::string str, char seperator, std::vector<std::string> &strings);
std::string     getCurrentDate();