all ::
	$(CC) $(CFLAGS) -o ext.http http-example.c $(LDFLAGS)

clean ::
	rm -vf ext.http

