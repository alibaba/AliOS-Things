This directory contains an example of how a project using lwIP might
look. It is also the development platform of lwIP, since it can be run
as a user process under FreeBSD or Linux. There are also a number of
example applications (including a simple web server) in the apps/
directory.

Some short instructions on how to build and run lwIP on a FreeBSD or
Linux host. For FreeBSD, the tap interface must be enabled in the
kernel configuration and the kernel must be recompiled. The tap
interface is enabled by adding the line "pseudo-device tap" in the
kernel configuration. See Chapter 9 in the FreeBSD handbook for
instructions on how to build a custom FreeBSD kernel.

For Linux you might need to create a device node with

  > mknod /dev/net/tun c 10 200

* Compile the code. This must be done by using GNU Make. Under
  FreeBSD, GNU Make can be found in the ports collection under
  /usr/ports/devel/gmake (type "make install distclean" to
  install). Under Linux, GNU Make is the default "make".

  > gmake         (FreeBSD)

  > make          (Linux)

* The compilation process produces the executable file "simhost". To
  run this, you have to be root.

  > su            (Type password for the root account)
  # ./simhost

* The lwIP TCP/IP stack is now running with IP address
  192.168.0.2. Some things that you can try:

  To see the packets that are going to and from the lwIP stack, run
  tcpdump:

  # tcpdump -l -n -i tap0

  You can ping lwIP:

  > ping 192.168.0.2

  For a telnet shell, run:

  > telnet 192.168.0.2
  
  Finally, "simhost" also includes a simple web server; the URL is
  of course http://192.168.0.2/.

* Simhost has some extra features that can be
  selected with command line options.
  
  To enable runtime debug output (project must be build with -DLWIP_DEBUG):
  
  # ./simhost -d
  
  To ping any host, e.g. the gateway:
  
  # ./simhost -p 192.168.0.1
