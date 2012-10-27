//
//  Client.cpp
//  Sockets
//
//  Created by Devon Hjelm on 10/19/12.
//  Copyright (c) 2012 Devon Hjelm. All rights reserved.
//

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pcrecpp.h>


int client_main(int argc, char *msg) {
   int socket_fd;
   int port_number = 1000;
   struct sockaddr_in server_address;
   
   char buffer[256];
   
   socket_fd = socket(AF_INET, SOCK_STREAM, 0);
   
   memset(&server_address, 0, sizeof(server_address));
   server_address.sin_family = AF_INET;
   server_address.sin_port = htons(port_number);
   server_address.sin_addr.s_addr = inet_addr("10.28.3.1");
   
   connect(socket_fd, (struct sockaddr *) &server_address, sizeof(server_address));
   
   memset(&buffer, 0, sizeof(buffer));
   std::string hello_message = "HELLO I'M, 10.28.3.2\n";
   write(socket_fd, &hello_message, sizeof(hello_message));
   read(socket_fd, &buffer, 255);
   
   std::cout << buffer << std::endl;
   
   return 0;
   
}