import select
import os


ep = select.epoll()
ep.register(0, select.EPOLLIN, (lambda x: x, (0,)))
res = ep.poll(2000)
print(res)
for ev, fd in res:
    print(os.read(fd, 256))
