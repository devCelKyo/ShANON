#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

typedef struct addrinfo addrinfo;

static int setup_server(addrinfo** result)
{
   WSADATA wsaData;
   int error = WSAStartup(MAKEWORD(2, 2), &wsaData);
   if (error != 0) {
      printf("WSAStartup failed with error: %d\n", error);
      return 1;
   }

   addrinfo hints;
   memset(&hints, 0, sizeof(hints));
   hints.ai_family = AF_UNSPEC;
   hints.ai_socktype = SOCK_STREAM;
   hints.ai_protocol = IPPROTO_TCP;
   hints.ai_flags = AI_PASSIVE;

   // Resolve the server address and port
   error = getaddrinfo("localhost", DEFAULT_PORT, &hints, result);
   if (error != 0) {
      printf("getaddrinfo failed with error: %d\n", error);
      WSACleanup();
      return 1;
   }
   return 0;
}

static int create_listen_socket(addrinfo* serverAdressInfo, SOCKET* listenSocket)
{
   *listenSocket = socket(serverAdressInfo->ai_family, serverAdressInfo->ai_socktype, serverAdressInfo->ai_protocol);
   if (!*listenSocket) {
      printf("socket failed with error: %ld\n", WSAGetLastError());
      freeaddrinfo(serverAdressInfo);
      WSACleanup();
      return 1;
   }

   // Setup the TCP listening socket
   if (bind(*listenSocket, serverAdressInfo->ai_addr, (int)serverAdressInfo->ai_addrlen) == SOCKET_ERROR) {
      printf("bind failed with error: %d\n", WSAGetLastError());
      freeaddrinfo(serverAdressInfo);
      closesocket(*listenSocket);
      WSACleanup();
      return 1;
   }
   return 0;
}

static int handle_client(SOCKET clientSocket)
{
   char recvbuf[DEFAULT_BUFLEN];
   int error = recv(clientSocket, recvbuf, DEFAULT_BUFLEN, 0);
   printf(recvbuf);

   // Echo the buffer back to the sender
   int iSendResult = send(clientSocket, recvbuf, error, 0);
   if (iSendResult == SOCKET_ERROR) {
      printf("send failed with error: %d\n", WSAGetLastError());
      closesocket(clientSocket);
      WSACleanup();
      return 1;
   }
   printf(recvbuf);
   return 0;
}

int main()
{
   addrinfo* serverAdressInfo = NULL;
   if (setup_server(&serverAdressInfo) != 0)
   {
      return 1;
   }

   SOCKET listenSocket = INVALID_SOCKET; 
   if (create_listen_socket(serverAdressInfo, &listenSocket) != 0)
   {
      return 1;
   }
   freeaddrinfo(serverAdressInfo);

   int error = listen(listenSocket, SOMAXCONN);
   if (error == SOCKET_ERROR) {
      printf("listen failed with error: %d\n", WSAGetLastError());
      closesocket(listenSocket);
      WSACleanup();
      return 1;
   }

   while (1)
   {
      SOCKET clientSocket = accept(listenSocket, NULL, NULL);
      if (!clientSocket) {
         printf("accept failed with error: %d\n", WSAGetLastError());
         closesocket(listenSocket);
         WSACleanup();
         return 1;
      }

      if (handle_client(clientSocket) != 0)
      {
         return 1;
      }

      // shutdown the connection since we're done
      error = shutdown(clientSocket, SD_SEND);
      if (error == SOCKET_ERROR) {
         printf("shutdown failed with error: %d\n", WSAGetLastError());
         closesocket(clientSocket);
         WSACleanup();
         return 1;
      }
      closesocket(clientSocket);
   }
   
   // cleanup
   WSACleanup();

   return 0;
} 