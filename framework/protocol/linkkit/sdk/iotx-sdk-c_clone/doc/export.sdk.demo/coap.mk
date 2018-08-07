all ::
	$(CC) $(CFLAGS) -o ext.coap coap-example.c $(LDFLAGS)

clean ::
	rm -vf ext.coap

