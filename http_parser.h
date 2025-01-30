#ifndef HTTP_PARSER_H
#define HTTP_PARSER_H

#include "http_server.h"

enum State
{
    REQUEST_LINE,
    HEADERS,
    BODY,
    DONE
};

void parse_http_request(char *buffer, size_t length, HttpRequest *httpRequest);
int parse_request_line(char *buffer, size_t *length, char *temp_buffer, size_t *temp_buf_len, HttpRequest *httpRequest);

#endif