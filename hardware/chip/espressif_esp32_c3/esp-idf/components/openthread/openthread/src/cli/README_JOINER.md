# OpenThread CLI - Joiner

## Quick Start

See [README_COMMISSIONING.md](README_COMMISSIONING.md).

## Command List

- [help](#help)
- [discerner](#discerner)
- [id](#id)
- [start](#start)
- [stop](#stop)

## Command Details

### help

Usage: `joiner help`

Print joiner help menu.

```bash
> joiner help
help
id
start
stop
Done
```

### discerner

Usage: `joiner discerner [discerner]`

Print or set the Joiner Discerner. Note this value takes the place of the place of EUI-64 during the joiner session of Thread commissioning.

- discerner: `clear` to clear discerner, `number/length` to set discerner

```bash
> joiner discerner 0xabc/12
Done
> joiner discerner
0xabc/12
Done
> joiner discerner clear
Done
```

### id

Usage: `joiner id`

Print the Joiner ID.

```bash
> joiner id
d65e64fa83f81cf7
Done
```

### start

Usage: `joiner start <pskd> [provisioning-url]`

Start the Joiner role.

- pskd: Pre-Shared Key for the Joiner.
- provisioning-url: Provisioning URL for the Joiner (optional).

This command will cause the device to start the Joiner process.

```bash
> joiner start J01NM3
Done
```

### stop

Usage: `joiner stop`

Stop the Joiner role.

```bash
> joiner stop
Done
```
