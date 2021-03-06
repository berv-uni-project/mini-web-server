#include <event.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include "cJSON.h"

#define MAX_PATH_LEN 1024
#define DEFAULT_PORT 12345

char uri_root[512];
char doc_root[100];
char name_server[100];
unsigned short port;

struct client {
	int fd;
	struct bufferevent *buf_ev;
};

int set_nonblock(int fd) {
	int flags;

	flags = fcntl(fd, F_GETFL);
	flags |= O_NONBLOCK;
	fcntl(fd, F_SETFL, flags);
}

void err(int exit_code, char * error_text) {
	perror(error_text);
	exit(1);
}

void parse_request(char* request, char** output_path) {
	char *ptr;
	int path_len;

	// don't care about the type of the request, just find the second arg
	ptr = request;
	while (*ptr != 0 && *ptr != ' ') {
		++ptr;
	}
	if (*ptr == 0) {
		*output_path = NULL;
		return;
	}

	++ptr;
	path_len = 0;
	while (ptr[path_len] != 0 &&
			ptr[path_len] != ' ' &&
			ptr[path_len] != '#' &&
			ptr[path_len] != '&') {
		++path_len;
	}
	*output_path = (char*) calloc(MAX_PATH_LEN, sizeof(char));
	strcpy(*output_path, doc_root);
	strncat(*output_path, ptr, path_len);
}

int get_default_file(char** path) {
	struct stat st;
	int err;

	err = stat(*path, &st);
	if (err < 0) {
		return err;
	} else {
		if (S_ISDIR(st.st_mode)) {
			if ((*path)[strlen(*path) - 1] == '/') {
				strcat(*path, "index.html");
			} else {
				strcat(*path, "/index.html");
			}
			err = stat(*path, &st);
			if (err < 0) {
				return err;
			} else if (S_ISDIR(st.st_mode)) {
				return -1;
			} else {
				return st.st_size;
			}
		} else {
			return st.st_size;
		}
	}
}

void add_404_response(struct evbuffer *event_response) {
	evbuffer_add_printf(event_response, "HTTP/1.1 404 Not Found\n");
	evbuffer_add_printf(event_response, "Content-Type: text/plain\n");
	evbuffer_add_printf(event_response, "Content-Length: 9\n");
	evbuffer_add_printf(event_response, "\n");
	evbuffer_add_printf(event_response, "Not Found\n");
}

void add_200_response(struct evbuffer *event_response, char* path, int fsize) {
	FILE* f;

	evbuffer_add_printf(event_response, "HTTP/1.1 200 OK\n");
	evbuffer_add_printf(event_response, "Content-Type: text/html\n");
	evbuffer_add_printf(event_response, "Content-Length: %d\n", fsize);
	evbuffer_add_printf(event_response, "\n");

	printf("Loading file %s ...\n", path);
	f = fopen(path, "r");
	evbuffer_add_file(event_response, fileno(f), 0, fsize);
}

void buf_read_callback(struct bufferevent *incoming, void *arg) {
	struct evbuffer *event_response;
	char *request, *path, *response;
	int fsize;

	request = evbuffer_readln(incoming->input, NULL, EVBUFFER_EOL_ANY);
	if (request == NULL) {
		return;
	}
	// skip the rest of request
	while (evbuffer_readln(incoming->input, NULL, EVBUFFER_EOL_ANY) != NULL);
	event_response = evbuffer_new();

	printf("Processing request: %s\n", request);
	parse_request(request, &path);
	fsize = get_default_file(&path);

	if (fsize < 0) {
		add_404_response(event_response);
	} else {
		add_200_response(event_response, path, fsize);
	}

	bufferevent_write_buffer(incoming, event_response);
	evbuffer_free(event_response);
	free(request);
	free(path);
}

void buf_error_callback(struct bufferevent *bev, short what, void *arg) {
	struct client *client = (struct client *)arg;
	bufferevent_free(client->buf_ev);
	close(client->fd);
	free(client);
}

