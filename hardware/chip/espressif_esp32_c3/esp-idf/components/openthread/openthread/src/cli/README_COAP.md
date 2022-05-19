# OpenThread CLI - CoAP Example

The OpenThread CoAP APIs may be invoked via the OpenThread CLI.

## Quick Start

### Form Network

Form a network with at least two devices.

### Node 1

On node 1, setup CoAP server with resource `test-resource`.

```bash
> coap start
Done
> coap resource test-resource
Done
```

### Node 2

```bash
> coap start
Done
> coap get fdde:ad00:beef:0:d395:daee:a75:3964 test-resource
Done
coap response from [fdde:ad00:beef:0:2780:9423:166c:1aac] with payload: 30
> coap put fdde:ad00:beef:0:2780:9423:166c:1aac test-resource con payload
Done
coap response from [fdde:ad00:beef:0:2780:9423:166c:1aac]
```

### Result

On node 1, you should see output similar to below:

```bash
coap request from [fdde:ad00:beef:0:b3:e3f6:2dcc:4b79] GET
coap response sent
coap request from [fdde:ad00:beef:0:b3:e3f6:2dcc:4b79] PUT with payload: 7061796c6f6164
coap response sent
```

## Command List

- [help](#help)
- [cancel](#cancel)
- [delete](#delete-address-uri-path-type-payload)
- [get](#get-address-uri-path-type)
- [observe](#observe-address-uri-path-type)
- [parameters](#parameters)
- [post](#post-address-uri-path-type-payload)
- [put](#put-address-uri-path-type-payload)
- [resource](#resource-uri-path)
- [set](#set-new-content)
- [start](#start)
- [stop](#stop)

## Command Details

### help

```bash
> coap help
help
cancel
delete
get
observe
parameters
post
put
resource
set
start
stop
Done
```

List the CoAP CLI commands.

### cancel

Request the cancellation of an existing observation subscription to a remote resource.

```bash
> coap cancel
Done
```

### delete \<address\> \<uri-path\> \[type\] \[payload\]

- address: IPv6 address of the CoAP server.
- uri-path: URI path of the resource.
- type: "con" for Confirmable or "non-con" for Non-confirmable (default).
- payload: CoAP request payload.

```bash
> coap delete fdde:ad00:beef:0:2780:9423:166c:1aac test-resource con payload
Done
```

### get \<address\> \<uri-path\> \[type\]

- address: IPv6 address of the CoAP server.
- uri-path: URI path of the resource.
- type: "con" for Confirmable or "non-con" for Non-confirmable (default). Use "block-<block-size>" if the response should be transferred block-wise. ("block-16","block-32","block-64","block-128","block-256","block-512","block-1024")

```bash
> coap get fdde:ad00:beef:0:2780:9423:166c:1aac test-resource
Done
```

```bash
> coap get fdde:ad00:beef:0:2780:9423:166c:1aac test-resource block-1024
Done
```

### observe \<address\> \<uri-path\> \[type\]

This is the same a `get`, but the `Observe` parameter will be sent, set to 0 triggering a subscription request.

- address: IPv6 address of the CoAP server.
- uri-path: URI path of the resource.
- type: "con" for Confirmable or "non-con" for Non-confirmable (default).

```bash
> coap observe fdde:ad00:beef:0:2780:9423:166c:1aac test-resource
Done
```

### parameters \<type\> \["default"|<ack_timeout\> <ack_random_factor_numerator\> <ack_random_factor_denominator\> <max_retransmit\>\]

Sets transmission parameters for the following interactions.

- type: "request" for CoAP requests and "response" for CoAP responses.

If no more parameters are given, the command prints the current configuration:

```bash
> coap parameters request
Transmission parameters for request:
ACK_TIMEOUT=1000 ms, ACK_RANDOM_FACTOR=255/254, MAX_RETRANSMIT=2
Done
```

If `"default"` is given, the command sets the default configuration for the transmission parameters.

```bash
> coap parameters request default
Transmission parameters for request:
default
Done
```

Also, you can specify the transmission parameters in the command line:

- ack_timeout (0~UINT32_MAX): RFC7252 ACK_TIMEOUT, in milliseconds.
- ack_random_factor_numerator, ack_random_factor_denominator (0~255): RFC7252 ACK_RANDOM_FACTOR=ack_random_factor_numerator/ack_random_factor_denominator.
- max_retransmit (0~255): RFC7252 MAX_RETRANSMIT.

```bash
> coap parameters request 1000 255 254 2
Transmission parameters for request:
ACK_TIMEOUT=1000 ms, ACK_RANDOM_FACTOR=255/254, MAX_RETRANSMIT=2
Done
```

### post \<address\> \<uri-path\> \[type\] \[payload\]

- address: IPv6 address of the CoAP server.
- uri-path: URI path of the resource.
- type: "con" for Confirmable or "non-con" for Non-confirmable (default). Use "block-<block-size>" to send blocks with random payload. ("block-16","block-32","block-64","block-128","block-256","block-512","block-1024")
- payload: CoAP request payload. If \[type\] is "block-<block-size>", the amount of blocks to be sent can be set here.

```bash
> coap post fdde:ad00:beef:0:2780:9423:166c:1aac test-resource con payload
Done
```

```bash
> coap post fdde:ad00:beef:0:2780:9423:166c:1aac test-resource block-1024 10
Done
```

### put \<address\> \<uri-path\> \[type\] \[payload\]

- address: IPv6 address of the CoAP server.
- uri-path: URI path of the resource.
- type: "con" for Confirmable or "non-con" for Non-confirmable (default). Use "block-<block-size>" to send blocks with random payload. ("block-16","block-32","block-64","block-128","block-256","block-512","block-1024")
- payload: CoAP request payload. If \[type\] is "block-<block-size>", the amount of blocks to be sent can be set here.

```bash
> coap put fdde:ad00:beef:0:2780:9423:166c:1aac test-resource con payload
Done
```

```bash
> coap put fdde:ad00:beef:0:2780:9423:166c:1aac test-resource block-1024 10
Done
```

### resource \[uri-path\]

Sets the URI path for the test resource.

```bash
> coap resource test-resource
Done
> coap resource
test-resource
Done
```

### set \[new-content\]

Sets the content sent by the test resource. If a CoAP client is observing the resource, a notification is sent to that client.

```bash
> coap set Testing123
Done
```

### start

Starts the application coap service.

```bash
> coap start
Done
```

### stop

Stops the application coap service.

```bash
> coap stop
Done
```
