# OpenThread CLI - TCP Example

The OpenThread TCP APIs may be invoked via the OpenThread CLI.

## Quick Start

### Form Network

Form a network with at least two devices.

### Node 1

On node 1, initialize the TCP CLI module and listen for incoming connections using the example TCP listener.

```bash
> tcp init
> tcp listen :: 30000
```

The `::` specifies the IPv6 Unspecified Address.

### Node 2

On node 2, initialize the TCP CLI module, connect to node 1, and send a simple message.

```bash
> tcp init
> tcp connect fe80:0:0:0:a8df:580a:860:ffa4 30000
> tcp send hello
```

### Result

After running the `tcp connect` command on node 1, you should see a printout on node 1 similar to below:

```bash
TCP: Connection established
```

In addition, you should also see a printout on node 2 similar to below:

```bash
Accepted connection from [fe80:0:0:0:8f3:f602:bf9b:52f2]:49152
TCP: Connection established
```

After running the `tcp send` command on node 1, you should see a printout on node 2 similar to below:

```bash
TCP: Received 5 bytes: hello
```

For a more in-depth example, see [this video](https://youtu.be/ppZ784YUKlI).

## Command List

- [help](#help)
- [init](#init-size)
- [deinit](#deinit)
- [bind](#bind-ip-port)
- [connect](#connect-ip-port)
- [send](#send-message)
- [benchmark](#benchmark-size)
- [sendend](#sendend)
- [abort](#abort)
- [listen](#listen-ip-port)
- [stoplistening](#stoplistening)

## Command Details

### abort

Unceremoniously ends the TCP connection, if one exists, associated with the example TCP endpoint, transitioning the TCP endpoint to the closed state.

```bash
> tcp abort
TCP: Connection reset
Done
```

### benchmark [\<size\>]

Transfers the specified number of bytes using the TCP connection currently associated with the example TCP endpoint (this TCP connection must be established before using this command).

- size: the number of bytes to send for the benchmark. If it is left unspecified, the default size is used.

```bash
> tcp benchmark
Done
TCP Benchmark Complete: Transferred 73728 bytes in 7233 milliseconds
TCP Goodput: 81.546 kb/s
```

### bind \<ip\> \<port\>

Associates a name (i.e. IPv6 address and port) to the example TCP endpoint.

- ip: the IPv6 address or the unspecified IPv6 address (`::`).
- port: the TCP port.

```bash
> tcp bind :: 30000
Done
```

### connect \<ip\> \<port\>

Establishes a connection with the specified peer.

If the connection establishment is successful, the resulting TCP connection is associated with the example TCP endpoint.

- ip: the peer's IPv6 address.
- port: the peer's TCP port.

```bash
> tcp connect fe80:0:0:0:a8df:580a:860:ffa4 30000
Done
TCP: Connection established
```

### deinit

Deinitializes the example TCP listener and the example TCP endpoint.

```bash
> tcp deinit
Done
```

### help

List the TCP CLI commands.

```bash
> tcp help
abort
benchmark
bind
connect
deinit
help
init
listen
send-message
sendend
stoplistening
Done
```

### init [\<size\>]

Initializes the example TCP listener and the example TCP endpoint.

- size: the size of the receive buffer to associate with the example TCP endpoint. If left unspecified, the maximum size is used.

```bash
> tcp init
Done
```

### listen \<ip\> \<port\>

Uses the example TCP listener to listen for incoming connections on the specified name (i.e. IPv6 address and port).

If no TCP connection is associated with the example TCP endpoint, then any incoming connections matching the specified name are accepted and associated with the example TCP endpoint.

- ip: the IPv6 address or the unspecified IPv6 address (`::`).
- port: the TCP port.

```bash
> tcp listen :: 30000
Done
```

### send \<message\>

Send data over the TCP connection associated with the example TCP endpoint.

- message: the message to send.

```bash
> tcp send hello
Done
```

### sendend

Sends the "end of stream" signal (i.e., FIN segment) over the TCP connection associated with the example TCP endpoint. This promises the peer that no more data will be sent to it over this TCP connection.

```bash
> tcp sendend
Done
```

### stoplistening

Stops listening for incoming TCP connections using the example TCP listener.

```bash
> tcp stoplistening
Done
```
