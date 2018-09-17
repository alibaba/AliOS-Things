caam_blob_gen application
=========================

Generates Cryptographic blob of data encryption key (DEK) during OEM manufacturing.  The DEK is used
for decrypting boot images.


Description
-----------

Background information: Encrypted boot is supported on i.MX6 D/Q amdi.MX6 DL/S. A cryptographic blob
of the DEK must be created during the OEM manufacturing stage on each processor and then attached to
the image on the boot device. The reason for this is the DEK blob is created using the device unique
key embedded into the Freescale processor which is only readable by the on-chip encryption engine.
The DEK is common to the image but the DEK blob is unique per IC.

CAAM_BLOB_GEN is provided as an example code that runs on the IC and uses the CAAM security hardware
engine to generate the blob. The resulting blob data is generated in memory at address 0x10701000
(can be changed in the code to any address). The CSF Install Key command (in versions of the CST
supporting encrypted boot only) for the secret key must have the same address specified in the blob
address field of the command.   


Requirements
------------

IC need to be in CLOSED security mode (SEC_CONFIG[1] efuse set) to create the blob.  It is possible
to create blobs on the IC in the OPEN security mode (SEC_CONFIG[0] set and SEC_CONFIG[1] cleared),
but the DEK blob will be generated using a known default test key rather than the unique key.


Build options
-------------

None.


Code organization
-----------------

apps/caam_blob_gen/
                   MakeFile
                   src/ 
                       caam_blob_gen.c
sdk/drivers/caam
                caam.h
                src/
                    caam.c
                    caam_internal.h
                test/
                    caam_test.c
                    Makefile

