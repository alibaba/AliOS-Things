import pwd
import os

user = os.getenv("USER")
passwd = pwd.getpwnam(user)
assert passwd
assert isinstance(passwd, pwd.struct_passwd)
