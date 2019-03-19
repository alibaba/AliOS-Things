

all: udp_client udp_server

udp_client_obj = udp_client.o
udp_server_obj = udp_server.o

udp_client:$(udp_client_obj)
	gcc $^ -o $@

udp_server:$(udp_server_obj)
	gcc $^ -o $@

%.o:%.c
	gcc -c $< -o $@

.PHONY: clean

clean:
	rm -rf $(udp_client_obj) $(udp_server_obj) udp_client udp_server
