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
#include <sstream>
#include <time.h>
#include <math.h>

static const uint32_t kCrc32Table[256] = {
   0x00000000, 0x77073096, 0xee0e612c, 0x990951ba,
   0x076dc419, 0x706af48f, 0xe963a535, 0x9e6495a3,
   0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
   0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91,
   0x1db71064, 0x6ab020f2, 0xf3b97148, 0x84be41de,
   0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
   0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec,
   0x14015c4f, 0x63066cd9, 0xfa0f3d63, 0x8d080df5,
   0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
   0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b,
   0x35b5a8fa, 0x42b2986c, 0xdbbbc9d6, 0xacbcf940,
   0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
   0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116,
   0x21b4f4b5, 0x56b3c423, 0xcfba9599, 0xb8bda50f,
   0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
   0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d,
   0x76dc4190, 0x01db7106, 0x98d220bc, 0xefd5102a,
   0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
   0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818,
   0x7f6a0dbb, 0x086d3d2d, 0x91646c97, 0xe6635c01,
   0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
   0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457,
   0x65b0d9c6, 0x12b7e950, 0x8bbeb8ea, 0xfcb9887c,
   0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
   0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2,
   0x4adfa541, 0x3dd895d7, 0xa4d1c46d, 0xd3d6f4fb,
   0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
   0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9,
   0x5005713c, 0x270241aa, 0xbe0b1010, 0xc90c2086,
   0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
   0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4,
   0x59b33d17, 0x2eb40d81, 0xb7bd5c3b, 0xc0ba6cad,
   0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
   0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683,
   0xe3630b12, 0x94643b84, 0x0d6d6a3e, 0x7a6a5aa8,
   0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
   0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe,
   0xf762575d, 0x806567cb, 0x196c3671, 0x6e6b06e7,
   0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
   0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5,
   0xd6d6a3e8, 0xa1d1937e, 0x38d8c2c4, 0x4fdff252,
   0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
   0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60,
   0xdf60efc3, 0xa867df55, 0x316e8eef, 0x4669be79,
   0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
   0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f,
   0xc5ba3bbe, 0xb2bd0b28, 0x2bb45a92, 0x5cb36a04,
   0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
   0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a,
   0x9c0906a9, 0xeb0e363f, 0x72076785, 0x05005713,
   0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
   0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21,
   0x86d3d2d4, 0xf1d4e242, 0x68ddb3f8, 0x1fda836e,
   0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
   0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c,
   0x8f659eff, 0xf862ae69, 0x616bffd3, 0x166ccf45,
   0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
   0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db,
   0xaed16a4a, 0xd9d65adc, 0x40df0b66, 0x37d83bf0,
   0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
   0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6,
   0xbad03605, 0xcdd70693, 0x54de5729, 0x23d967bf,
   0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
   0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d,
}; // kCrc32Table

class Crc32
{
public:
   Crc32() { Reset(); }
   ~Crc32() throw() {}
   void Reset() { _crc = (uint32_t)~0; }
   void AddData(const uint8_t* pData, const uint32_t length)
   {
      uint8_t* pCur = (uint8_t*)pData;
      uint32_t remaining = length;
      for (; remaining--; ++pCur)
         _crc = ( _crc >> 8 ) ^ kCrc32Table[(_crc ^ *pCur) & 0xff];
   }
   const uint32_t GetCrc32() { return ~_crc; }
   
private:
   uint32_t _crc;
};

std::string my_IP;
int port_number = 8080;
pcrecpp::RE punk("ARE YOU FEELING LUCKY, PUNK\?");
pcrecpp::RE hello("HELLO I'M (.+)\n");
pcrecpp::RE hello_back("HELLO ([^,]+), I'M (.+)\n");
pcrecpp::RE goodbye("GOODBYE (.+)\n");
pcrecpp::RE generate_XYZ("GENERATE (\\d+) BYTES CALLED (\\d+)\n");
pcrecpp::RE get_XYZ_from("GET (\\d+) FROM (.+)\n");
pcrecpp::RE give_me_XYZ("GIVE ME (\\w+)\n");
pcrecpp::RE XYZ_is("(\\d+) IS (.+)\n");

void error(const char *msg)
{
   perror(msg);
   exit(0);
}

void say_hello(int socket_fd, std::string to_IP) {
   char buffer[256];
   std::string hello_message = "HELLO I'M " + my_IP + "\n";
   write(socket_fd,hello_message.c_str(),hello_message.length());
   
   bzero(buffer,256);
   read(socket_fd,buffer,255);
   
   if (!hello_back.FullMatch(buffer)) {
      close(socket_fd);
      error("NO HELLO BACK");
   }
}

