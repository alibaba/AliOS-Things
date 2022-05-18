# OpenThread CLI - SRP (Service Registration Protocol)

## Quick Start

### Start SRP Server

Start the SRP Server node:

```bash
./output/simulation/bin/ot-cli-ftd 1
```

Setup a Thread network and start the SRP Server:

```bash
> dataset init new
Done
> dataset
Active Timestamp: 1
Channel: 22
Channel Mask: 0x07fff800
Ext PAN ID: 8d6ed7a05a28fb3b
Mesh Local Prefix: fded:5114:8263:1fe1::/64
Network Key: 7fcbae4153cc2955c28440c15d4d4219
Network Name: OpenThread-f7af
PAN ID: 0xf7af
PSKc: b658e40f174e3a11be149b302ef07a0f
Security Policy: 672, onrcb
Done
> dataset commit active
Done
> ifconfig up
Done
> thread start
Done
> state
leader
Done
>ipaddr
fded:5114:8263:1fe1:0:ff:fe00:fc00
fded:5114:8263:1fe1:0:ff:fe00:c000
fded:5114:8263:1fe1:68bc:ec03:c1ad:9325
fe80:0:0:0:a8cd:6e23:df3d:4193
Done
> srp server enable
Done
```

### Start SRP Client

Start the SRP Client node:

```bash
./output/simulation/bin/ot-cli-ftd 2
```

Join the Thread Network and register a `_ipps._tcp` service:

```bash
> dataset networkkey 7fcbae4153cc2955c28440c15d4d4219
Done
> dataset commit active
Done
> ifconfig up
Done
> thread start
Done
> state
child
Done
> ipaddr
fded:5114:8263:1fe1:0:ff:fe00:c001
fded:5114:8263:1fe1:44f9:cc06:4a2d:534
fe80:0:0:0:38dd:fdf7:5fd:24e
Done
> srp client host name my-host
Done
> srp client host address fded:5114:8263:1fe1:44f9:cc06:4a2d:534
Done
> srp client service add my-service _ipps._tcp 12345
Done
> srp client autostart enable
Done
```

The last command enables the auto-start mode on the client which then monitors the network data to discover available SRP servers within the Thread network and automatically starts the client.

Alternatively, the client can be started manually using the `srp client start`.

The SRP Server listening UDP port (which is `c002`(`49154`) in the example below) can be found from the Server Data (listed by the `netdata show` command).

Make sure the SRP Server address & port are used for the `srp client start` command.

```bash
> netdata show
Prefixes:
Routes:
Services:
44970 5d c002 s 8400
Done
srp client start fded:5114:8263:1fe1:68bc:ec03:c1ad:9325 49154
Done
```

### Verify the service status

Check if the host and service has been successfully registered on the client node:

```bash
> srp client host
name:"my-host", state:Registered, addrs:[fded:5114:8263:1fe1:44f9:cc06:4a2d:534]
Done
> srp client service
instance:"my-service", name:"_ipps._tcp", state:Registered, port:12345, priority:0, weight:0
Done
```

Make sure it shows `state:Registered` for both host and service commands.

Check the host & service on the server node:

```bash
> srp server host
my-host.default.service.arpa.
    deleted: false
    addresses: [fded:5114:8263:1fe1:44f9:cc06:4a2d:534]
Done
> srp server service
my-service._ipps._tcp.default.service.arpa.
    deleted: false
    port: 12345
    priority: 0
    weight: 0
    TXT: 00
    host: my-host.default.service.arpa.
    addresses: [fded:5114:8263:1fe1:44f9:cc06:4a2d:534]
Done
```

Make sure it shows `deleted: false` for both host and service commands.

### Remove the service

A service can be removed through the client node:

```bash
> srp client service remove my-service _ipps._tcp
Done
```

Confirm on the server node that the service has been removed:

```bash
> srp server service
my-service._ipps._tcp.default.service.arpa.
    deleted: true
Done
```

The service entry is listed because the name of service is not removed.

### Remove the host and service names

A host and service, along with their names, can be removed through the client node:

```bash
> srp client host remove 1
Done
```

Confirm on the server node that no host or service entries are listed:

```bash
> srp server host
Done
> srp server service
Done
>
```

## CLI Reference

- [SRP Client CLI Reference](README_SRP_CLIENT.md)
- [SRP Server CLI Reference](README_SRP_SERVER.md)
