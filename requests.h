// #include <stdlib.h>     
// #include <stdio.h>
// #include <string>
// #include <unistd.h>    
// #include <string.h>     
// #include <sys/socket.h> 
// #include <netinet/in.h> 
// #include <netdb.h>      
// #include <arpa/inet.h>

#ifndef _REQUESTS_
#define _REQUESTS_



using std :: string;


char *compute_get_request(char *host, char *url, char *query_params,
							string cookies[50], int cookies_count);

char *compute_post_request( char *host,  char *url,  char* content_type, string body_data,
							int body_data_fields_count, string cookies[50], int cookies_count);

char *compute_delete_request(char *host, char *url, char *query_params,
                            string cookies[50], int cookies_count);


#endif
