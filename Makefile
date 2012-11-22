all: file_monitor32.so file_monitor64.so

file_monitor32.so: file_monitor.c
	gcc -m32 -fPIC -shared -ldl -o $@ $^

file_monitor64.so: file_monitor.c
	gcc -m64 -fPIC -shared -ldl -o $@ $^

install: file_monitor32.so file_monitor64.so
	cp file_monitor32.so file_monitor64.so ~/bin/file_monitor-libs/
	cp file_monitor ~/bin/

clean:
	rm -f file_monitor32.so file_monitor64.so
