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

#include <message.h>

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

typedef struct addrinfo addrinfo;

static int setup_client(char* server_addr, addrinfo** result)
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

   // Resolve the server address and port
   error = getaddrinfo(server_addr, DEFAULT_PORT, &hints, result);
   if (error != 0) {
      printf("getaddrinfo failed with error: %d\n", error);
      WSACleanup();
      return 1;
   }
   return error;
}

static SOCKET connect_to_server(addrinfo* result, char* username)
{
   SOCKET connectSocket = INVALID_SOCKET;
   addrinfo* ptr = NULL;
   for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {
      connectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
      if (!connectSocket) {
         printf("socket failed with error: %ld\n", WSAGetLastError());
         WSACleanup();
         return INVALID_SOCKET;
      }

      if (connect(connectSocket, ptr->ai_addr, (int)ptr->ai_addrlen) == SOCKET_ERROR) {
         closesocket(connectSocket);
         connectSocket = INVALID_SOCKET;
         continue;
      }
      break;
   }

   if (!connectSocket) {
      printf("Unable to connect to server!\n");
      WSACleanup();
   }

   int iResult = send(connectSocket, username, (int)strlen(username), 0);
   if (!iResult) {
      printf("Unable to connect to server!\n");
      WSACleanup();
   }
   return connectSocket;
}

DWORD WINAPI message_prompt_loop(LPVOID arg)
{
   SOCKET connectSocket = (SOCKET)arg;
   char txt[50];
   int iResult;
   do
   {
      printf("\x1B[999;1H"); // Move to row 999, column 1
      printf("Your message: ");
      fgets(txt, sizeof(txt), stdin);
      printf("\x1B[1A"); // move up one line
      printf("\x1B[2K"); // clear line
      iResult = send(connectSocket, txt, 50, 0);

   } while (iResult != 0);
   return 0;
}

int main(int argc, char* argv[])
{
   char serverIp[16];
   if (argc != 2) {
      printf("Server address: ");
      fgets(serverIp, sizeof(serverIp), stdin);
   }
   else
   {
      strcpy_s(serverIp, 16, argv[1]);
   }

   char username[20];
   printf("Username: ");
   fgets(username, sizeof(username), stdin);

   addrinfo* result = NULL;
   if (setup_client(argv[1], &result))
   {
      return 1;
   }
   
   SOCKET connectSocket = connect_to_server(result, username);
   freeaddrinfo(result);
   
   if (!connectSocket)
   {
      return 1;
   }

   system("cls");
   printf("Now connected to server. \n");

   int iResult;
   char recvBuffer[DEFAULT_BUFLEN];

   HANDLE promptThread = CreateThread(NULL, 0, message_prompt_loop, (LPVOID)connectSocket, 0, NULL);

   MessageArray messages = messagearray();
   int messageLimit = 10;
   do
   {
      iResult = recv(connectSocket, recvBuffer, DEFAULT_BUFLEN, 0);
      
      // If a message was broadcasted from server
      if (iResult > 0)
      {
         char* author = (char*)malloc(20 * sizeof(char));
         if (author)
         {
            //strcpy_s(author, 20, recvBuffer);
         }
         char* content = (char*)malloc(300 * sizeof(char));
         if (content)
         {
            //strcpy_s(content, 300, recvBuffer + 20);
            strcpy_s(content, iResult, recvBuffer);
         }
         Message* m = create_message(author, content);
         push(&messages, m);

         // refreshing all the messages when receiving one
         printf("\x1B[s");
         int firstMessage = max((int)messages.length - messageLimit, 0);
         for (int index = firstMessage; index < messages.length; ++index)
         {
            char* content;
            Message* currentMessage = get(&messages, index);
            if (currentMessage)
            {
               content = currentMessage->content;
            }
            else
            {
               content = "slt";
            }
            printf("\x1B[%d;1H", index - firstMessage + 1);
            printf("\x1B[2K");
            char* author = "author";
            printf("%s: %s", author, content);
         }
         printf("\x1B[u");
      }
   } while (iResult != 0);

   iResult  = shutdown(connectSocket, SD_SEND);
   if (iResult  == SOCKET_ERROR) {
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