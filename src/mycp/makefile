all: build
	@./mycp foo bar
	@rm -f mycp

build: mycp.c
	@gcc mycp.c -o mycp

clean:
	@rm -rf mycp bar

show: foo bar
	@echo FOO DIRECTORY:
	@ls -laR foo
	@echo
	@echo BAR DIRECTORY:
	@ls -laR bar
