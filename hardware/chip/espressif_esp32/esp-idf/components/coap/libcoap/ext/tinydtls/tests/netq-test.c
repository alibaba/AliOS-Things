#include <string.h>
#include <stdio.h>

#include "utlist.h" 
#include "netq.h" 

void
dump_queue(struct netq_t *queue) {
  struct netq_t *p;
  int i = 0;

  if (!queue) {
    printf("(null)\n");
  } else {
    LL_FOREACH(queue, p) {
      printf("node #%d, timeout: %d\n", i++, p->t);
    }
  }
}

int main(int argc, char **argv) {
  struct netq_t *nq = NULL, *node;
  int i;
    
  clock_time_t timestamps[] = { 300, 100, 200, 400, 500 };

  for (i = 0; i < sizeof(timestamps)/sizeof(clock_time_t); i++) {
    node = netq_node_new(0);

    if (!node) {
      fprintf(stderr, "E: cannot create node #%d\n", i);
      exit(EXIT_FAILURE);
    }

    node->t = timestamps[i];

    if (!netq_insert_node(&nq, node)) {
      fprintf(stderr, "E: cannot add node to nodelist #%d\n", i);
      exit(EXIT_FAILURE);
    }
  }

  printf("------------------------------------------------------------------------\n");
  printf("initial queue:\n");
  dump_queue(nq);

  printf("------------------------------------------------------------------------\n");
  printf("pop first element:\n");
  node = netq_pop_first(&nq);

  assert(node);
  printf("first node's timeout is %d\n", node->t);
  dump_queue(nq);

  netq_node_free(node);
  
  printf("------------------------------------------------------------------------\n");
  printf("queue head:\n");
  node = netq_head(&nq);

  printf("head's timeout is %d\n", node->t);
  dump_queue(nq);

  printf("------------------------------------------------------------------------\n");
  printf("next timeout:\n");

  node = netq_next(node);

  printf("next node's timeout is %d\n", node->t);

  printf("------------------------------------------------------------------------\n");
  printf("remove next:\n");

  netq_remove(&nq, node);
  dump_queue(nq);

  netq_node_free(node);

  printf("------------------------------------------------------------------------\n");
  printf("insert new item (timeout 50):\n");
  node = netq_node_new(0);

  assert(node);
  node->t = 50;

  netq_insert_node(&nq, node);
  dump_queue(nq);

  printf("------------------------------------------------------------------------\n");
  printf("insert new item (timeout 350):\n");
  node = netq_node_new(0);

  assert(node);
  node->t = 350;

  netq_insert_node(&nq, node);
  dump_queue(nq);

  printf("------------------------------------------------------------------------\n");
  printf("insert new item (timeout 1000):\n");
  node = netq_node_new(0);

  assert(node);
  node->t = 1000;

  netq_insert_node(&nq, node);
  dump_queue(nq);

  printf("------------------------------------------------------------------------\n");
  printf("remove all:\n");
  
  netq_delete_all(&nq);
  dump_queue(nq);

  printf("------------------------------------------------------------------------\n");
  printf("pop first element of empty queue:\n");
  node = netq_pop_first(&nq);

  assert(node == NULL);
  dump_queue(nq);

  return 0;
}
