dest_directory = /opt/mws/
program_name = mini-web-server

all:
	gcc cJSON.c server-evhttp.c -o $(program_name) -levent

install:
	gcc cJSON.c server-evhttp.c -o $(program_name) -levent
	test -d $(dest_directory) || mkdir -p $(dest_directory) && cp $(program_name) $(dest_directory)
	cp -R config $(dest_directory)
	ln -s $(dest_directory)$(program_name) /usr/bin/$(program_name) 
	rm mini-web-server
	
remove:
	rm -Rf $(dest_directory)
	rm -f /usr/bin/$(program_name)

