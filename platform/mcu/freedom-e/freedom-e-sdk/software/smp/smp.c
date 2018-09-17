#include <stdio.h>
#include <unistd.h>
#include "atomic.h"
void write_hex(int, unsigned long);

atomic_t tty_lock = ATOMIC_INIT(0);

void get_lock(atomic_t *lock)
{
  while (atomic_xchg(lock, 1) == 1);
  mb();
}

void put_lock(atomic_t *lock)
{
  mb();
  atomic_xchg(lock, 0);
}

int secondary_main(int hartid)
{
  volatile int counter;

  while (1) {
    get_lock(&tty_lock);
    write(1, "hello world from hart ", 22);
    char s[] = {'0', '\n', '\0'};
    s[0] += hartid;
    write(1, s, 2);
    put_lock(&tty_lock);

    for (counter = 0; counter < 10000 + 100*hartid; ++counter)
        mb();
  }
}

int main()
{
  return secondary_main(0);
}
