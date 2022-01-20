umqtt.robust
============

umqtt is a simple MQTT client for MicroPython. (Note that it uses some
MicroPython shortcuts and doesn't work with CPython). It consists of
two submodules: umqtt.simple and umqtt.robust. umqtt.robust is built
on top of umqtt.simple and adds auto-reconnect facilities for some of
networking errors.

What does it mean to be "robust" ?
----------------------------------

Modern computing systems are sufficiently complex and have multiple
points of failure. Consider for example that nothing will work if
there's no power (mains outage or battery ran out). As you may imagine,
umqtt.robust won't help you with your flat battery. Most computing
systems are now networked, and communication is another weak link.
This is especially true for wireless communications. If two of your
systems can't connect reliably communicate via WiFi, umqtt.robust
can't magically resolve that (but it may help with intermittent
WiFi issues).

What umqtt.robust tries to do is very simple - if while trying to
perform some operation, it detects that connection to MQTT breaks,
it tries to reconnect to it. That's good direction towards "robustness",
but the problem that there is no single definition of what "robust"
is. Let's consider following usecase:

1. A temperature reading gets transmitted once a minute. Then the
best option in case of a transmission error might be not doing
anything at all - in a minute, another reading will be transmitted,
and for slowly-changing parameter like a temperature, a one-minute
lost reading is not a problem. Actually, if the sending device is
battery-powered, any connection retries will just drain battery and
make device "less robust" (it will run out of juice sooner and more
unexpectedly, which may be a criteria for "robustness").

2. If there's a button, which communicates its press event, then
perhaps it's really worth to retry to deliver this event (a user
expects something to happen when they press the button, right?).
But if a button is battery-power, unconstrained retries won't do
much good still. Consider mains power outage for several hours,
MQTT server down all this time, and battery-powered button trying
to re-publish event every second. It will likely drain battery
during this time, which is very non-robust. Perhaps, if a press
isn't delivered in 15 seconds, it's no longer relevant (depending
on what press does, the above may be good for a button turning
on lights, but not for something else!)

3. Finally, let's consider security sensors, like a window broken
sensor. That's the hardest case. Apparently, those events are
important enough to be delivered no matter what. But if done with
short, dumb retries, it will only lead to quick battery drain. So,
a robust device would retry, but in smart manner, to let battery
run for as long as possible, to maximize the chance of the message
being delivered.

Let's sum it up:

a) There's no single definition of what "robust" is. It depends on
   a particular application.
b) Robustness is a complex measure, it doesn't depend on one single
   feature, but rather many different features working together.
   Consider for example that to make button from the case 2 above
   work better, it would help to add a visual feedback, so a user
   knew what happens.

As you may imagine, umqtt.robust doesn't, and can't, cover all possible
"robustness" scenarios, nor it alone can make your MQTT application
"robust". Rather, it's a barebones example of how to reconnect to an
MQTT server in case of a connection error. As such, it's just one
of many steps required to make your app robust, and majority of those
steps lie on *your application* side. With that in mind, any realistic
application would subclass umqtt.robust.MQTTClient class and override
delay() and reconnect() methods to suit particular usage scenario. It
may even happen that umqtt.robust won't even suit your needs, and you
will need to implement your "robust" handling from scratch.


Persistent and non-persistent MQTT servers
------------------------------------------

Consider an example: you subscribed to some MQTT topics, then connection
went down. If we talk "robust", then once you reconnect, you want any
messages which arrived when the connection was down, to be still delivered
to you. That requires retainment and persistency enabled on MQTT server.
As umqtt.robust tries to achieve as much "robustness" as possible, it
makes a requirement that the MQTT server it communicates to has persistency
enabled. This include persistent sessions, meaning that any client
subscriptions are retained across disconnect, and if you subscribed once,
you no longer need to resubscribe again on next connection(s). This makes
it more robust, minimizing amount of traffic to transfer on each connection
(the more you transfer, the higher probability of error), and also saves
battery power.

However, not all broker offer true, persistent MQTT support:

* If you use self-hosted broker, you may need to configure it for
  persistency. E.g., a popular open-source broker Mosquitto requires
  following line::

    persistence true

  to be added to ``mosquitto.conf``. Please consult documentation of
  your broker.

* Many so-called "cloud providers" offer very limited subset of MQTT for
  their free/inexpensive tiers. Persistence and QoS are features usually
  not supported. It's hard to achieve any true robustness with these
  demo-like offerings, and umqtt.robust isn't designed to work with them.
