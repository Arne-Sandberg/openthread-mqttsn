################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../openthread/third_party/mbedtls/repo/library/aes.c \
../openthread/third_party/mbedtls/repo/library/aesni.c \
../openthread/third_party/mbedtls/repo/library/arc4.c \
../openthread/third_party/mbedtls/repo/library/aria.c \
../openthread/third_party/mbedtls/repo/library/asn1parse.c \
../openthread/third_party/mbedtls/repo/library/asn1write.c \
../openthread/third_party/mbedtls/repo/library/base64.c \
../openthread/third_party/mbedtls/repo/library/bignum.c \
../openthread/third_party/mbedtls/repo/library/blowfish.c \
../openthread/third_party/mbedtls/repo/library/camellia.c \
../openthread/third_party/mbedtls/repo/library/ccm.c \
../openthread/third_party/mbedtls/repo/library/certs.c \
../openthread/third_party/mbedtls/repo/library/chacha20.c \
../openthread/third_party/mbedtls/repo/library/chachapoly.c \
../openthread/third_party/mbedtls/repo/library/cipher.c \
../openthread/third_party/mbedtls/repo/library/cipher_wrap.c \
../openthread/third_party/mbedtls/repo/library/cmac.c \
../openthread/third_party/mbedtls/repo/library/ctr_drbg.c \
../openthread/third_party/mbedtls/repo/library/debug.c \
../openthread/third_party/mbedtls/repo/library/des.c \
../openthread/third_party/mbedtls/repo/library/dhm.c \
../openthread/third_party/mbedtls/repo/library/ecdh.c \
../openthread/third_party/mbedtls/repo/library/ecdsa.c \
../openthread/third_party/mbedtls/repo/library/ecjpake.c \
../openthread/third_party/mbedtls/repo/library/ecp.c \
../openthread/third_party/mbedtls/repo/library/ecp_curves.c \
../openthread/third_party/mbedtls/repo/library/entropy.c \
../openthread/third_party/mbedtls/repo/library/entropy_poll.c \
../openthread/third_party/mbedtls/repo/library/error.c \
../openthread/third_party/mbedtls/repo/library/gcm.c \
../openthread/third_party/mbedtls/repo/library/havege.c \
../openthread/third_party/mbedtls/repo/library/hkdf.c \
../openthread/third_party/mbedtls/repo/library/hmac_drbg.c \
../openthread/third_party/mbedtls/repo/library/md.c \
../openthread/third_party/mbedtls/repo/library/md2.c \
../openthread/third_party/mbedtls/repo/library/md4.c \
../openthread/third_party/mbedtls/repo/library/md5.c \
../openthread/third_party/mbedtls/repo/library/md_wrap.c \
../openthread/third_party/mbedtls/repo/library/memory_buffer_alloc.c \
../openthread/third_party/mbedtls/repo/library/net_sockets.c \
../openthread/third_party/mbedtls/repo/library/nist_kw.c \
../openthread/third_party/mbedtls/repo/library/oid.c \
../openthread/third_party/mbedtls/repo/library/padlock.c \
../openthread/third_party/mbedtls/repo/library/pem.c \
../openthread/third_party/mbedtls/repo/library/pk.c \
../openthread/third_party/mbedtls/repo/library/pk_wrap.c \
../openthread/third_party/mbedtls/repo/library/pkcs11.c \
../openthread/third_party/mbedtls/repo/library/pkcs12.c \
../openthread/third_party/mbedtls/repo/library/pkcs5.c \
../openthread/third_party/mbedtls/repo/library/pkparse.c \
../openthread/third_party/mbedtls/repo/library/pkwrite.c \
../openthread/third_party/mbedtls/repo/library/platform.c \
../openthread/third_party/mbedtls/repo/library/platform_util.c \
../openthread/third_party/mbedtls/repo/library/poly1305.c \
../openthread/third_party/mbedtls/repo/library/ripemd160.c \
../openthread/third_party/mbedtls/repo/library/rsa.c \
../openthread/third_party/mbedtls/repo/library/rsa_internal.c \
../openthread/third_party/mbedtls/repo/library/sha1.c \
../openthread/third_party/mbedtls/repo/library/sha256.c \
../openthread/third_party/mbedtls/repo/library/sha512.c \
../openthread/third_party/mbedtls/repo/library/ssl_cache.c \
../openthread/third_party/mbedtls/repo/library/ssl_ciphersuites.c \
../openthread/third_party/mbedtls/repo/library/ssl_cli.c \
../openthread/third_party/mbedtls/repo/library/ssl_cookie.c \
../openthread/third_party/mbedtls/repo/library/ssl_srv.c \
../openthread/third_party/mbedtls/repo/library/ssl_ticket.c \
../openthread/third_party/mbedtls/repo/library/ssl_tls.c \
../openthread/third_party/mbedtls/repo/library/threading.c \
../openthread/third_party/mbedtls/repo/library/timing.c \
../openthread/third_party/mbedtls/repo/library/version.c \
../openthread/third_party/mbedtls/repo/library/version_features.c \
../openthread/third_party/mbedtls/repo/library/x509.c \
../openthread/third_party/mbedtls/repo/library/x509_create.c \
../openthread/third_party/mbedtls/repo/library/x509_crl.c \
../openthread/third_party/mbedtls/repo/library/x509_crt.c \
../openthread/third_party/mbedtls/repo/library/x509_csr.c \
../openthread/third_party/mbedtls/repo/library/x509write_crt.c \
../openthread/third_party/mbedtls/repo/library/x509write_csr.c \
../openthread/third_party/mbedtls/repo/library/xtea.c 

