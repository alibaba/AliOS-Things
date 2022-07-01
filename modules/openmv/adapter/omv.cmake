if(CONFIG_OPENMV_SUPPORT)
get_filename_component(OPENMV_DIR ${MICROPY_DIR}/../modules/openmv ABSOLUTE)

list(APPEND MICROPY_DEF_MODULES_OMV MICROPY_PY_OMV_SENSOR=1)
list(APPEND MICROPY_DEF_MODULES_OMV MICROPY_PY_OMV_IMAGE=1)
list(APPEND MICROPY_DEF_MODULES_OMV MICROPY_PY_OMV_MJPGE=1)
list(APPEND MICROPY_DEF_MODULES_OMV MICROPY_PY_OMV_LCD=1)


set(OMV_SRCS
  ${OPENMV_DIR}/omv/alloc/xalloc.c
  ${OPENMV_DIR}/omv/alloc/umm_malloc.c
  ${OPENMV_DIR}/omv/alloc/dma_alloc.c

  ${OPENMV_DIR}/omv/common/array.c
  ${OPENMV_DIR}/omv/common/ff_wrapper.c
  ${OPENMV_DIR}/omv/common/ini.c
  ${OPENMV_DIR}/omv/common/ringbuf.c
  ${OPENMV_DIR}/omv/common/trace.c

  ${OPENMV_DIR}/omv/imlib/agast.c
  ${OPENMV_DIR}/omv/imlib/apriltag.c
  ${OPENMV_DIR}/omv/imlib/bayer.c                     
  ${OPENMV_DIR}/omv/imlib/binary.c                    
  ${OPENMV_DIR}/omv/imlib/blob.c                      
  ${OPENMV_DIR}/omv/imlib/bmp.c                       
  ${OPENMV_DIR}/omv/imlib/clahe.c                     
  ${OPENMV_DIR}/omv/imlib/collections.c               
  ${OPENMV_DIR}/omv/imlib/dmtx.c                      
  ${OPENMV_DIR}/omv/imlib/draw.c                      
  ${OPENMV_DIR}/omv/imlib/edge.c                      
  ${OPENMV_DIR}/omv/imlib/eye.c                       
  ${OPENMV_DIR}/omv/imlib/fast.c                      
  ${OPENMV_DIR}/omv/imlib/fft.c                       
  ${OPENMV_DIR}/omv/imlib/filter.c                                       
  ${OPENMV_DIR}/omv/imlib/font.c                           
  ${OPENMV_DIR}/omv/imlib/fsort.c                     
  ${OPENMV_DIR}/omv/imlib/gif.c                       
  ${OPENMV_DIR}/omv/imlib/haar.c                      
  ${OPENMV_DIR}/omv/imlib/hog.c                       
  ${OPENMV_DIR}/omv/imlib/hough.c                     
  ${OPENMV_DIR}/omv/imlib/imlib.c                     
  ${OPENMV_DIR}/omv/imlib/integral.c                  
  ${OPENMV_DIR}/omv/imlib/integral_mw.c               
  ${OPENMV_DIR}/omv/imlib/jpeg.c                      
  ${OPENMV_DIR}/omv/imlib/kmeans.c                    
  ${OPENMV_DIR}/omv/imlib/lab_tab.c                   
  ${OPENMV_DIR}/omv/imlib/lbp.c                       
  ${OPENMV_DIR}/omv/imlib/line.c                      
  ${OPENMV_DIR}/omv/imlib/lsd.c                       
  ${OPENMV_DIR}/omv/imlib/mathop.c                    
  ${OPENMV_DIR}/omv/imlib/mjpeg.c                     
  ${OPENMV_DIR}/omv/imlib/orb.c                       
  ${OPENMV_DIR}/omv/imlib/phasecorrelation.c          
  ${OPENMV_DIR}/omv/imlib/point.c                     
  ${OPENMV_DIR}/omv/imlib/pool.c                      
  ${OPENMV_DIR}/omv/imlib/ppm.c                       
  ${OPENMV_DIR}/omv/imlib/qrcode.c                    
  ${OPENMV_DIR}/omv/imlib/qsort.c                     
  ${OPENMV_DIR}/omv/imlib/rainbow_tab.c               
  ${OPENMV_DIR}/omv/imlib/rectangle.c                 
  ${OPENMV_DIR}/omv/imlib/selective_search.c          
  ${OPENMV_DIR}/omv/imlib/sincos_tab.c                
  ${OPENMV_DIR}/omv/imlib/stats.c                     
  ${OPENMV_DIR}/omv/imlib/template.c                  
  ${OPENMV_DIR}/omv/imlib/xyz_tab.c                   
  ${OPENMV_DIR}/omv/imlib/zbar.c

  ${OPENMV_DIR}/adapter/srcs/fb_alloc.c
  ${OPENMV_DIR}/adapter/srcs/unaligned_memcpy.c
  ${OPENMV_DIR}/adapter/srcs/mutex.c
  ${OPENMV_DIR}/adapter/srcs/fmath.c
  ${OPENMV_DIR}/adapter/srcs/framebuffer.c

  ${OPENMV_DIR}/adapter/port_esp32s/sensor.c 
  ${OPENMV_DIR}/adapter/port_esp32s/arm_dsp_compat.c
  ${OPENMV_DIR}/adapter/srcs/lcd.c
)

set(OMV_MOD_SRCS
  ${OPENMV_DIR}/omv/modules/py_clock.c
  ${OPENMV_DIR}/omv/modules/py_gif.c  
  ${OPENMV_DIR}/omv/modules/py_helper.c
  ${OPENMV_DIR}/omv/modules/py_imageio.c
  ${OPENMV_DIR}/omv/modules/py_mjpeg.c
  ${OPENMV_DIR}/omv/modules/py_omv.c

  ${OPENMV_DIR}/adapter/srcs/py_sensor.c
  ${OPENMV_DIR}/adapter/srcs/py_lcd.c
  ${OPENMV_DIR}/adapter/srcs/py_image.c
     
)

list(APPEND OMV_SRCS "${OMV_MOD_SRCS}")
if(CONFIG_USB_ENABLED)
list(APPEND OMV_SRCS ${OPENMV_DIR}/adapter/srcs/usbdbg.c)
endif()

set(OMV_INCLUDEDIRS
  ${OPENMV_DIR}/adapter/inc
  ${OPENMV_DIR}/adapter/board_esp32s
  ${OPENMV_DIR}/omv/alloc
  ${OPENMV_DIR}/omv/common
  ${OPENMV_DIR}/omv/imlib
  ${OPENMV_DIR}/omv/modules  
  ${MICROPY_DIR}/lib/oofatfs
  ${MICROPY_DIR}/py
)
endif()
