#include "http_parser.h"
#include <stdio.h>

void parse_http_request(char *buffer, size_t length, HttpRequest *httpRequest)
{
    static enum State state = REQUEST_LINE;
    static char temp_buffer[1024];
    static size_t temp_buf_len = 0;

    while (length > 0)
    {
        switch (state)
        {
        case REQUEST_LINE:
            printf("I'm in request line.\n");
            printf("buffer: %s\n", buffer);
            printf("length: %d\n", length);
            printf("temp_buffer: %s\n", temp_buffer);
            printf("temp_buf_len: %d\n", temp_buf_len);
            if (parse_request_line(buffer, &length, temp_buffer, &temp_buf_len, httpRequest) == 0)
            {
                state = HEADERS;
            }
            printf("temp_buffer after: %s\n\n", temp_buffer);
            Sleep(500);
            break;

        case HEADERS:
            printf("I'm in headers line.");
            break;

        case BODY:
            printf("I'm in body line.");
            break;
        default:
            fprintf(stderr, "Switch type was unexpected");
        }
    }
}

int parse_request_line(char *buffer, size_t *length, char *temp_buffer, size_t *temp_buf_len, HttpRequest *httpRequest)
{
    if (length <= 0)
    {
        fprintf(stderr, "Cannot parse request line from an empty buffer");
        return 1;
    }

    char *endline = strstr(buffer, "\r\n");
    if (endline == NULL)
    {
        strcat(temp_buffer, buffer);
        *temp_buf_len += *length;
        *length = 0;
        memset(buffer, 0, sizeof(buffer)); // clear buffer
        return 1;
    }

    int length_to_endline = buffer - endline;
    char buffer_to_endline[1024];
    strncpy(buffer_to_endline, buffer, length_to_endline);
    buffer_to_endline[length_to_endline] = '\0';
    strcat(temp_buffer, buffer_to_endline); // temp_buffer should have the whole request line now
    temp_buf_len += length_to_endline + 1;

    // HTTP request line parsing
    int scanRes = sscanf(temp_buffer, "%s %s %s", httpRequest->method, httpRequest->path, httpRequest->protocol);
    if (scanRes != 3)
    {
        fprintf(stderr, "Cannot parse incorrect request line\n");
    }
    else
    {
        // Validate method
        int cmpRes = strcmp("GET", httpRequest->method);
        if (cmpRes != 0)
        {
            fprintf(stderr, "Unsupported HTTP method of '%s' was specified\n", httpRequest->method);
        }
        else
        {
            printf("Method: %s\n", httpRequest->method);
        }

        // Validate URI - only accept absolute path. No query or fragment parameters
        const char *slash = strchr(httpRequest->path, '/');
        if (httpRequest->path - slash != 0)
        {
            fprintf(stderr, "URI '%s' is not an absolute path\n", httpRequest->path);
        }
        else
        {
            printf("URI: %s\n", httpRequest->path);
        }

        // Validate version - only accept HTTP/1.1
        if (strcmp(httpRequest->protocol, "HTTP/1.1") != 0)
        {
            fprintf(stderr, "Unsupported HTTP protocol version of '%s' was specified\n", httpRequest->protocol);
        }
        else
        {
            printf("Version: %s\n", httpRequest->protocol); // check for delimeters at the end
        }
    }
}