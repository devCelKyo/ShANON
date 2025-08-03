#pragma once

#include <winsock2.h>

#include <dynamic_array.h>

typedef struct Client 
{
   char username[20];
   SOCKET socket;
} Client;

Client* client(char*, SOCKET);
void destroy_client(Client*);

// Message must be null-terminated
int client_send(Client, char*);

// Blocking
int client_receive(Client, char* buffer, int len);

typedef struct ClientList
{
   DynamicArray arr;
} ClientList;

ClientList clientlist();
Client* clientlist_get(ClientList*, size_t);
void clientlist_push(ClientList*, Client*);

void clientlist_broadcast(ClientList*, int sizeInBytes, char* payload);