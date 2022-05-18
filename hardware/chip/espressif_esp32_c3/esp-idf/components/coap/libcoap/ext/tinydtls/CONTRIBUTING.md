Contributing to tinydtls
========================

Thanks for your interest in this project.

Project description:
--------------------

tinydtls is a library for Datagram Transport Layer Security (DTLS) covering both the client and the server state machine. It is implemented in C and provides support for the mandatory cipher suites specified in CoAP.

- https://projects.eclipse.org/projects/iot.tinydtls

Developer resources:
--------------------

Information regarding source code management, builds, coding standards, and more.

- https://projects.eclipse.org/projects/iot.tinydtls/developer

Contributor License Agreement:
------------------------------

Before your contribution can be accepted by the project, you need to create and electronically sign the Eclipse Foundation Contributor License Agreement (CLA).

- http://www.eclipse.org/legal/CLA.php

Contact:
--------

Contact the project developers via the project's "dev" list.

- https://dev.eclipse.org/mailman/listinfo/tinydtls-dev

Search for bugs:
----------------

This project uses Bugzilla to track ongoing development and issues.

- https://bugs.eclipse.org/bugs/buglist.cgi?product=tinydtls

Create a new bug:
-----------------

Be sure to search for existing bugs before you create another one. Remember 
that contributions are always welcome!

- https://bugs.eclipse.org/bugs/enter_bug.cgi?product=tinydtls

Submit Patches via Gerrit:
--------------------------

Patches must follow to the tinydtls coding style and must be submitted
to [gerrit](https://git.eclipse.org/r/p/tinydtls/org.eclipse.tinydtls)
for review. To submit a patch to gerrit, the author needs to have a
CLA on file and must have a Signed-off-by entry with the same email
address in the commit message footer.

- https://www.eclipse.org/projects/handbook/#resources-source 

Every new file must contain the Eclipse license information and the
copyright holder(s). Please take a look into existing files and adopt
the needed changes to your new file(s).

Tinydtls Coding style:
----------------------

* For better reading the indentation is set to 2 characters as spaces,
  this is depended on the often used nested functions like
  'if-else'. Don't use TABs any there! Avoid trailing white spaces at
  the end of a line.

* Single lines within the source code should not be longer then 78
  characters.

* In the implementation (i.e., in files ending with '.c'), function
  identifiers start on the first column of a line. The function's
  return type preceeds the function identifier on a line of its
  own. For example, in `dtls.c` the following definition is found:

```
dtls_peer_t *
dtls_get_peer(const dtls_context_t *ctx, const session_t *session) {
...
}
```

* Declarations in header files do not follow the previous rule. For
  example, the declaration for `dtls_get_peer()` in `dtls.h` reads as
  follows:

```
dtls_peer_t *dtls_get_peer(const dtls_context_t *context,
			   const session_t *session);
```

* A useful source code documentation is mandatory. Mostly to be done
  within the source code files.

* Please set up/adjust the doxygen documentation if you create new
  functions or change existing functions. The doxygen documentation
  has to be done in the header files as they are the public part of
  tinydtls and only use the @-syntax for doxygen commands (akin to
  javadoc).

* Never break the API!
  Do not remove old functions unless absolutely necessary. If changes
  are needed in some kind always provide a wrapper for the old call to
  let the library be backward compatible and mark the old function as
  @deprecated in the doxygen comment.  Please discuss needed changes
  on the mailing list.

