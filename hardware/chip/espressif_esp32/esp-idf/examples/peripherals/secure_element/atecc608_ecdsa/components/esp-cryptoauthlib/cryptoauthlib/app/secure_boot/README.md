# Secure boot using ATECC608A

The SecureBoot command is a new feature on the
[ATECC608A](https://www.microchip.com/wwwproducts/en/ATECC608A) device compared
to earlier CryptoAuthentication devices from Microchip. This feature helps the
MCU to identify fraudulent code installed on it. When this feature is
implemented, the MCU can send a firmware digest and signature to the ATECC608A.
The ATECC608A validates this information (ECDSA verify) and responds to host
with a yes or no answer.

The ATECC608A provides options to reduce the firmware verification time by
storing the signature or digest after a good full verification (FullStore mode
of the SecureBoot command).

 - When the ATECC608A stores the digest (SecureBootMode is FullDig), the host
   only needs to send the firmware digest, which is compared to the stored
   copy. This skips the comparatively lengthy ECDSA verify, speeding up the
   secure boot process.

 - When the ATECC608A stores the signature (SecureBootMode is FullSig), the
   host only needs to send the firmware digest, which is verified against the
   stored signature using ECDSA. This saves time by not needing to send the
   signature in the command over the bus.

The ATECC608A also provides wire protection features for the SecureBoot
command, which can be used to encrypt the digest being sent from the host to
the ATECC608A and add a MAC to the verify result coming back to the host so it
can't be forced to a success state. This feature makes use of a shared secret
between the host and ATECC608A, called the IO protection key.

The secure boot feature can be easily integrated to an existing project. The
project should include the following files from the secure_boot folder:
 - secure_boot.c
 - secure_boot.h
 - secure_boot_memory.h
 - io_protection_key.h

The project should also implement the following platform-specific APIs:
 - secure_boot_init_memory()
 - secure_boot_read_memory()
 - secure_boot_deinit_memory()
 - secure_boot_mark_full_copy_completion()
 - secure_boot_check_full_copy_completion()
 - io_protection_get_key()
 - io_protection_set_key()

The project can set the secure boot configuration with the following defines:
 - SECURE_BOOT_CONFIGURATION
 - SECURE_BOOT_DIGEST_ENCRYPT_ENABLED
 - SECURE_BOOT_UPGRADE_SUPPORT

The secure boot process is performed by initializing CryptoAuthLib and calling
the secure_boot_process() function.

## Implementation Considerations

 - Need to perform SHA256 calculations on the host. CryptoAuthLib provides a
   software implementation in lib/crypto/atca_crypto_sw_sha2.c

 - When using the wire protection features:

    - The host needs to be able to generate a nonce (number used once).
      This is the NumIn parameter to the Nonce command that is sent before the
      SecureBoot command. The ATECC608A can not be used to generate NumIn, but
      it should come from a good random or non-repeating source in the host.
      
    - If the host has any protected internal memory, it should be used to store
      its copy of the IO protection key.

 - Secure boot depends on proper protections of the boot loader code in the
   host. If the code can be easily changed, then the secure boot process can be
   easily skipped. Boot loader should ideally be stored in an immutable
   (unchangeable) location like a boot ROM or write-protected flash.

 - Note that these APIs don't provision the ATECC608A. They assume the
   ATECC608A has already been configured and provisioned with the necessary
   keys for secure boot.

Examples
-----------
For more information about secure boot, please see the example implementation
project and documentation at:
https://github.com/MicrochipTech/cryptoauth_usecase_secureboot
