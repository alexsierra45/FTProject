port ?= 5000
root ?= $(HOME)

.PHONY: dev
dev:
	gcc -o main *.c -lpthread -lcurl && ./main $(port) $(root)

.PHONY: build
build:
	gcc -o main *.c -lpthread -lcurl