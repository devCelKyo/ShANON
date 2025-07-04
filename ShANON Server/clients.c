#ifdef PLATFORM_WINDOWS
#include <clients.h>

int client_send(Client client, char* message)
{
   return send(client.socket, message, (int)strlen(message), 0);
}

int client_receive(Client client, char* buffer, int len)
{
   return recv(client.socket, buffer, len, 0);
}

#elif defined(PLATFORM_MAC)
#endif