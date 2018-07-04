all ::
	$(CC) $(CFLAGS) -o ext.mqtt mqtt-example.c $(LDFLAGS)

clean ::
	rm -vf ext.mqtt

