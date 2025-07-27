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
   error = getaddrinfo("192.168.1.11", DEFAULT_PORT, &hints, result);
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

static void announce(char* msg)
{
   printf("-----------\n");
   printf(msg);
   printf("\n");
   printf("-----------\n");
}

DWORD WINAPI handle_client_thread(LPVOID arg)
{
   SOCKET clientSocket = (SOCKET)arg;
   int iReturn;
   char recvbuf[DEFAULT_BUFLEN];
   iReturn = recv(clientSocket, recvbuf, DEFAULT_BUFLEN, 0);

   char username[20];
   strncpy_s(username, iReturn, recvbuf, -1);

   while (TRUE)
   {
      iReturn = recv(clientSocket, recvbuf, DEFAULT_BUFLEN, 0);
      if (iReturn == 0)
      {
         announce("Client dropped.");
         closesocket(clientSocket);
         return 0;
      }
      if (iReturn == SOCKET_ERROR) {
         int error = WSAGetLastError();
         if (error = WSAECONNRESET)
         {
            announce("Client dropped.");
            closesocket(clientSocket);
            return 0;
         }
         printf("recv failed with error: %d\n", WSAGetLastError());
         closesocket(clientSocket);
         WSACleanup();
         return 1;
      }

      printf("[00:00:00] ");
      printf(username);
      printf(": ");
      printf("%.*s\n", iReturn, recvbuf);

      int iSendResult = send(clientSocket, recvbuf, iReturn, 0);
      if (iSendResult == SOCKET_ERROR) {
         printf("send failed with error: %d\n", WSAGetLastError());
         closesocket(clientSocket);
         WSACleanup();
         return 1;
      }
   }
   
   // shutdown the connection since we're done
   shutdown(clientSocket, SD_SEND);
   closesocket(clientSocket);
   
   return 0;
}

static int handle_client(SOCKET clientSocket)
{
   HANDLE thread = CreateThread(NULL, 0, handle_client_thread, (LPVOID)clientSocket, 0, NULL);
   if (!thread)
   {
      return 1;
   }
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

   announce("Now accepting a first client.");
   while (1)
   {
      SOCKET clientSocket = accept(listenSocket, NULL, NULL);
      announce("New client.");
      if (!clientSocket) {
         printf("accept failed with error: %d\n", WSAGetLastError());
         closesocket(listenSocket);
         WSACleanup();
         return 1;
      }

      // Opens a thread for the client
      if (handle_client(clientSocket) != 0)
      {
         return 1;
      }
   }
   
   // cleanup
   WSACleanup();

   return 0;
}