OBJS += \
./openthread/third_party/mbedtls/repo/library/aes.o \
./openthread/third_party/mbedtls/repo/library/aesni.o \
./openthread/third_party/mbedtls/repo/library/arc4.o \
./openthread/third_party/mbedtls/repo/library/aria.o \
./openthread/third_party/mbedtls/repo/library/asn1parse.o \
./openthread/third_party/mbedtls/repo/library/asn1write.o \
./openthread/third_party/mbedtls/repo/library/base64.o \
./openthread/third_party/mbedtls/repo/library/bignum.o \
./openthread/third_party/mbedtls/repo/library/blowfish.o \
./openthread/third_party/mbedtls/repo/library/camellia.o \
./openthread/third_party/mbedtls/repo/library/ccm.o \
./openthread/third_party/mbedtls/repo/library/certs.o \
./openthread/third_party/mbedtls/repo/library/chacha20.o \
./openthread/third_party/mbedtls/repo/library/chachapoly.o \
./openthread/third_party/mbedtls/repo/library/cipher.o \
./openthread/third_party/mbedtls/repo/library/cipher_wrap.o \
./openthread/third_party/mbedtls/repo/library/cmac.o \
./openthread/third_party/mbedtls/repo/library/ctr_drbg.o \
./openthread/third_party/mbedtls/repo/library/debug.o \
./openthread/third_party/mbedtls/repo/library/des.o \
./openthread/third_party/mbedtls/repo/library/dhm.o \
./openthread/third_party/mbedtls/repo/library/ecdh.o \
./openthread/third_party/mbedtls/repo/library/ecdsa.o \
./openthread/third_party/mbedtls/repo/library/ecjpake.o \
./openthread/third_party/mbedtls/repo/library/ecp.o \
./openthread/third_party/mbedtls/repo/library/ecp_curves.o \
./openthread/third_party/mbedtls/repo/library/entropy.o \
./openthread/third_party/mbedtls/repo/library/entropy_poll.o \
./openthread/third_party/mbedtls/repo/library/error.o \
./openthread/third_party/mbedtls/repo/library/gcm.o \
./openthread/third_party/mbedtls/repo/library/havege.o \
./openthread/third_party/mbedtls/repo/library/hkdf.o \
./openthread/third_party/mbedtls/repo/library/hmac_drbg.o \
./openthread/third_party/mbedtls/repo/library/md.o \
./openthread/third_party/mbedtls/repo/library/md2.o \
./openthread/third_party/mbedtls/repo/library/md4.o \
./openthread/third_party/mbedtls/repo/library/md5.o \
./openthread/third_party/mbedtls/repo/library/md_wrap.o \
./openthread/third_party/mbedtls/repo/library/memory_buffer_alloc.o \
./openthread/third_party/mbedtls/repo/library/net_sockets.o \
./openthread/third_party/mbedtls/repo/library/nist_kw.o \
./openthread/third_party/mbedtls/repo/library/oid.o \
./openthread/third_party/mbedtls/repo/library/padlock.o \
./openthread/third_party/mbedtls/repo/library/pem.o \
./openthread/third_party/mbedtls/repo/library/pk.o \
./openthread/third_party/mbedtls/repo/library/pk_wrap.o \
./openthread/third_party/mbedtls/repo/library/pkcs11.o \
./openthread/third_party/mbedtls/repo/library/pkcs12.o \
./openthread/third_party/mbedtls/repo/library/pkcs5.o \
./openthread/third_party/mbedtls/repo/library/pkparse.o \
./openthread/third_party/mbedtls/repo/library/pkwrite.o \
./openthread/third_party/mbedtls/repo/library/platform.o \
./openthread/third_party/mbedtls/repo/library/platform_util.o \
./openthread/third_party/mbedtls/repo/library/poly1305.o \
./openthread/third_party/mbedtls/repo/library/ripemd160.o \
./openthread/third_party/mbedtls/repo/library/rsa.o \
./openthread/third_party/mbedtls/repo/library/rsa_internal.o \
./openthread/third_party/mbedtls/repo/library/sha1.o \
./openthread/third_party/mbedtls/repo/library/sha256.o \
./openthread/third_party/mbedtls/repo/library/sha512.o \
./openthread/third_party/mbedtls/repo/library/ssl_cache.o \
./openthread/third_party/mbedtls/repo/library/ssl_ciphersuites.o \
./openthread/third_party/mbedtls/repo/library/ssl_cli.o \
./openthread/third_party/mbedtls/repo/library/ssl_cookie.o \
./openthread/third_party/mbedtls/repo/library/ssl_srv.o \
./openthread/third_party/mbedtls/repo/library/ssl_ticket.o \
./openthread/third_party/mbedtls/repo/library/ssl_tls.o \
./openthread/third_party/mbedtls/repo/library/threading.o \
./openthread/third_party/mbedtls/repo/library/timing.o \
./openthread/third_party/mbedtls/repo/library/version.o \
./openthread/third_party/mbedtls/repo/library/version_features.o \
./openthread/third_party/mbedtls/repo/library/x509.o \
./openthread/third_party/mbedtls/repo/library/x509_create.o \
./openthread/third_party/mbedtls/repo/library/x509_crl.o \
./openthread/third_party/mbedtls/repo/library/x509_crt.o \
./openthread/third_party/mbedtls/repo/library/x509_csr.o \
./openthread/third_party/mbedtls/repo/library/x509write_crt.o \
./openthread/third_party/mbedtls/repo/library/x509write_csr.o \
./openthread/third_party/mbedtls/repo/library/xtea.o 