void say_hello_back(int socket_fd, std::string to_IP) {
   std::string hello_message = "HELLO " + to_IP + ", I'M " + my_IP + "\n";
   write(socket_fd,hello_message.c_str(),hello_message.length());
}

void say_goodbye(int socket_fd, std::string to_IP) {
   char buffer[256];
   std::string message = "GOODBYE " + to_IP + "\n";
   write(socket_fd, message.c_str(), message.length());
   bzero(buffer,256);
   read(socket_fd, buffer, 255);
   
   if (!goodbye.FullMatch(buffer)) {
      close(socket_fd);
      error("NO GOODBYE BACK");
   }
   close(socket_fd);
}

void say_goodbye_back(int socket_fd, std::string to_IP) {
   std::string message = "GOODBYE " + to_IP + "\n";
   write(socket_fd, message.c_str(), message.length());
   close(socket_fd);
}

void ask_for(int socket_fd, unsigned thing, char *buffer, std::string from_IP) {
   std::stringstream thingout;
   thingout << thing;
   std::string message = "GIVE ME " + thingout.str() + "\n";
   write(socket_fd, message.c_str(), message.length());
   bzero(buffer, 256);
   read(socket_fd, buffer, 255);
   if (punk.FullMatch(buffer)) {
      bzero(buffer, 256);
      read(socket_fd, buffer, 255);
   }
   std::cout << buffer << std::endl;
   say_goodbye(socket_fd, from_IP);
}

void return_checksum(int socket_fd, unsigned thing, u_int32_t checksum) {
   std::stringstream out;
   out.str("");
   out << checksum;
   std::stringstream thingout;
   thingout << thing;
   std::string message = thingout.str() + "'s CHECKSUM IS " + out.str() + "\n";
   write(socket_fd, message.c_str(), message.length());
}

void generate(int socket_fd, unsigned thing, int number) {
   srand(thing);
   static const char alphanum[] =
   "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
   
   Crc32 crc;
   u_int32_t checksum;
   
   FILE *file_handle;
   char XYZ[number];
   bzero(XYZ, number);
   for (int i = 0; i < number; ++i) {
      int r = rand()%(strlen(alphanum));
      XYZ[i] = alphanum[r];
   }
   XYZ[number] = 0;
   std::stringstream out;
   char str[4];
   std::cout << XYZ << std::endl;
   sprintf(str, "%d", thing);
   file_handle = fopen(str, "w");
   fwrite(XYZ, 1, sizeof(XYZ), file_handle);
   fclose(file_handle);
   
   crc.AddData((u_int8_t*)XYZ, (u_int32_t)sizeof(XYZ));
   checksum = crc.GetCrc32();
   crc.Reset();
   return_checksum(socket_fd, thing, checksum);
}

void refuse_XYZ(int socket_fd, std::string to_IP) {
   std::string message = "ARE YOU FEELING LUCKY, PUNK?\n";
   write(socket_fd, message.c_str(), message.length());
   say_goodbye(socket_fd, to_IP);
}

void give_XYZ(int socket_fd, unsigned thing, std::string to_IP) {
   char str[4];
   FILE *file_handle;
   sprintf(str, "%d", thing);
   file_handle = fopen(str, "r");
   if (file_handle == NULL) {
      refuse_XYZ(socket_fd, to_IP);
      return;
   }
   fseek (file_handle , 0 , SEEK_END);
   size_t lSize = ftell (file_handle);
   rewind (file_handle);
   
   char XYZ[lSize];
   fread(XYZ, 1, lSize, file_handle);
   
   std::stringstream out;
   out << XYZ;

   std::stringstream thingout;
   thingout << thing;
   
   std::string message = thingout.str() + " IS " + XYZ + "\n";
   write(socket_fd, message.c_str(), message.length());
}

int connect_to(std::string IP) {
   int socket_fd;
   struct sockaddr_in server_address, my_address;
   socket_fd = socket(AF_INET, SOCK_STREAM, 0);
   
   memset(&server_address, 0, sizeof(server_address));
   server_address.sin_family = AF_INET;
   server_address.sin_port = htons(port_number);
   server_address.sin_addr.s_addr = inet_addr(IP.c_str());
   
   if (connect(socket_fd, (struct sockaddr *) &server_address, sizeof(server_address)) < 0) error("ERROR ON CONNECTION");
   
   socklen_t len = sizeof(my_address);
   getsockname(socket_fd, (struct sockaddr *)&my_address, &len);
   
   std::stringstream out;
   out << inet_ntoa(my_address.sin_addr);
   my_IP = out.str();
   
   say_hello(socket_fd, IP);
   
   return socket_fd;
}

