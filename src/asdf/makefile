all: build
	@./mycp makefile foo/

build: mycp.c
	@gcc mycp.c -o mycp

clean:
	@rm -rf mycp bar
