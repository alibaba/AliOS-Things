all ::
	$(CC) $(CFLAGS) -o ext.http2 http2-example.c $(LDFLAGS)

clean ::
	rm -vf ext.http2

