/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   str_utils.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hamza <hamza@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/16 04:02:41 by hamza             #+#    #+#             */
/*   Updated: 2021/10/18 00:51:43 by hamza            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "str_utils.hpp"

std::string GetFileExtension(const std::string& fileName)
{
	if(fileName.find_last_of(".") != std::string::npos)
		return fileName.substr(fileName.find_last_of(".") + 1);
	return "";
}

int len(std::string str)  
{  
	int length = 0;  
	for (int i = 0; str[i] != '\0'; i++)  
	{  
		length++;  
		  
	}  
	return length;     
}  

// create custom split() function  
void split (std::string str, char seperator, std::vector<std::string> &strings)  
{  
	int currIndex = 0, i = 0;  
	int startIndex = 0, endIndex = 0;  
	while (i <= len(str))  
	{  
		if (str[i] == seperator || i == len(str))  
		{  
			endIndex = i;  
			std::string subStr = "";  
			subStr.append(str, startIndex, endIndex - startIndex);  
			strings.push_back(subStr);  
			currIndex += 1;  
			startIndex = endIndex + 1;  
		}  
		i++;  
	}     
}

// get current date and time
std::string getCurrentDate()
{
	// current date/time based on current system
	// std::ostringstream currentDate;
	// time_t now = time(0);

	// tm *ltm = localtime(&now);
	// currentDate << << ltm->tm_mday << " "  // day
	// currentDate << 1 + ltm->tm_mon << " " // month
	// currentDate << 1900 + ltm->tm_year << " " // year
	// currentDate << "Time: "<< 5+ltm->tm_hour << ":";
	// currentDate << 30+ltm->tm_min << ":";
	// currentDate << ltm->tm_sec << std::endl;

	char outstr[200] = {0};
	time_t t;
	struct tm *tmp;
	const char* fmt = "%a, %d %b %Y %T GMT"; // https://www.cplusplus.com/reference/ctime/strftime/

	t = time(NULL);
	tmp = gmtime(&t);
	if (tmp == NULL) {
		perror("gmtime error");
		exit(EXIT_FAILURE);
	}

	if (strftime(outstr, sizeof(outstr), fmt, tmp) == 0) { 
		fprintf(stderr, "strftime returned 0");
		exit(EXIT_FAILURE); 
	} 
   return (outstr);
}
//Sun, 17 Oct 2021 23:38:35 GMT