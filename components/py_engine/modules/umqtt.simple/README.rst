umqtt.simple
============

umqtt is a simple MQTT client for MicroPython. (Note that it uses some
MicroPython shortcuts and doesn't work with CPython).

Design requirements
-------------------

* Memory efficiency.
* Avoid infamous design anti-patterns like "callback hell".
* Support for both publishing and subscription via a single client
  object (another alternative would be to have separate client classes
  for publishing and subscription).

API design
----------

Based on the requirements above, there are following API traits:

* All data related to MQTT messages is encoded as bytes. This includes
  both message content AND topic names (even though MQTT spec states
  that topic name is UTF-8 encoded). The reason for this is simple:
  what is received over network socket is binary data (bytes) and
  it would require extra step to convert that to a string, spending
  memory on that. Note that this applies only to topic names (because
  they can be both sent and received). Other parameters specified by
  MQTT as UTF-8 encoded (e.g. ClientID) are accepted as strings.
* Subscribed messages are delivered via a callback. This is to avoid
  using a queue for subscribed messages, as otherwise they may be
  received at any time (including when client expects other type
  of server response, so there're 2 choices: either deliver them
  immediately via a callback or queue up until an "expected" response
  arrives). Note that lack of need for a queue is delusive: the
  runtime call stack forms an implicit queue in this case. And unlike
  explicit queue, it's much harder to control. This design was chosen
  because in a common case of processing subscribed messages it's
  the most efficient. However, if in subscription callback, new
  messages of QoS>0 are published, this may lead to deep, or
  infinite recursion (the latter means an application will terminate
  with ``RuntimeException``).

API reference
-------------

Taking into account API traits described above, umqtt pretty closely
follows MQTT control operations, and maps them to class methods:

* ``connect(...)`` - Connect to a server. Returns True if this connection
  uses persisten session stored on a server (this will be always False if
  clean_session=True argument is used (default)).
* ``disconnect()`` - Disconnect from a server, release resources.
* ``ping()`` - Ping server (response is processed automatically by wait_msg()).
* ``publish()`` - Publish a message.
* ``subscribe()`` - Subscribe to a topic.
* ``set_callback()`` - Set callback for received subscription messages.
* ``set_last_will()`` - Set MQTT "last will" message. Should be called
  *before* connect().
* ``wait_msg()`` - Wait for a server message. A subscription message will be
  delivered to a callback set with set_callback(), any other messages
  will be processed internally.
* ``check_msg()`` - Check if there's pending message from server. If yes,
  process the same way as wait_msg(), if not, return immediately.

``wait_msg()`` and ``check_msg()`` are "main loop iteration" methods, blocking
and non-blocking version. They should be called periodically in a loop,
``wait_msg()`` if you don't have any other foreground tasks to perform
(i.e. your app just reacts to subscribed MQTT messages), ``check_msg()``
if you process other foreground tasks too.

Note that you don't need to call ``wait_msg()``/``check_msg()`` if you only
publish messages, never subscribe to them.

For more detailed information about API please see the source code
(which is quite short and easy to review) and provided examples.


Supported MQTT features
-----------------------

QoS 0 and 1 are supported for both publish and subscribe. QoS2 isn't
supported to keep code size small. Besides ClientID, only "clean
session" parameter is supported for connect as of now.


MQTT client with automatic reconnect
------------------------------------

There's a separate `umqtt.robust` module which builds on `umqtt.simple`
and adds automatic reconnect support in case of network errors.
Please see its documentation for further details.
