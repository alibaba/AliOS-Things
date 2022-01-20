from multiprocessing import Process


def f(name):
    assert name == "bob"


if __name__ == "__main__":
    p = Process(target=f, args=("bob",))
    p.start()
    p.join()
