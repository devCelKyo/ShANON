#pragma once

#ifdef PLATFORM_WINDOWS
#include <winsock2.h>

typedef struct Client 
{
   char username[20];
   SOCKET socket;
} Client;

// Message must be null-terminated
int client_send(Client, char*);

// Blocking
int client_receive(Client, char* buffer, int len);
#endif