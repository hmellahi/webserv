/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   macros.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hamza <hamza@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/16 00:15:27 by hamza             #+#    #+#             */
/*   Updated: 2021/10/17 01:50:01 by hamza            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <map>
#include <string.h>
#include <stdio.h>
#include <cstring>
#include <iostream>
#include <sys/stat.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fstream>
#include <fcntl.h>
#include <string>

enum methods
{
    GET,
    POST,
    DELETE    
};

enum file_status
{
    IS_DIRECTORY
};
// {
//     SUCCESS,
//     CANT_OPEN_FILE,
//     FILE_NOT_FOUND,
// };

enum status_codes
{
    OK = 200,

    PERMISSION_DENIED = 403,
    NOT_FOUND = 404,
    METHOD_NOT_ALLOWED = 405,
    
    BAD_GATEWAY = 502
};