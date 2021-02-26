try:
    import utime as time
except ImportError:
    import time
import _thread


def foo():
    pass


def thread_entry(thread_name,n):
    for i in range(n):
        print(" %s:%d" %(thread_name,i))
        foo()
        time.sleep_ms(10)

print('this is module thread_test')
try:
    _thread.start_new_thread(thread_entry, ('thread1',10,))
    _thread.start_new_thread(thread_entry, ('thread2',20,))
except:
    print("unable to start thread")
print('module thread_test end ')
