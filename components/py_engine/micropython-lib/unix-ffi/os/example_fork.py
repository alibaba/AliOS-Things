import os

r, w = os.pipe()
print(r, w)

pid = os.fork()
print("hello", pid)

if not pid:
    print("pid:", os.getpid())
    os.close(r)
    os.write(w, "from child")
    os._exit(0)
else:
    os.close(w)
    print(os.read(r, 100))
    print(os.waitpid(pid, 0))
