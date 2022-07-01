#include "py/obj.h"
#include "py/nlr.h"
#include "py/runtime.h"

#if MICROPY_PY_LOCAL_AI

#include "py_helper.h"
#include "imlib.h"
#include "human_face_recognition.h"

typedef enum {
    CAM_PIXFORMAT_RGB565,    // 2BPP/RGB565
    CAM_PIXFORMAT_YUV422,    // 2BPP/YUV422
    CAM_PIXFORMAT_GRAYSCALE, // 1BPP/GRAYSCALE
    CAM_PIXFORMAT_JPEG,      // JPEG/COMPRESSED
    CAM_PIXFORMAT_RGB888,    // 3BPP/RGB888
    CAM_PIXFORMAT_RAW,       // RAW
    CAM_PIXFORMAT_RGB444,    // 3BP2P/RGB444
    CAM_PIXFORMAT_RGB555,    // 3BP2P/RGB555
} cam_pixformat_t;

static void py_localai_convert_image_to_camera_type(image_t *src_img, camera_fb_t *cam_fb)
{
  cam_fb->buf = src_img->data;
  cam_fb->width = src_img->w;
  cam_fb->height = src_img->h;

  switch(src_img->bpp) {
    case IMAGE_BPP_GRAYSCALE:
        cam_fb->format = CAM_PIXFORMAT_GRAYSCALE;
        break;
    case IMAGE_BPP_RGB565:
        cam_fb->format = CAM_PIXFORMAT_RGB565;
        break;
    default:
        printf("convert format default\n");
        break;
  }
}

STATIC mp_obj_t py_human_face_recognize_init()
{
  human_face_recognize_init();
  return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(py_human_face_recognize_init_obj, py_human_face_recognize_init);

STATIC mp_obj_t py_human_face_detect(mp_obj_t image)
{
  image_t *src_img = py_helper_arg_to_image_mutable_bayer(image);
  camera_fb_t cam_fb = {0};
  bool detect_result = false;
  py_localai_convert_image_to_camera_type(src_img, &cam_fb);
  detect_result = human_face_detect(&cam_fb);
  return mp_obj_new_bool(detect_result);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_human_face_detect_obj, py_human_face_detect);

STATIC mp_obj_t py_human_face_enroll(mp_obj_t image)
{
  image_t *src_img = py_helper_arg_to_image_mutable_bayer(image);
  camera_fb_t cam_fb = {0};
  int enroll_id = -1;
  py_localai_convert_image_to_camera_type(src_img, &cam_fb);
  enroll_id = human_face_enroll(&cam_fb);
  return mp_obj_new_int(enroll_id);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_human_face_enroll_obj, py_human_face_enroll);

STATIC mp_obj_t py_human_face_recognize(mp_obj_t image)
{
  image_t *src_img = py_helper_arg_to_image_mutable_bayer(image);
  camera_fb_t cam_fb = {0};
  int recognize_id = -1;
  py_localai_convert_image_to_camera_type(src_img, &cam_fb);
  recognize_id = human_face_recognize(&cam_fb);
  return mp_obj_new_int(recognize_id);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_human_face_recognize_obj, py_human_face_recognize);

STATIC mp_obj_t py_human_face_delete(mp_obj_t image)
{
  image_t *src_img = py_helper_arg_to_image_mutable_bayer(image);
  camera_fb_t cam_fb = {0};
  int left_ids = -1;
  py_localai_convert_image_to_camera_type(src_img, &cam_fb);
  left_ids = human_face_delete(&cam_fb);
  return mp_obj_new_int(left_ids);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_human_face_delete_obj, py_human_face_delete);

STATIC mp_obj_t py_human_face_result_print(mp_obj_t image, mp_obj_t show_stage)
{
  image_t *src_img = py_helper_arg_to_image_mutable_bayer(image);
  mp_int_t frame_show_stage = mp_obj_get_int(show_stage);
  camera_fb_t cam_fb = {0};
  int left_ids = -1;
  py_localai_convert_image_to_camera_type(src_img, &cam_fb);
  rgb_printf_result(&cam_fb, frame_show_stage);
  return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(py_human_face_result_print_obj, py_human_face_result_print);

STATIC const mp_rom_map_elem_t globals_dict_table[] = {
    {MP_ROM_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_localAI)},
    {MP_ROM_QSTR(MP_QSTR_human_recognize_init), MP_ROM_PTR(&py_human_face_recognize_init_obj)},
    {MP_ROM_QSTR(MP_QSTR_human_face_detect), MP_ROM_PTR(&py_human_face_detect_obj)},
    {MP_ROM_QSTR(MP_QSTR_human_face_enroll), MP_ROM_PTR(&py_human_face_enroll_obj)},
    {MP_ROM_QSTR(MP_QSTR_human_face_recognize), MP_ROM_PTR(&py_human_face_recognize_obj)},
    {MP_ROM_QSTR(MP_QSTR_human_face_delete), MP_ROM_PTR(&py_human_face_delete_obj)},
    {MP_ROM_QSTR(MP_QSTR_human_face_result_print), MP_ROM_PTR(&py_human_face_result_print_obj)}};
STATIC MP_DEFINE_CONST_DICT(globals_dict, globals_dict_table);

const mp_obj_module_t local_ai_module = {
    .base = {&mp_type_module},
    .globals = (mp_obj_t)&globals_dict,
};

MP_REGISTER_MODULE(MP_QSTR_localAI, local_ai_module, MICROPY_PY_LOCAL_AI);

#endif // MICROPY_PY_LOCAL_AI