void get_and_return(int socket_fd, unsigned thing, std::string from_IP) {
   int new_socket_fd;
   
   char buffer[256];
   char child_buffer[256];
   int pipe_fds[2];
   int read_fd, write_fd;
   char XYZ[32];
   Crc32 crc;
   u_int32_t checksum;
   pipe(pipe_fds);
   read_fd = pipe_fds[0];
   write_fd = pipe_fds[1];
   int pid = fork();
   if (pid == 0) {
      new_socket_fd = connect_to(from_IP);
      ask_for(new_socket_fd, thing, child_buffer, from_IP);
      close(read_fd);
      write(write_fd, child_buffer, sizeof(buffer));
      sleep(1);
      close(write_fd);
      exit(0);
   }
   else {
      close (write_fd);
      bzero(buffer, 255);
      read(read_fd, buffer, sizeof(buffer));
      close(read_fd);
      std::cout << buffer << std::endl;
   }
   
   if (!XYZ_is.FullMatch(buffer, &thing, XYZ)) error("NOTHING RETURNED");
   
   crc.AddData((u_int8_t*)XYZ, sizeof(XYZ));
   checksum = crc.GetCrc32();
   crc.Reset();
   
   return_checksum(socket_fd, thing, checksum);
}

int main(int argc, const char * argv[])
{
   
   int pid;
   int socket_fd, new_socket_fd;
   socklen_t client_length;
   char buffer[256];
   
   struct sockaddr_in server_address, client_address, our_address;
   
   socket_fd = socket(AF_INET, SOCK_STREAM, 0);
   if (socket_fd < 0) error("ERROR opening socket");
   
   bzero((char *) &server_address, sizeof(server_address));
   server_address.sin_family = AF_INET;
   server_address.sin_addr.s_addr = INADDR_ANY;
   server_address.sin_port = htons(port_number);
   
   std::stringstream out;
   out << server_address.sin_addr.s_addr;
   my_IP = out.str();
   
   if (bind(socket_fd, (struct sockaddr *) &server_address, sizeof(server_address)) < 0) error("BIND FAILED");
   
   listen(socket_fd, 5);
   
   client_length = sizeof(client_address);
   
   std::cout << "Listening on port " << port_number << std::endl;
   
   while (1)
   {
      new_socket_fd = accept(socket_fd, (struct sockaddr *) &client_address, &client_length);
      socklen_t server_length = sizeof(our_address);
      getsockname(new_socket_fd, (struct sockaddr *) &our_address, &server_length);
   
      out.str("");
      out << inet_ntoa(our_address.sin_addr);
      my_IP = out.str();
      out.str("");
      out << inet_ntoa(client_address.sin_addr);
      pid = fork();
      
      if (pid == 0) {
         string IP, from_IP;
         int n;
         unsigned thing;
         close(socket_fd);
         std::cout << "My IP: "<< my_IP << ", Client connected from " << out.str() << std::endl;
         bzero(buffer, 256);
         read(new_socket_fd, buffer, 255);
         std::cout << buffer << std::endl;
         if (hello.FullMatch(buffer, &IP)) say_hello_back(new_socket_fd, IP);
         else {
            error("NO HELLO");
            close(new_socket_fd);
            exit(0);
         }
         while (1) {
            bzero(buffer, 256);
            read(new_socket_fd, buffer, 255);
            std::cout << buffer << std::endl;
            if (hello.FullMatch(buffer, &IP)) say_hello_back(new_socket_fd, IP);
            else if (goodbye.FullMatch(buffer)) {
               say_goodbye_back(new_socket_fd, IP);
               close(new_socket_fd);
               exit(0);
            }
            else if (generate_XYZ.FullMatch(buffer, &n, &thing)) generate(new_socket_fd, thing, n);
            else if (get_XYZ_from.FullMatch(buffer, &thing, &from_IP)) get_and_return(new_socket_fd, thing, from_IP);
            else if (give_me_XYZ.FullMatch(buffer, &thing)) give_XYZ(new_socket_fd, thing, IP);
            else {
               close(new_socket_fd);
               error("DID NOT UNDERSTAND COMMAND");
            }
         }
         exit(0);
      }
      else close(new_socket_fd);
   }
   return 0;
}