C_DEPS += \
./openthread/third_party/mbedtls/repo/library/aes.d \
./openthread/third_party/mbedtls/repo/library/aesni.d \
./openthread/third_party/mbedtls/repo/library/arc4.d \
./openthread/third_party/mbedtls/repo/library/aria.d \
./openthread/third_party/mbedtls/repo/library/asn1parse.d \
./openthread/third_party/mbedtls/repo/library/asn1write.d \
./openthread/third_party/mbedtls/repo/library/base64.d \
./openthread/third_party/mbedtls/repo/library/bignum.d \
./openthread/third_party/mbedtls/repo/library/blowfish.d \
./openthread/third_party/mbedtls/repo/library/camellia.d \
./openthread/third_party/mbedtls/repo/library/ccm.d \
./openthread/third_party/mbedtls/repo/library/certs.d \
./openthread/third_party/mbedtls/repo/library/chacha20.d \
./openthread/third_party/mbedtls/repo/library/chachapoly.d \
./openthread/third_party/mbedtls/repo/library/cipher.d \
./openthread/third_party/mbedtls/repo/library/cipher_wrap.d \
./openthread/third_party/mbedtls/repo/library/cmac.d \
./openthread/third_party/mbedtls/repo/library/ctr_drbg.d \
./openthread/third_party/mbedtls/repo/library/debug.d \
./openthread/third_party/mbedtls/repo/library/des.d \
./openthread/third_party/mbedtls/repo/library/dhm.d \
./openthread/third_party/mbedtls/repo/library/ecdh.d \
./openthread/third_party/mbedtls/repo/library/ecdsa.d \
./openthread/third_party/mbedtls/repo/library/ecjpake.d \
./openthread/third_party/mbedtls/repo/library/ecp.d \
./openthread/third_party/mbedtls/repo/library/ecp_curves.d \
./openthread/third_party/mbedtls/repo/library/entropy.d \
./openthread/third_party/mbedtls/repo/library/entropy_poll.d \
./openthread/third_party/mbedtls/repo/library/error.d \
./openthread/third_party/mbedtls/repo/library/gcm.d \
./openthread/third_party/mbedtls/repo/library/havege.d \
./openthread/third_party/mbedtls/repo/library/hkdf.d \
./openthread/third_party/mbedtls/repo/library/hmac_drbg.d \
./openthread/third_party/mbedtls/repo/library/md.d \
./openthread/third_party/mbedtls/repo/library/md2.d \
./openthread/third_party/mbedtls/repo/library/md4.d \
./openthread/third_party/mbedtls/repo/library/md5.d \
./openthread/third_party/mbedtls/repo/library/md_wrap.d \
./openthread/third_party/mbedtls/repo/library/memory_buffer_alloc.d \
./openthread/third_party/mbedtls/repo/library/net_sockets.d \
./openthread/third_party/mbedtls/repo/library/nist_kw.d \
./openthread/third_party/mbedtls/repo/library/oid.d \
./openthread/third_party/mbedtls/repo/library/padlock.d \
./openthread/third_party/mbedtls/repo/library/pem.d \
./openthread/third_party/mbedtls/repo/library/pk.d \
./openthread/third_party/mbedtls/repo/library/pk_wrap.d \
./openthread/third_party/mbedtls/repo/library/pkcs11.d \
./openthread/third_party/mbedtls/repo/library/pkcs12.d \
./openthread/third_party/mbedtls/repo/library/pkcs5.d \
./openthread/third_party/mbedtls/repo/library/pkparse.d \
./openthread/third_party/mbedtls/repo/library/pkwrite.d \
./openthread/third_party/mbedtls/repo/library/platform.d \
./openthread/third_party/mbedtls/repo/library/platform_util.d \
./openthread/third_party/mbedtls/repo/library/poly1305.d \
./openthread/third_party/mbedtls/repo/library/ripemd160.d \
./openthread/third_party/mbedtls/repo/library/rsa.d \
./openthread/third_party/mbedtls/repo/library/rsa_internal.d \
./openthread/third_party/mbedtls/repo/library/sha1.d \
./openthread/third_party/mbedtls/repo/library/sha256.d \
./openthread/third_party/mbedtls/repo/library/sha512.d \
./openthread/third_party/mbedtls/repo/library/ssl_cache.d \
./openthread/third_party/mbedtls/repo/library/ssl_ciphersuites.d \
./openthread/third_party/mbedtls/repo/library/ssl_cli.d \
./openthread/third_party/mbedtls/repo/library/ssl_cookie.d \
./openthread/third_party/mbedtls/repo/library/ssl_srv.d \
./openthread/third_party/mbedtls/repo/library/ssl_ticket.d \
./openthread/third_party/mbedtls/repo/library/ssl_tls.d \
./openthread/third_party/mbedtls/repo/library/threading.d \
./openthread/third_party/mbedtls/repo/library/timing.d \
./openthread/third_party/mbedtls/repo/library/version.d \
./openthread/third_party/mbedtls/repo/library/version_features.d \
./openthread/third_party/mbedtls/repo/library/x509.d \
./openthread/third_party/mbedtls/repo/library/x509_create.d \
./openthread/third_party/mbedtls/repo/library/x509_crl.d \
./openthread/third_party/mbedtls/repo/library/x509_crt.d \
./openthread/third_party/mbedtls/repo/library/x509_csr.d \
./openthread/third_party/mbedtls/repo/library/x509write_crt.d \
./openthread/third_party/mbedtls/repo/library/x509write_csr.d \
./openthread/third_party/mbedtls/repo/library/xtea.d 


