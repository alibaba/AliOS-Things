ping application
================

Utility to send an ICMP echo request to an Ethernet host.


Description
-----------

This application lets you send ICMP ping requests to any IP or domain name on the network and
time the reply. It also demonstrates how to resolved domain names into IP addresses using lwIP.
The ping source code is reusable to send and receive pings to any address.

When the application runs, it first initializes the system and starts up networking. By default,
an IP address is requested from a DHCP server (though this can be changed, see the Build options
section below). Once an IP address has been obtained, the user is asked to enter the IP address
or domain name of the ping target. After the domain name is resolved, pings will be sent every
1 second until the system is powered off. The sequence number in the ICMP packet starts at 1 and
is incremented for each ping request. For every reply that is received, the sequence number and
elapsed time since the request was sent are printed to the console.

The Ethernet MAC address is currently fixed to 00:04:9f:00:00:01, though this can be change by
editing the source. Future releases will read the MAC address from OTP.


Requirements
------------

The board must be connected to an Ethernet network. With the default configuration, a DHCP server
is required.


Build options
-------------

The device's IP address can be adjusted by editing init_lwip() in ping_main.c. By default, an
IP address is obtained from a DHCP server. Alternatively, Auto IP may be used to obtain a 169.254
address, or a simple static IP address can be set.

The Ethernet MAC address can be adjusted by editing the kMACAddress array in httpd_main.c.


Code organization
-----------------

The ping code is based on the example ping source code included in the contrib/apps/ping directory
of lwIP. The ping.c and ping.h files have been copied into the src directory. They have been
extended to allow for easily sending ping requests to arbitrary addresses specified at runtime, and
to support installation of an application callback that will be invoked when the ping reply is
received or the ping times out.

The ping_main.c source file is responsible for initializing the system, starting networking, and
running the TCP/IP stack. It also interfaces with the user via the debug console.
 

