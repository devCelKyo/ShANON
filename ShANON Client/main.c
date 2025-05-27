#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>


// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

typedef struct addrinfo addrinfo;

int main(int argc, char* argv[])
{
   if (argc != 2) {
      printf("usage: %s server-name\n", argv[0]);
      return 1;
   }

   int iResult;

   struct addrinfo* result = NULL;

   WSADATA wsaData;
   iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
   if (iResult != 0) {
      printf("WSAStartup failed with error: %d\n", iResult);
      return 1;
   }

   addrinfo hints;
   memset(&hints, 0, sizeof(hints));
   hints.ai_family = AF_UNSPEC;
   hints.ai_socktype = SOCK_STREAM;
   hints.ai_protocol = IPPROTO_TCP;

   // Resolve the server address and port
   iResult = getaddrinfo(argv[1], DEFAULT_PORT, &hints, &result);
   if (iResult != 0) {
      printf("getaddrinfo failed with error: %d\n", iResult);
      WSACleanup();
      return 1;
   }

   SOCKET connectSocket = INVALID_SOCKET;
   addrinfo* ptr = NULL;
   for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {
      connectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
      if (!connectSocket) {
         printf("socket failed with error: %ld\n", WSAGetLastError());
         WSACleanup();
         return 1;
      }

      iResult = connect(connectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
      if (iResult == SOCKET_ERROR) {
         closesocket(connectSocket);
         connectSocket = INVALID_SOCKET;
         continue;
      }
      break;
   }

   freeaddrinfo(result);

   if (!connectSocket) {
      printf("Unable to connect to server!\n");
      WSACleanup();
      return 1;
   }

   const char* sendbuf = "this is a test";
   iResult = send(connectSocket, sendbuf, (int)strlen(sendbuf), 0);
   if (iResult == SOCKET_ERROR) {
      printf("send failed with error: %d\n", WSAGetLastError());
      closesocket(connectSocket);
      WSACleanup();
      return 1;
   }

   char recvbuf[DEFAULT_BUFLEN];
   // while the peer doesn't close the connection
   do {

      iResult = recv(connectSocket, recvbuf, DEFAULT_BUFLEN, 0);
      if (iResult > 0)
         printf("Bytes received: %d\n", iResult);
      else if (iResult == 0)
         printf("Connection closed\n");
      else
         printf("recv failed with error: %d\n", WSAGetLastError());

   } while (1);

   iResult = shutdown(connectSocket, SD_SEND);
   if (iResult == SOCKET_ERROR) {
      printf("shutdown failed with error: %d\n", WSAGetLastError());
      closesocket(connectSocket);
      WSACleanup();
      return 1;
   }

   // cleanup
   closesocket(connectSocket);
   WSACleanup();

   return 0;
}