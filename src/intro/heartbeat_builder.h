#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>


void concat_string(char** ptr, const char* str)
{
       size_t length = strlen(str);

       if ( length >= 32 )
       {
               printf("Does not allow string longer than 31 currently.");
               exit(1);
       }
       *((*ptr)++) = 0xA0 | ((length) & 0x1F);
       memcpy(*ptr, str, length);
       (*ptr) += length;
}

void concat_uint64(char** ptr, uint64_t value)
{
       *((*ptr)++) = 0xCF;
       *((*ptr)++) = (value >> 56) & 0xFF;
       *((*ptr)++) = (value >> 48) & 0xFF;
       *((*ptr)++) = (value >> 40) & 0xFF;
       *((*ptr)++) = (value >> 32) & 0xFF;
       *((*ptr)++) = (value >> 24) & 0xFF;
       *((*ptr)++) = (value >> 16) & 0xFF;
       *((*ptr)++) = (value >>  8) & 0xFF;
       *((*ptr)++) = (value )      & 0xFF;
}

void heartbeat_msg_builider(
       char** msg_head,
       size_t* msg_size,
       const char* host,
       const char* service_type,
       const char* instance,
       const char* version,
       char status,
       const char* comments,
       uint64_t start_time,
       uint64_t update_time
)
{
       static const size_t MAX_MSG_LENGTH = 200;
       char* ptr;

       *msg_head = (char*)malloc(MAX_MSG_LENGTH);
       ptr = *msg_head;

       // Message Head
       *(ptr++) = 0x98;

       concat_string(&ptr, host);
       concat_string(&ptr, service_type);
       concat_string(&ptr, instance);
       concat_string(&ptr, version);

       *(ptr++) = status;

       concat_string(&ptr, comments);

       concat_uint64(&ptr, start_time);
       concat_uint64(&ptr, update_time);

       *msg_size = (size_t)(ptr-*msg_head);
}

void userdata_msg_builder(
       char** msg_head,
       size_t* msg_size,
       uint64_t crc,
       const char* protocol_version
)
{
       static const size_t UD_LENGTH = 20;
       char *ptr;

       *msg_head = (char*)malloc(UD_LENGTH);
       *msg_size = UD_LENGTH;

       ptr = *msg_head;

       *(ptr++) = ((crc >> 24) & 0xFF);
       *(ptr++) = ((crc >> 16) & 0xFF);
       *(ptr++) = ((crc >>  8) & 0xFF);
       *(ptr++) = ((crc )      & 0xFF);

       size_t length = strlen(protocol_version);
       length = length > 12 ? 12: length;

       memcpy(ptr, protocol_version, length);
       memset(ptr+length, 0, 16-length);
}
