#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include <netinet/tcp.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>

#include "http.h"


/**
 * The socket_connect() function establishes a connection to a specified host on a 
 * specified port. It takes two parameters, the first is the hostname to connect to 
 * and the second is the port number. The function uses the gethostbyname() function 
 * to retrieve the IP address associated with the hostname. If the function fails, it prints
 * an error message and returns -2. The function then creates a socket using the socket() 
 * function and assigns it to sock. If the function fails, it prints an error message and 
 * returns -1. Finally, the function uses the connect() function to connect to the specified
 * host and port. If the function fails, it prints an error message, closes the socket, 
 * and returns -1. If the connection is successful, it returns the socket descriptor.
*/
int socket_connect(const char *host, uint16_t port){
    struct hostent *hp;
    struct sockaddr_in addr;
    int sock;

    if((hp = gethostbyname(host)) == NULL){
		herror("gethostbyname");
		return -2;
	}
	bcopy(hp->h_addr, &addr.sin_addr, hp->h_length);
	addr.sin_port = htons(port);
	addr.sin_family = AF_INET;
	sock = socket(PF_INET, SOCK_STREAM, 0); 
	
	if(sock == -1){
		perror("socket");
		return -1;
	}

    if(connect(sock, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) == -1){
		perror("connect");
		close(sock);
        return -1;
	}

    return sock;
}

/**
 * The get_http_header_len() function finds the length of the HTTP header in a 
 * given buffer. It takes two parameters, the first is a buffer containing the 
 * HTTP response, and the second is the length of the buffer. The function uses 
 * the strnstr() function to find the position of the end of the HTTP header in 
 * the buffer. If the end of the header is not found, it prints an error message 
 * and returns -1. Otherwise, it calculates and returns the length of the HTTP header.
*/
int get_http_header_len(char *http_buff, int http_buff_len){
    char *end_ptr;
    int header_len = 0;
    end_ptr = strnstr(http_buff,HTTP_HEADER_END,http_buff_len);

    if (end_ptr == NULL) {
        fprintf(stderr, "Could not find the end of the HTTP header\n");
        return -1;
    }

    header_len = (end_ptr - http_buff) + strlen(HTTP_HEADER_END);

    return header_len;
}

/**
 * The get_http_content_len() function finds the length of the HTTP content in a given 
 * buffer. It takes two parameters, the first is a buffer containing the HTTP response, 
 * and the second is the length of the HTTP header in the buffer. The function searches 
 * for the "Content-Length" header in the HTTP header and extracts the value associated 
 * with it. If the header is not found, it prints an error message and returns 0. 
 * Otherwise, it converts the value to an integer and returns it.
*/
int get_http_content_len(char *http_buff, int http_header_len){
    char header_line[MAX_HEADER_LINE];

    char *next_header_line = http_buff;
    char *end_header_buff = http_buff + http_header_len;

    while (next_header_line < end_header_buff){
        bzero(header_line,sizeof(header_line));
        sscanf(next_header_line,"%[^\r\n]s", header_line);

        char *isCLHeader = strcasestr(header_line,CL_HEADER);
        if(isCLHeader != NULL){
            char *header_value_start = strchr(header_line, HTTP_HEADER_DELIM);
            if (header_value_start != NULL){
                char *header_value = header_value_start + 1;
                int content_len = atoi(header_value);
                return content_len;
            }
        }
        next_header_line += strlen(header_line) + strlen(HTTP_HEADER_EOL);
    }
    fprintf(stderr,"Did not find content length\n");
    return 0;
}

//This function just prints the header
void print_header(char *http_buff, int http_header_len){
    fprintf(stdout, "%.*s\n",http_header_len,http_buff);
}

int process_http_header(char *http_buff, int http_buff_len, int *header_len, int *content_len){
    int h_len, c_len = 0;
    h_len = get_http_header_len(http_buff, http_buff_len);
    if (h_len < 0) {
        *header_len = 0;
        *content_len = 0;
        return -1;
    }
    c_len = get_http_content_len(http_buff, http_buff_len);
    if (c_len < 0) {
        *header_len = 0;
        *content_len = 0;
        return -1;
    }

    *header_len = h_len;
    *content_len = c_len;
    return 0; //success
}