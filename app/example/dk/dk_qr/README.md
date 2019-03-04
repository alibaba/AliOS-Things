### Content
```sh
devkit.dk_qr
	├── Config.in
	├── decode.c
	├── dk_qr.mk
	├── identify.c
	├── k_app_config.h
	├── qr.c
	├── qrcodegen.c
	├── qrcodegen.h
	├── quirc.c
	├── quirc.h
	├── quirc_internal.h
	├── README.md
	├── ucube.py
	└── version_db.c
```
### Overview
The **dk_qr** is a **QR** scan example based on developerkit board.

### Supported Boards

- developerkit

### Build and Flash

```sh
# generate dk.dk_qr@developerkit default config
aos make dk.dk_qr@developerkit -c config

# or customize config manually
aos make menuconfig

# build
aos make
```

### Run

* As default, Camera is to be opened to capture images, the terminal will print log if these images be identified as QR codes.
* Press `key B` to show the QR Code of AliOS Things on LCD disaplay.