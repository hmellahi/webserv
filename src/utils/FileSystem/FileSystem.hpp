/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileSystem.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hamza <hamza@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/16 20:11:46 by hmellahi          #+#    #+#             */
/*   Updated: 2021/10/19 00:11:20 by hamza            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "macros.hpp"
#include <vector>

class FileSystem
{
public:
    static std::string  readFile(std::string filename, int &status);
    static std::string  openFile(std::string filename, int &status);
    static std::string  getIndexFileContent(std::string directory, std::string & filename);
};