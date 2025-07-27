#include <message.h>

#include <stdlib.h>
#include <string.h>

Message* create_message(char* author, char* content)
{
   Message* m = (Message*)malloc(sizeof(Message));
   if (m)
   {
      m->author = author;
      m->content = content;
   }
   return m;
}

void destroy(Message* m)
{
   free(m->author);
   free(m->content);
   free(m);
}

// MessageArray

MessageArray messagearray()
{
   MessageArray a = { 0, 30, NULL };
   a.messages = (Message**)malloc(a.capacity * sizeof(Message*));
   if (a.messages)
   {
      memset(a.messages, 0, a.capacity);
   }
   return a;
}

Message* get(MessageArray* array, size_t index)
{
   if (index < array->length)
   {
      return array->messages[index];
   }
   return NULL;
}

// Internal
static void resize(MessageArray* array, size_t size)
{
   Message** resized = (Message**)malloc(size * sizeof(Message*));
   if (resized)
   {
      memset(resized, 0, size);
      for (size_t i = 0; i < array->length; ++i)
      {
         resized[i] = get(array, i);
      }
      free(array->messages);
      array->messages = resized;
      array->capacity = size;
   }
}

void push(MessageArray* array, Message* m)
{
   if (array->length == array->capacity)
   {
      resize(array, array->capacity * 2);
   }
   array->messages[array->length] = m;
   ++array->length;
}
