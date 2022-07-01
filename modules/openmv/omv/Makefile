# This file is part of the OpenMV project.
#
# Copyright (c) 2013-2021 Ibrahim Abdelkader <iabdalkader@openmv.io>
# Copyright (c) 2013-2021 Kwabena W. Agyeman <kwagyeman@openmv.io>
#
# This work is licensed under the MIT license, see the file LICENSE for details.
#
# OMV Makefile

SRCS += $(addprefix alloc/,     \
	xalloc.c                    \
	fb_alloc.c                  \
	umm_malloc.c                \
	dma_alloc.c                 \
	unaligned_memcpy.c          \
   )

SRCS += $(addprefix common/,    \
	array.c                     \
	ff_wrapper.c                \
	ini.c                       \
	ringbuf.c                   \
	trace.c                     \
	mutex.c                     \
	usbdbg.c                    \
   )

SRCS += $(addprefix sensors/,   \
	ov2640.c                    \
	ov5640.c                    \
	ov7670.c                    \
	ov7690.c                    \
	ov7725.c                    \
	ov9650.c                    \
	mt9v034.c                   \
	mt9m114.c                   \
	lepton.c                    \
	hm01b0.c                    \
   )

SRCS += $(addprefix modules/,   \
	py_clock.c                  \
	py_gif.c                    \
	py_helper.c                 \
	py_image.c                  \
	py_imageio.c                \
	py_mjpeg.c                  \
	py_omv.c                    \
	py_sensor.c                 \
	py_tf.c                     \
	py_fir.c                    \
   )

SRCS += $(addprefix imlib/,     \
	agast.c                     \
	apriltag.c                  \
	bayer.c                     \
	binary.c                    \
	blob.c                      \
	bmp.c                       \
	clahe.c                     \
	collections.c               \
	dmtx.c                      \
	draw.c                      \
	edge.c                      \
	eye.c                       \
	fast.c                      \
	fft.c                       \
	filter.c                    \
	fmath.c                     \
	font.c                      \
	framebuffer.c               \
	fsort.c                     \
	gif.c                       \
	haar.c                      \
	hog.c                       \
	hough.c                     \
	imlib.c                     \
	integral.c                  \
	integral_mw.c               \
	jpeg.c                      \
	kmeans.c                    \
	lab_tab.c                   \
	lbp.c                       \
	line.c                      \
	lsd.c                       \
	mathop.c                    \
	mjpeg.c                     \
	orb.c                       \
	phasecorrelation.c          \
	point.c                     \
	pool.c                      \
	ppm.c                       \
	qrcode.c                    \
	qsort.c                     \
	rainbow_tab.c               \
	rectangle.c                 \
	selective_search.c          \
	sincos_tab.c                \
	stats.c                     \
	template.c                  \
	xyz_tab.c                   \
	zbar.c                      \
   )

SRCS += $(wildcard ports/$(PORT)/*.c)
SRCS += $(wildcard ports/$(PORT)/modules/*.c)

OBJS = $(addprefix $(BUILD)/, $(SRCS:.c=.o))
OBJ_DIRS = $(sort $(dir $(OBJS)))

all: | $(OBJ_DIRS) $(OBJS)
$(OBJ_DIRS):
	$(MKDIR) -p $@

$(BUILD)/%.o : %.c
	$(ECHO) "CC $<"
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD)/%.o : %.s
	$(ECHO) "AS $<"
	$(AS) $(AFLAGS) $< -o $@

-include $(OBJS:%.o=%.d)
