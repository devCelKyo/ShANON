#pragma once

#include <stddef.h>

typedef struct
{
   char* author;
   char* content;
} Message;

Message* create_message(char*, char*);
void destroy(Message*);

typedef struct
{
   size_t length;
   size_t capacity;
   Message** messages;
} MessageArray;

MessageArray messagearray();
Message* get(MessageArray*, size_t);
void push(MessageArray*, Message*);