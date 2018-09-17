httpd application
=================

Simple web server application using the lwIP TCP/IP stack.


Description
-----------

This application runs a simple HTTP server. It uses the example server that comes with lwIP in the
contrib directory. The fixed file system is compiled directly into the code and presents a single
index page and image.

After the application begins running, it initializes the lwIP stack and attempts to get an IP
address from a DHCP server. See the Build options section below for how to use other methods of
getting an IP address. Once the IP address is obtained, a message similar to the following is
printed on the debug console:

    netif: up (ip=10.81.4.214)

At this point, the web server is ready to receive requests. Point a web brower on your host PC to
"http://<ip-address>/" to view the index page.

The included file system contains these files:
    404.html
    img/sics.gif
    index.html

The Ethernet MAC address is currently fixed to 00:04:9f:00:00:01, though this can be change by
editing the source. Future releases will read the MAC address from OTP.

Requirements
------------

The board must be connected to an Ethernet network. With the default configuration, a DHCP server
is required.


Build options
-------------

The device's IP address can be adjusted by editing init_lwip() in httpd_main.c. By default, an
IP address is obtained from a DHCP server. Alternatively, Auto IP may be used to obtain a 169.254
address, or a simple static IP address can be set.

The Ethernet MAC address can be adjusted by editing the kMACAddress array in httpd_main.c.


Code organization
-----------------

Both the httpd server code and compiled-in filesystem reside in the lwip/contrib/apps/httpserver_raw
directory and are used unmodified. The httpd_main.c file in the src directory is responsible for
initializing the system, starting networking, and running the TCP/IP stack.

You can use the makefsdata tool in lwip/contrib/apps/httpserver_raw/makefsdata to generate a new
filesystem to compile into the application. See the readme.txt in that directory for more details
about the tool.

