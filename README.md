# HTTP_Client_using_C

Here we are trying to build an operational HTTP Client implementing the 'Connection: Keep-Alive' protocol. Therefore, the header requires the client to open a socket once, such that it can issue multiple requests over the same socket. Much of this takes place in the client-ka.c file, but we have other files with helper functions such as http.c and header files like http.h with user-defined variables and exported functions



The code can be compiled and runned using the Makefile which can executed by the command 'make'.

