/*
*****************************************************************************
**
**  File        : XR871.ld
**
**  Abstract    : Linker script for XR871 Device with
**                2048KByte FLASH, 448KByte RAM
**
**                Set heap size, stack size and stack location according
**                to application requirements.
**
**                Set memory bank area and size if external memory is used.
**
**  Target      : Xradio Technology XR871
**
**  Environment : Gcc
**
**  Distribution: The file is distributed �as is,� without any warranty
**                of any kind.
**
**
*****************************************************************************
*/

/* Specify the memory areas */
MEMORY
{
  RAM (rwx)  : ORIGIN = 0x00010000, LENGTH = 448K
  FLASH (rx) : ORIGIN = 0x10000000, LENGTH = 768K
}

__RAM_BASE = ORIGIN(RAM);

ENTRY(Reset_Handler)

SECTIONS
{
    .xip :
    {
        __xip_start__ = .;
        /* examples */
        *alinkapp.a: (.text .text.* .rodata .rodata.*)
        *linkkitapp.a: (.text .text.* .rodata .rodata.*)
        *linkkit_gateway.a: (.text .text.* .rodata .rodata.*)
        *http2app.a: (.text .text.* .rodata .rodata.*)
        *opusapp.a: (.text .text.* .rodata .rodata.*)
        *linkvoiceapp.a: (.text .text.* .rodata .rodata.*)
        *xplayerapp.a: (.text .text.* .rodata .rodata.*)

        /* framework */
        *cloud.a: (.text .text.* .rodata .rodata.*)
        *common.a: (.text .text.* .rodata .rodata.*)
        *common_transport.a: (.text .text.* .rodata .rodata.*)
        *coap.a: (.text .text.* .rodata .rodata.*)
        *mqtt.a: (.text .text.* .rodata .rodata.*)
        *websockets.a: (.text .text.* .rodata .rodata.*)
        *mbedtls_wrapper.a:(.text .text.* .rodata .rodata.*)
        *wsf.a: (.text .text.* .rodata .rodata.*)
        *netmgr.a: (.text .text.* .rodata .rodata.*)
        *gateway.a: (.text .text.* .rodata .rodata.*)
        *framework.a: (.text .text.* .rodata .rodata.*)
        *linkvoice.a: (.text .text.* .rodata .rodata.*)
        *douglas.a: (.text .text.* .rodata .rodata.*)
        *opus.a: (.text .text.* .rodata .rodata.*)
        *alink.a: (.text .text.* .rodata .rodata.*)
        *alink_transport.a: (.text .text.* .rodata .rodata.*)
        *libiot_awss.a: (.text .text.* .rodata .rodata.*)
        *libiot_nghttp2.a: (.text .text.* .rodata .rodata.*)
        *libiot_http.a: (.text .text.* .rodata .rodata.*)
        *libiot_http2stream.a: (.text .text.* .rodata .rodata.*)
        *libiot_devmodel.a: (.text .text.* .rodata .rodata.*)
        *libiot_mqtt.a: (.text .text.* .rodata .rodata.*)
        *libiot_coap.a: (.text .text.* .rodata .rodata.*)
        *libiot_infra.a: (.text .text.* .rodata .rodata.*)
        *libiot_wrappers.a: (.text .text.* .rodata .rodata.*)
        *libiot_bind.a: (.text .text.* .rodata .rodata.*)
        *libiot_dynreg.a: (.text .text.* .rodata .rodata.*)
        *libiot_certs.a: (.text .text.* .rodata .rodata.*)
        *libiot_sign.a: (.text .text.* .rodata .rodata.*)
				*uOTA.a: (.text .text.* .rodata .rodata.*)

		/* kernel */
        *net.a: (.text .text.* .rodata .rodata.*)
        *mesh.a: (.text .text.* .rodata .rodata.*)
        *fatfs.a: (.text .text.* .rodata .rodata.*)
        *cli.a: (.text .text.* .rodata .rodata.*)
        *yloop.a: (.text .text.* .rodata .rodata.*)
        *kv.a: (.text .text.* .rodata .rodata.*)
        *udhcp.a: (.text .text.* .rodata .rodata.*)

		/* security */
        *alicrypto.a: (.text .text.* .rodata .rodata.*)
        *libmbedcrypto.a: (.text .text.* .rodata .rodata.*)
        *libmbedtls.a: (.text .text.* .rodata .rodata.*)
        *tlsrc.a: (.text .text.* .rodata .rodata.*)
        *mbedtls.a: (.text .text.* .rodata .rodata.*)


		/* utils */
        *base64.a: (.text .text.* .rodata .rodata.*)
        *cjson.a: (.text .text.* .rodata .rodata.*)
        *digest_algorithm.a: (.text .text.* .rodata .rodata.*)

		/* platform */
        *cedarx.a: (.text .text.* .rodata .rodata.*)
        *libmp3.a: (.text .text.* .rodata .rodata.*)
        *libamr.a: (.text .text.* .rodata .rodata.*)
	*libamren.a: (.text .text.* .rodata .rodata.*)
	*libaudmgr.a: (.text .text.* .rodata .rodata.*)
	*libudhcpd.a: (.text .text.* .rodata .rodata.*)
        *lwip.a: (.text .text.* .rodata .rodata.*)
        *efpg.a: (.text .text.* .rodata .rodata.*)

        /* All xip end */
        __xip_end__ = .;
    } > FLASH

    .text :
    {
        __text_start__ = .;
        KEEP(*(.isr_vector))
        KEEP(*(.text.Reset_Handler))
        KEEP(*(.cpu_text))
        *(.text*)
        KEEP(*(.init))
        KEEP(*(.fini))

        /* .ctors */
        *crtbegin.o(.ctors)
        *crtbegin?.o(.ctors)
        *(EXCLUDE_FILE(*crtend?.o *crtend.o) .ctors)
        *(SORT(.ctors.*))
        *(.ctors)

        /* .dtors */
        *crtbegin.o(.dtors)
        *crtbegin?.o(.dtors)
        *(EXCLUDE_FILE(*crtend?.o *crtend.o) .dtors)
        *(SORT(.dtors.*))
        *(.dtors)

        *(.rodata*)

        KEEP(*(.eh_frame*))
        /* All text end */
        __text_end__ = .;
    } > RAM

    .ARM.extab :
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > RAM

    __exidx_start = .;
    .ARM.exidx :
    {
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    } > RAM
    __exidx_end = .;

    __etext = .;
    _sidata = .;

    .data :
    {
        __data_start__ = .;
        _sdata = .;
        *(vtable)
        *(.data*)

        . = ALIGN(4);
        /* preinit data */
        PROVIDE_HIDDEN (__preinit_array_start = .);
        KEEP(*(.preinit_array))
        PROVIDE_HIDDEN (__preinit_array_end = .);

        . = ALIGN(4);
        /* init data */
        PROVIDE_HIDDEN (__init_array_start = .);
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array))
        PROVIDE_HIDDEN (__init_array_end = .);


        . = ALIGN(4);
        /* finit data */
        PROVIDE_HIDDEN (__fini_array_start = .);
        KEEP(*(SORT(.fini_array.*)))
        KEEP(*(.fini_array))
        PROVIDE_HIDDEN (__fini_array_end = .);

        KEEP(*(.jcr*))
        . = ALIGN(4);
        /* All data end */
        __data_end__ = .;
        _edata = .;

    } > RAM

    .bss :
    {
        . = ALIGN(4);
        __bss_start__ = .;
        _sbss = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        __bss_end__ = .;
        _ebss = .;
    } > RAM

    .heap (COPY):
    {
        __end__ = .;
        end = __end__;
        *(.heap*)
        __HeapLimit = .;
    } > RAM

    /* .stack_dummy section doesn't contains any symbols. It is only
     * used for linker to calculate size of stack sections, and assign
     * values to stack symbols later */
    .stack_dummy (COPY):
    {
        *(.stack*)
    } > RAM

    /* Set stack top to end of RAM, and stack limit move down by
     * size of stack_dummy section */
    __StackTop = ORIGIN(RAM) + LENGTH(RAM);
    __StackBottom = __StackTop - 0x400;
    _estack = __StackTop;
    __StackLimit = __StackTop - SIZEOF(.stack_dummy);
    PROVIDE(__stack = __StackTop);
    PROVIDE(heap_start = __end__);
    PROVIDE(heap_end = __StackBottom);
    PROVIDE(heap_len = heap_end - heap_start);

    /* Check if data + heap + stack exceeds RAM limit */
    ASSERT(__StackLimit >= __HeapLimit, "region RAM overflowed with stack")
}
