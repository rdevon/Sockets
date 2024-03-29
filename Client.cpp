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
#include <sstream>

int port_number = 8080;
std::string my_IP;
pcrecpp::RE punk("(?i)ARE YOU FEELING LUCKY, PUNK\?");
pcrecpp::RE hello("(?i)HELLO\\s+I\'M\\s+(.+)\n?");
pcrecpp::RE hello_back("(?i)HELLO\\s+(.+)\\s+I\'M\\s+(.+)\n?");
pcrecpp::RE goodbye("(?i)GOODBYE\\s+(.+)\n?");
pcrecpp::RE generate_XYZ("(?i)GENERATE\\s+(\\d.+)\\s+BYTES\\s+CALLED\\s+(\\d+)\n?");
pcrecpp::RE get_XYZ_from("(?i)GET\\s+(\\d+)\\s+FROM\\s+(.+)\n?");
pcrecpp::RE give_me_XYZ("(?i)GIVE\\s+ME\\s+(\\d+)\n?");
pcrecpp::RE XYZ_is("(?i)(\\d+)\\s+IS\\s+(.+)\n?");


void error(const char *msg)
{
   perror(msg);
   exit(1);
}

void print_instructs() {
   std::cout << "Enter: " << std::endl;
   std::cout << "      1) HELLO" << std::endl;
   std::cout << "      2) GOODBYE" << std::endl;
   std::cout << "      3) MESSAGE" << std::endl;
}

void enter_message(int socket_fd) {
   char buffer[256];
   bzero(buffer, strlen(buffer));
   std::cout << "Enter message: " << std::endl;
   std::cin.getline(buffer, 256);
   std::stringstream out;
   out << buffer;
   std::string message = out.str() + "\n";
   write(socket_fd, message.c_str(),message.length());
   bzero(buffer, strlen(buffer));
   read(socket_fd, buffer, 255);
   std::cout << buffer << std::endl;
}

void say_hello(int socket_fd, std::string to_IP) {
   char buffer[256];
   std::string hello_message = "HELLO I'M " + my_IP + "\n";
   write(socket_fd,hello_message.c_str(),hello_message.length());
   bzero(buffer,256);
   read(socket_fd,buffer,255);
   std::cout << buffer << std::endl;
   if (!hello_back.FullMatch(buffer)) error("NO HELLO BACK");
}

void say_goodbye(int socket_fd, std::string to_IP) {
   char buffer[256];
   std::string message = "GOODBYE " + to_IP + "\n";
   write(socket_fd, message.c_str(), message.length());
   bzero(buffer,256);
   read(socket_fd, buffer, 255);
   std::cout << buffer << std::endl;
   if (!goodbye.FullMatch(buffer)) error("NO GOODBYE BACK");
   close(socket_fd);
   exit(0);
}

void infinite_hello(int socket_fd, std::string to_IP) {
   while (1) {
      char buffer[256];
      std::string hello_message = "HELLO I'M " + my_IP + "\n";
      write(socket_fd,hello_message.c_str(),hello_message.length());
      bzero(buffer,256);
      read(socket_fd,buffer,255);
   }
}

int main(int argc, const char * argv[]) {
   
   std::string host_IP;
   
   int socket_fd;
   struct sockaddr_in server_address, my_address;
   
   char buffer[256];
   
   if (argc < 2) {
      fprintf(stderr,"usage %s hostname\n", argv[0]);
      exit(0);
   }
   
   socket_fd = socket(AF_INET, SOCK_STREAM, 0);
   
   std::stringstream out;   
   
   memset(&server_address, 0, sizeof(server_address));
   server_address.sin_family = AF_INET;
   server_address.sin_port = htons(port_number);
   server_address.sin_addr.s_addr = inet_addr(argv[1]);
   
   if (connect(socket_fd, (struct sockaddr *) &server_address, sizeof(server_address)) < 0) error("ERROR CONNECTING");
   memset(&my_address, 0, sizeof(my_address));
   socklen_t my_length = sizeof(my_address);
   if (getsockname(socket_fd, (struct sockaddr *) &my_address, &my_length) <0) error("CANT GET NAME");
   
   std::cout << "Connecting on port " << ntohs(my_address.sin_port) << std::endl;
   
   int input;
   
   out << inet_ntoa(my_address.sin_addr);
   my_IP = out.str();
   out.str("");
   out << inet_ntoa(server_address.sin_addr);
   host_IP = out.str();
   
   std::cout << "Client at IP " << my_IP << " connecting to " << host_IP << std::endl;
   
   while (1) {
      memset(&buffer, 0, sizeof(buffer));
      print_instructs();
      std::cin >> input;
      switch (input) {
         case 0: infinite_hello(socket_fd, host_IP); break;
         case 1: say_hello(socket_fd, host_IP); break;
         case 2: say_goodbye(socket_fd, host_IP); break;
         case 3: enter_message(socket_fd); break;
         default: {
            close(socket_fd);
            exit(0);}
      }
      std::cout << buffer << std::endl;
   
   }
   return 0;
   
}