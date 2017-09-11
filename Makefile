dest_directory = /opt/mws/
program_name = mini-web-server
small_program = mws

all:
	gcc cJSON.c server-evhttp.c -o $(program_name) -levent

small:
	gcc cJSON.c server.c -o $(small_program) -levent

install:
	gcc cJSON.c server-evhttp.c -o $(program_name) -levent
	gcc cJSON.c server.c -o $(small_program) -levent
	test -d $(dest_directory) || mkdir -p $(dest_directory) && cp -R . $(dest_directory)
	ln -sf $(dest_directory)$(program_name) /usr/bin/$(program_name)
	ln -sf $(dest_directory)$(small_program) /usr/bin/$(small_program)
	rm mini-web-server
	rm mws

remove:
	rm -Rf $(dest_directory)
	rm -f /usr/bin/$(program_name)
	rm -f /usr/bin/$(small_program)
