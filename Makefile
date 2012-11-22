file_monitor.so: file_monitor.h
	gcc -FPIC -shared -ldl -o $> $<
