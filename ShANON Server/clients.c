#include <clients.h>

Client* client(char* username, SOCKET socket)
{
   Client* c = malloc(sizeof(Client));
   if (c)
   {
      c->socket = socket;
      strcpy_s(c->username, 20, username);
   }
   return c;
}

void destroy_client(Client* client)
{
   free(client);
}

int client_send(Client client, char* message)
{
   return send(client.socket, message, (int)strlen(message), 0);
}

int client_receive(Client client, char* buffer, int len)
{
   return recv(client.socket, buffer, len, 0);
}

// ClientList

ClientList clientlist()
{
   ClientList c = { dynarray() };
   return c;
}

Client* clientlist_get(ClientList* list, size_t index)
{
   return (Client*)dynarray_get(&list->arr, index);
}

void clientlist_push(ClientList* list, Client* client)
{
   dynarray_push(&list->arr, (void*)client);
}

void clientlist_broadcast(ClientList* list, int sizeInBytes, char* payload)
{
   size_t len = list->arr.length;
   for (size_t index = 0; index < len; ++index)
   {
      Client* current = clientlist_get(list, index);
      send(current->socket, payload, sizeInBytes, 0);
   }
}