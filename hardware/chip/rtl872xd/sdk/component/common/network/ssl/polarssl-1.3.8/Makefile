
include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#

OBJS = library/aesni.o			\
       library/ccm.o			\
       library/cipher.o			\
       library/cipher_wrap.o		\
       library/debug.o			\
       library/ecp_ram.o		\
       library/error.o			\
       library/gcm.o			\
       library/havege.o			\
       library/memory_buffer_alloc.o	\
       library/padlock.o		\
       library/pbkdf2.o			\
       library/pkcs5.o			\
       library/pkcs11.o			\
       library/pkcs12.o			\
       library/pkparse.o		\
       library/ssl_cache.o		\
       library/ssl_cli.o		\
       library/ssl_srv.o   		\
       library/ssl_ciphersuites.o	\
       library/ssl_tls.o		\
       library/threading.o		\
       library/timing.o			\
       library/version.o		\
       library/version_features.o	\
       library/x509.o			\
       library/x509_create.o		\
       library/x509_crl.o		\
       library/x509_crt.o		\
       library/x509_csr.o		\
       library/x509write_crt.o		\
       library/x509write_csr.o		\
       library/net.o		\
       ../ssl_ram_map/ssl_ram_map.o	\
       ssl_self_test.o

#*****************************************************************************#
#                               Object FILE LIST                              #
#*****************************************************************************#

#*****************************************************************************#
#                               Include Dependency                            #
#*****************************************************************************#
-include $(OBJS:.o=.d)

	
#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
#*****************************************************************************#
# Define the Rules to build the core targets
all: CORE_TARGETS COPY_RAM_OBJS

#*****************************************************************************#
#              GENERATE OBJECT FILE 
#*****************************************************************************#
CORE_TARGETS: $(OBJS)

#*****************************************************************************#
#              CLEAN GENERATED FILES                                          #
#*****************************************************************************#
clean: CLEAN_OBJS 
	$(REMOVE) *.o
	$(REMOVE) *.d
	$(REMOVE) *.s
	$(REMOVE) *.i

-include $(DEPS)
