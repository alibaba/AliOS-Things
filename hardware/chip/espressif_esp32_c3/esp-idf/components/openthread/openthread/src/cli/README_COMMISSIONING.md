# OpenThread CLI - Commissioning

## Quick Start

### Form Network

Form a network with the device that has Commissioner support.

1. Generate and view new network configuration.

   ```bash
   > dataset init new
   Done
   > dataset
   Active Timestamp: 1
   Channel: 13
   Channel Mask: 0x07fff800
   Ext PAN ID: d63e8e3e495ebbc3
   Mesh Local Prefix: fd3d:b50b:f96d:722d::/64
   Network Key: dfd34f0f05cad978ec4e32b0413038ff
   Network Name: OpenThread-8f28
   PAN ID: 0x8f28
   PSKc: c23a76e98f1a6483639b1ac1271e2e27
   Security Policy: 0, onrcb
   Done
   ```

2. Commit new dataset to the Active Operational Dataset in non-volatile storage.

   ```bash
   dataset commit active
   Done
   ```

3. Enable Thread interface

   ```bash
   > ifconfig up
   Done
   > thread start
   Done
   ```

### Obtain Joiner IEEE EUI-64

1. Start the Joiner node and obtain the Joiner's IEEE EUI-64.

   ```bash
   > eui64
   18b4300000000002
   Done
   ```

### Start Commissioner

1. Start the Commissioner Role

   ```bash
   > commissioner start
   Commissioner: petitioning
   Done
   Commissioner: active
   ```

2. Add the Joiner with the device-specific pre-shared key (PSKd)

   ```bash
   > commissioner joiner add 18b4300000000002 J01NME
   Done
   ```

### Start Joiner

1. Factory reset the device.

   ```bash
   > factoryreset
   ```

2. Start the Joiner process.

   ```bash
   > ifconfig up
   Done
   > joiner start J01NME
   Done
   Join success
   ```

3. Attach to Thread network

   ```bash
   > thread start
   Done
   ```

4. After successful attach, validate the device has the same Active Operational Dataset as above.

   ```bash
   > dataset active
   Active Timestamp: 1
   Channel: 13
   Channel Mask: 0x07fff800
   Ext PAN ID: d63e8e3e495ebbc3
   Mesh Local Prefix: fd3d:b50b:f96d:722d::/64
   Network Key: dfd34f0f05cad978ec4e32b0413038ff
   Network Name: OpenThread-8f28
   PAN ID: 0x8f28
   PSKc: c23a76e98f1a6483639b1ac1271e2e27
   Security Policy: 0, onrcb
   Done
   ```

## CLI Reference

- [Commissioner CLI Reference](README_COMMISSIONER.md)
- [Joiner CLI Reference](README_JOINER.md)
