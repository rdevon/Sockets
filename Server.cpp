//
//  main.cpp
//  Sockets
//
//  Created by Devon Hjelm on 10/18/12.
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

void error(const char *msg)
{
   perror(msg);
   exit(1);
}

void say_hello(char *buffer[]) {}

int main(int argc, const char * argv[])
{
   std::string IP;
   std::string my_IP = "10.17.3.1";
   
   pcrecpp::RE hello("HELLO I'M (.+)\n");
   pcrecpp::RE goodbye("GOODBYE (.+)\n");
   pcrecpp::RE generate("GENERATE (\\d+) BYTES CALLED (\\w+)\n");
   pcrecpp::RE get_from("GET (\\w+) FROM (.+)\n");
   pcrecpp::RE give_me("GIVE ME (\\w+)\n");
   //--------
   
   int pid;
   int socket_fd, new_socket_fd;
   int port_number = 50632;
   socklen_t client_length;
   char buffer[256];
   
   struct sockaddr_in server_address, client_address;
   
   socket_fd = socket(AF_INET, SOCK_STREAM, 0);
   if (socket_fd < 0) error("ERROR opening socket");
   
   bzero((char *) &server_address, sizeof(server_address));
   server_address.sin_family = AF_INET;
   server_address.sin_addr.s_addr = inet_addr(my_IP.c_str());
   server_address.sin_port = htons(port_number);
   
   if (bind(socket_fd, (struct sockaddr *) &server_address, sizeof(server_address)) < 0) error("ERROR on binding");
   
   listen(socket_fd, 5);
   
   client_length = sizeof(client_address);
   while (1)
   {
      new_socket_fd = accept(socket_fd, (struct sockaddr *) &client_address, &client_length);
      pid = fork();
      
      if (pid == 0) {
         bzero(buffer,256);
         read(new_socket_fd,buffer,255);
         std::cout << buffer << std::endl;
         hello.FullMatch((std::string)buffer, &IP);
         if (IP.size() == 0) error("no fucking ip");
         std::string hello_message = "HELLO " + IP + ", I'M " + my_IP + "\n";
         write(new_socket_fd,hello_message.c_str(),hello_message.length());
         exit(0);
      }
      else close(new_socket_fd);
   }
   return 0;
}

