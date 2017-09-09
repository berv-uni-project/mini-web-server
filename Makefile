all:
	gcc cJSON.c server-evhttp.c -o mini-web-server -levent
