all: build
	@./mycp foo -R bar

build: mycp.c
	@gcc mycp.c -o mycp

clean:
	@rm -rf mycp bar
