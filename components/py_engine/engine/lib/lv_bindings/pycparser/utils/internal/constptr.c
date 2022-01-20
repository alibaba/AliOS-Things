void foo(char * const * arg) {
  arg += 1;
  (*arg) += 1;
}

void foo2(char ** const arg) {
  arg += 1;
  (*arg) += 1;
}
