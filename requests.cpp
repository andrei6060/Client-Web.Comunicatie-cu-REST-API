#include <stdlib.h>     
#include <stdio.h>
#include <string>
#include <unistd.h>     
#include <string.h>    
#include <sys/socket.h>
#include <netinet/in.h> 
#include <netdb.h>      
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"
//#include <String>

using std :: string;
char *compute_get_request(char *host, char *url, char *query_params,
                            string cookies[50], int cookies_count)
{
    char *message = (char*)calloc(BUFLEN, sizeof(char));
    char *line = (char*)calloc(LINELEN, sizeof(char));

    if (query_params != NULL) {
        sprintf(line, "GET %s?%s HTTP/1.1", url, query_params);
    } else {
        sprintf(line, "GET %s HTTP/1.1", url);
    }

    compute_message(message, line);

    sprintf(line, "Host: %s", host);
    compute_message(message, line);

   
    if (cookies != NULL) {
         for (int i = 0; i < cookies_count; i++) {
      compute_message(message, cookies[i].c_str());
    }
       
    }
   
    compute_message(message, "");
    return message;
}

char *compute_post_request(char *host, char *url, char* content_type, string body_data,
                            int body_data_fields_count, string cookies[50], int cookies_count)
{
    char *message = (char*)calloc(BUFLEN, sizeof(char));
    char *line = (char*)calloc(LINELEN, sizeof(char));
    char *body_data_buffer = (char*)calloc(LINELEN, sizeof(char));

    int content_len = body_data.length();

   
    sprintf(line, "POST %s HTTP/1.1", url);
    compute_message(message, line);

    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    sprintf(line, "Content-Type: %s", content_type);
    compute_message(message, line);

    sprintf(line, "Content-Length: %d", content_len);
    compute_message(message, line);
    

    if (cookies != NULL) {
        for (int i = 0; i < cookies_count; i++) {
        compute_message(message, cookies[i].c_str());
    }
    }
    

    compute_message(message, "");

    memset(line, 0, LINELEN);
    if (body_data_fields_count > 0) {
        compute_message(message, body_data.c_str());
    }

    free(line);
    return message;
}
char *compute_delete_request(char *host, char *url, char *query_params,
                            string cookies[50], int cookies_count)
{
    char *message = (char*)calloc(BUFLEN, sizeof(char));
    char *line = (char*)calloc(LINELEN, sizeof(char));

  
    if (query_params != NULL) {
        sprintf(line, "DELETE %s?%s HTTP/1.1", url, query_params);
    } else {
        sprintf(line, "DELETE %s HTTP/1.1", url);
    }

    compute_message(message, line);

    sprintf(line, "Host: %s", host);
    compute_message(message, line);

  
    if (cookies != NULL) {
         for (int i = 0; i < cookies_count; i++) {
      compute_message(message, cookies[i].c_str());
    }
       
    }
  
    compute_message(message, "");
    return message;
}