# Each subdirectory must supply rules for building sources it contributes
openthread/third_party/mbedtls/repo/library/%.o: ../openthread/third_party/mbedtls/repo/library/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__NEWLIB__ -DFSL_RTOS_BM -DSDK_OS_BAREMETAL -DSDK_DEBUGCONSOLE=1 -DCPU_MKW41Z512CAT4_cm0plus -DCPU_MKW41Z512CAT4 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -DMBEDTLS_CONFIG_FILE='"mbedtls-config.h"' -DOPENTHREAD_PROJECT_CORE_CONFIG_FILE='"openthread-core-kw41z-config.h"' -DOPENTHREAD_CONFIG_FILE='"openthread-config-generic.h"' -DOPENTHREAD_FTD=1 -DCPU_MKW41Z512VHT4 -DCPU_MKW41Z512VHT4_cm0plus -DOPENTHREAD_CONFIG_ENABLE_DEBUG_UART=1 -DOPENTHREAD_CONFIG_LOG_OUTPUT=OPENTHREAD_CONFIG_LOG_OUTPUT_APP -I../board -I../source -I../ -I../drivers -I../CMSIS -I../utilities -I../startup -I"C:/MCUXpressoIDE/workspace/openthread-mqttsn/openthread/include" -I"C:/MCUXpressoIDE/workspace/openthread-mqttsn/openthread/src/core" -I"C:/MCUXpressoIDE/workspace/openthread-mqttsn/openthread/third_party/mbedtls/repo/include" -I"C:/MCUXpressoIDE/workspace/openthread-mqttsn/openthread/examples/platforms" -I"C:/MCUXpressoIDE/workspace/openthread-mqttsn/openthread/examples/platforms/kw41z" -I"C:/MCUXpressoIDE/workspace/openthread-mqttsn/openthread/third_party/nxp/MKW41Z4/XCVR" -I"C:/MCUXpressoIDE/workspace/openthread-mqttsn/MQTTSNPacket/src" -O0 -fno-common -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -mcpu=cortex-m0plus -mthumb -D__NEWLIB__ -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


