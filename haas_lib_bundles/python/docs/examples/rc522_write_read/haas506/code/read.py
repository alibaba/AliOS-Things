import mfrc522
import utime as time


def do_read(rdr):
	print("enter do_read()")
	try:
		(stat, tag_type) = rdr.request(rdr.REQIDL)
		if stat == rdr.OK:
			(stat, raw_uid) = rdr.anticoll()
			if stat == rdr.OK:
				print("New card detected")
				print("  - tag type: 0x%02x" % tag_type)
				print("  - uid	 : 0x%02x%02x%02x%02x" % (raw_uid[0], raw_uid[1], raw_uid[2], raw_uid[3]))
				print("")
				if rdr.select_tag(raw_uid) == rdr.OK:
					#默认密钥
					key = [0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF]
					#自定义密钥
					# key = [0x61, 0x73, 0x64, 0x38, 0x38, 0x38]
					if rdr.auth(rdr.AUTHENT1A, 1, key, raw_uid) == rdr.OK:         # 验证密钥
						# 例如扇区0：块0、块1、块2、块3 
						# 0x00就是扇区0块0数据的地址
						# 0x01是扇区0的块1数据的地址
						# 0x02 就是扇区0块2数据的地址
						# 每增加一个块  地址就加1，有很多个扇区
						# 每个扇区的 块3是校验密钥的，即存放密码的
						# 当前读取的是 0x01，即扇区0块1的数据
						print("data: %s" % rdr.read(1))  
						rdr.stop_crypto1()
						return True,raw_uid
					else:
						print("Authentication error")
						return False,False
				else:
					print("Failed to select tag")
					return False,False
			else:
				return False,None
		else:
			return False,None

		#set period of reading		
		# time.sleep(1)			

	except KeyboardInterrupt:
		print("Bye")
		return False,False