#ifdef PROTO3
#include "t/test-proto3.pb-c.h"
#else
#include "t/test.pb-c.h"
#endif
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int main(void)
{
  Foo__Person__PhoneNumber__Comment comment = FOO__PERSON__PHONE_NUMBER__COMMENT__INIT;
  Foo__Person__PhoneNumber phone = FOO__PERSON__PHONE_NUMBER__INIT;
  Foo__Person__PhoneNumber *phone_numbers[1];
  Foo__Person person = FOO__PERSON__INIT;
  Foo__Person *person2;
  unsigned char simple_pad[8];
  size_t size, size2;
  unsigned char *packed;
  ProtobufCBufferSimple bs = PROTOBUF_C_BUFFER_SIMPLE_INIT (simple_pad);

  comment.comment = "protobuf-c guy";

  phone.number = "1234";
#ifndef PROTO3
  phone.has_type = 1;
#endif
  phone.type = FOO__PERSON__PHONE_TYPE__WORK;
  phone.comment = &comment;

  phone_numbers[0] = &phone;

  person.name = "dave b";
  person.id = 42;
  person.n_phone = 1;
  person.phone = phone_numbers;

  size = foo__person__get_packed_size (&person);
  packed = malloc (size);
  assert (packed);

  size2 = foo__person__pack (&person, packed);

  assert (size == size2);
  foo__person__pack_to_buffer (&person, &bs.base);
  assert (bs.len == size);
  assert (memcmp (bs.data, packed, size) == 0);

  PROTOBUF_C_BUFFER_SIMPLE_CLEAR (&bs);

  person2 = foo__person__unpack (NULL, size, packed);
  assert (person2 != NULL);
  assert (person2->id == 42);
#ifndef PROTO3
  assert (person2->email == NULL);
#else
  assert (strcmp (person2->email, "") == 0);
#endif
  assert (strcmp (person2->name, "dave b") == 0);
  assert (person2->n_phone == 1);
  assert (strcmp (person2->phone[0]->number, "1234") == 0);
  assert (person2->phone[0]->type == FOO__PERSON__PHONE_TYPE__WORK);
  assert (strcmp (person2->phone[0]->comment->comment, "protobuf-c guy") == 0);

  foo__person__free_unpacked (person2, NULL);
  free (packed);

  printf ("test succeeded.\n");

  return 0;
}
