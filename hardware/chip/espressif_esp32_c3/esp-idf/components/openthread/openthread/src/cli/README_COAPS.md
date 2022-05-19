# OpenThread CLI - CoAPS Example

The OpenThread CoAPS APIs may be invoked via the OpenThread CLI.

## Quick Start

### Form Network

Form a network with at least two devices.

### Configure DTLS ciphersuite.

CoAPS uses DTLS to establish a secure, end-to-end connection.

This example supports two ciphersuites:

- TLS_PSK_WITH_AES_128_CCM_8

  ```bash
  > coaps psk <your-psk> <your-psk-id>
  Done
  ```

- TLS_ECDHE_ECDSA_WITH_AES_128_CCM_8
  ```bash
  > coaps x509
  Done
  ```
  The X.509 certificate stored in `core/cli/x509_cert_key.hpp`.

### Node 1

On node 1, setup CoAPS server with resource `test-resource`.

```bash
> coaps start
Done
> coaps resource test-resource
Done
```

### Node 2

```bash
> coaps start
Done
> coaps connect <peer-ip6-address>
Done
coaps connected
> coaps get test-resource
Done
coaps response from fdde:ad00:beef:0:9903:14b:27e0:5744 with payload: 68656c6c6f576f726c6400
> coaps put test-resource con payload
Done
coaps response from fdde:ad00:beef:0:9903:14b:27e0:5744
```

### Result

On node 1, you should see output similar to below:

```bash
coaps request from fdde:ad00:beef:0:9e68:576f:714c:f395 GET
coaps response sent
coaps request from fdde:ad00:beef:0:9e68:576f:714c:f395 PUT with payload: 7061796c6f6164
coaps response sent
```

## Generate Elliptic Curve Private Key and X.509 Certificate

### EC Private Key

```bash
> openssl ecparam -genkey -name prime256v1 -noout -out ec_private.pem
```

### X.509 Certificate

```bash
> openssl req -x509 -new -key ec_private.pem -out x509_cert.pem -days 30
```

## Command List

- [help](#help)
- [connect](#connect-address)
- [delete](#delete-uri-path-type-payload)
- [disconnect](#disconnect)
- [get](#get-uri-path-type)
- [post](#post-uri-path-type-payload)
- [psk](#psk-psk-pskid)
- [put](#put-uri-path-type-payload)
- [resource](#resource-uri-path)
- [set](#set-new-content)
- [start](#start)
- [stop](#stop)
- [x509](#x509)

## Command Details

### help

```bash
> coaps help
help
connect
delete
disconnect
get
post
psk
put
resource
set
start
stop
x509
Done
```

List the CoAPS CLI commands.

### connect \<address\>

Establish DTLS session.

- address: IPv6 address of the peer.

```bash
> coaps connect fdde:ad00:beef:0:9903:14b:27e0:5744
Done
coaps connected
```

### delete \<uri-path\> \[type\] \[payload\]

- uri-path: URI path of the resource.
- type: "con" for Confirmable or "non-con" for Non-confirmable (default).
- payload: CoAPS request payload.

```bash
> coaps delete test-resource con payload
Done
```

### disconnect

```bash
> coaps disconnect
coaps disconnected
Done
```

### get \<uri-path\> \[type\]

- uri-path: URI path of the resource.
- type: "con" for Confirmable or "non-con" for Non-confirmable (default). Use "block-<block-size>" if the response should be transferred block-wise. ("block-16","block-32","block-64","block-128","block-256","block-512","block-1024")

```bash
> coaps get test-resource
Done
```

```bash
> coaps get test-resource block-1024
Done
```

### post \<uri-path\> \[type\] \[payload\]

- uri-path: URI path of the resource.
- type: "con" for Confirmable or "non-con" for Non-confirmable (default). Use "block-<block-size>" to send blocks with random payload. ("block-16","block-32","block-64","block-128","block-256","block-512","block-1024")
- payload: CoAP request payload. If \[type\] is "block-<block-size>", the amount of blocks to be sent can be set here.

```bash
> coaps post test-resource con payload
Done
```

```bash
> coaps post test-resource block-1024 10
Done
```

### psk \<psk\> \<pskid\>

Set DTLS ciphersuite to `TLS_PSK_WITH_AES_128_CCM_8`.

- psk: pre-shared key
- pskid: pre-shared key identifier

```bash
> coaps psk 123 pskid
Done
```

### put \<uri-path\> \[type\] \[payload\]

- uri-path: URI path of the resource.
- type: "con" for Confirmable or "non-con" for Non-confirmable (default). Use "block-<block-size>" to send blocks with random payload. ("block-16","block-32","block-64","block-128","block-256","block-512","block-1024")
- payload: CoAP request payload. If \[type\] is "block-<block-size>", the amount of blocks to be sent can be set here.

```bash
> coaps put test-resource con payload
Done
```

```bash
> coaps put test-resource block-1024 10
Done
```

### resource \[uri-path\]

Sets the URI path for the test resource.

```bash
> coaps resource test-resource
Done
> coaps resource
test-resource
Done
```

### set \[new-content\]

Sets the content sent by the test resource.

```bash
> coaps set Testing123
Done
```

### start

Starts the application coaps service.

- checkPeerCert: Peer Certificate Check can be disabled by typing false.

```bash
> coaps start
Done
```

### stop

Stops the application coaps service.

```bash
> coaps stop
Done
```

### x509

Set DTLS ciphersuite to `TLS_ECDHE_ECDSA_WITH_AES_128_CCM_8`.

The X.509 certificate stored in [`src/cli/x509_cert_key.hpp`](x509_cert_key.hpp).

```bash
> coaps x509
Done
```
