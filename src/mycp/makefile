all: mycp
	@./mycp foo bar

mycp: mycp.c
	@gcc mycp.c -o mycp

show: foo bar
	@echo FOO DIRECTORY:
	@ls -laR foo
	@echo
	@echo BAR DIRECTORY:
	@ls -laR bar

clean:
	@rm -rf mycp bar
