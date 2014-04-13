tar=foo
dest=bar

all: mycp.c
	@-gcc mycp.c -o mycp && ./mycp $(tar) $(dest)
	@rm -f mycp

run: mycp
	@./mycp $(tar) $(dest)

clean:
	@rm -f mycp
