all: client-ka

client-ka: client-ka.c http.c
	gcc -g client-ka.c http.c -o client-ka

.PHONY: run-ka
run-ka:
	./client-ka httpbin.org 80 /

.PHONY: run-ka3
run-ka3:
	./client-ka httpbin.org 80 / /json /html