void accept_callback(int fd, short ev, void *arg) {
	int client_fd;
	struct sockaddr_in client_addr;
	socklen_t client_len = sizeof(client_addr);
	struct client *client;
	struct timeval *timeout;

	client_fd = accept(fd, (struct sockaddr *)&client_addr, &client_len);
	if (client_fd < 0) {
		printf("%s\n", "Client: accept() failed");
		return;
	}

	set_nonblock(client_fd);

	client = calloc(1, sizeof(*client));
	if (client == NULL) {
		err(1, "Failed to allocate memory on client");
	}
	client->fd = client_fd;

	client->buf_ev = bufferevent_new(
			client_fd,
			buf_read_callback,
			NULL,
			buf_error_callback,
			client);

	bufferevent_enable(client->buf_ev, EV_READ);

	timeout = (struct timeval*)malloc(sizeof(struct timeval*));
	timeout->tv_usec = 100000;
	bufferevent_set_timeouts(client->buf_ev, timeout, NULL);
}

int initialize_socket() {
	int sock;
	struct sockaddr_in serv_addr;

	if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP)) < 0) {
		close(sock);
		err(1, "Cannot open socket\n");
	}

	int reusable = 1;
	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &reusable, sizeof (reusable)) == -1) {
		err(1, "Cannot set option\n");
	}

	bzero((char *) &serv_addr, sizeof (serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(port);

	if (bind(sock, (struct sockaddr *) &serv_addr, sizeof (serv_addr)) < 0) {
		close(sock);
		err(1, "Failed to bind socket\n");
	}

	if (listen(sock, 5) < 0) {
		close(sock);
		err(1, "Failed to listen to socket\n");
	}

	set_nonblock(sock);

	return sock;
}

char * read_file(char * filename) {
   char *buffer = NULL;
   int string_size, read_size;
   FILE *handler = fopen(filename, "r");

   if (handler)
   {
       // Seek the last byte of the file
       fseek(handler, 0, SEEK_END);
       // Offset from the first to the last byte, or in other words, filesize
       string_size = ftell(handler);
       // go back to the start of the file
       rewind(handler);

       // Allocate a string that can hold it all
       buffer = (char*) malloc(sizeof(char) * (string_size + 1) );

       // Read it all in one operation
       read_size = fread(buffer, sizeof(char), string_size, handler);

       // fread doesn't set it so put a \0 in the last position
       // and buffer is now officially a string
       buffer[string_size] = '\0';

       if (string_size != read_size)
       {
           // Something went wrong, throw away the memory and set
           // the buffer to NULL
           free(buffer);
           buffer = NULL;
       }

       // Always remember to close the file.
       fclose(handler);
    }

    return buffer;
}

void configure() {
	char * config_location = "config/config.json";
	char * json_string = read_file(config_location);
	cJSON * root = cJSON_Parse(json_string);
	cJSON * port_item = cJSON_GetObjectItemCaseSensitive(root, "port");
	cJSON * directory_item = cJSON_GetObjectItemCaseSensitive(root, "directory");
	cJSON * name_server_item = cJSON_GetObjectItemCaseSensitive(root, "name_server");
	int port_value = 0;
	if (cJSON_IsNumber(port_item)) {
		port_value = port_item->valueint;
		if (port_value > 0) {
			port = port_value;
		} else {
			port = DEFAULT_PORT;
		}
	}
	if (cJSON_IsString(directory_item)) {
		strcpy(doc_root, directory_item->valuestring);
	} else {
		strcpy(doc_root, "htdocs");
	}
	if (cJSON_IsString(name_server_item)) {
		strcpy(name_server, name_server_item->valuestring);
	} else {
		strcpy(name_server, "localhost");
	}
	cJSON_Delete(root);
}

void show_license() {
	char * license = read_file("LICENSE-TEXT");
	printf("%s\n", license);
}

int main(int argc, char **argv) {
	configure();
	show_license();

	int sock;
	struct event accept_event;
	sock = initialize_socket();

	event_init();
	event_set(&accept_event, sock, EV_READ|EV_PERSIST, accept_callback, NULL);
	event_add(&accept_event, NULL);
	event_dispatch();

	close(sock);

	return 0;
}
