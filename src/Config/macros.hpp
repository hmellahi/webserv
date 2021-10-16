/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   macros.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hamza <hamza@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/16 00:15:27 by hamza             #+#    #+#             */
/*   Updated: 2021/10/16 19:20:59 by hamza            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
#include <unistd.h>
#include <fstream>
#include <string>

enum methods
{
    GET,
    POST,
    DELETE    
};

// enum file_status
// {
//     SUCCESS,
//     CANT_OPEN_FILE,
//     FILE_NOT_FOUND,
// };

enum status_codes
{
    OK = 200,
    PERMISSION_DENIED = 402,
    NOT_FOUND = 404,
    BAD_GATEWAY = 502
};