port ?= 5000
root ?= $(HOME)

.PHONY: dev
dev:
	gcc -o main main.c -lcurl && ./main $(port) $(root)

.PHONY: build
build:
	gcc -o main main.c -lcurl