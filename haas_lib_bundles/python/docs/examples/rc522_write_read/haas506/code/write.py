import mfrc522
import utime as time

def do_write():
	rdr = mfrc522.MFRC522()
	try:
		while True:
			(stat, tag_type) = rdr.request(rdr.REQIDL)
			if stat == rdr.OK:
				(stat, raw_uid) = rdr.anticoll()
				if stat == rdr.OK:
					print("New card detected")
					print("  - tag type: 0x%02x" % tag_type)
					print("  - uid	 : 0x%02x%02x%02x%02x" % (raw_uid[0], raw_uid[1], raw_uid[2], raw_uid[3]))
					print("")
					if rdr.select_tag(raw_uid) == rdr.OK:
						key = [0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF]
						if rdr.auth(rdr.AUTHENT1A, 1, key, raw_uid) == rdr.OK:
							#往指定的块中 写数据，数据长度最大为16个字节
							#当前 是往 0x01即扇区0块1写数据
							stat = rdr.write(1, b"\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\xff")
							rdr.stop_crypto1()
							if stat == rdr.OK:
								print("Data written to card")
								rdr.deinit()
								break
							else:
								print("Failed to write data to card")
						else:
							print("Authentication error")
					else:
						print("Failed to select tag")
			#set period of writing			
			time.sleep(1)			

	except KeyboardInterrupt:
		print("Bye")
