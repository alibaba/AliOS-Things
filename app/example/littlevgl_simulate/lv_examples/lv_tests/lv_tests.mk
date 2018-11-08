NAME := lv_tests

$(NAME)_TYPE 	    := framework

#default gcc
ifeq ($(COMPILER),)
$(NAME)_CFLAGS      += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS      += -Wall -Werror
else ifeq ($(COMPILER),armcc)
GLOBAL_DEFINES      += __BSD_VISIBLE
else ifeq ($(COMPILER),rvct)
GLOBAL_DEFINES      += __BSD_VISIBLE
endif

GLOBAL_INCLUDES     += include
GLOBAL_DEFINES      += AOS_LV_TESTS

ifneq ($(IDE),keil)
$(NAME)_COMPONENTS  += app.example.littlevgl_simulate.lv_examples.lv_tests.lv_test_group
$(NAME)_COMPONENTS  += app.example.littlevgl_simulate.lv_examples.lv_tests.lv_test_obj
$(NAME)_COMPONENTS  += app.example.littlevgl_simulate.lv_examples.lv_tests.lv_test_objx.lv_test_bar
$(NAME)_COMPONENTS  += app.example.littlevgl_simulate.lv_examples.lv_tests.lv_test_objx.lv_test_btn
$(NAME)_COMPONENTS  += app.example.littlevgl_simulate.lv_examples.lv_tests.lv_test_objx.lv_test_btnm
$(NAME)_COMPONENTS  += app.example.littlevgl_simulate.lv_examples.lv_tests.lv_test_objx.lv_test_cb
$(NAME)_COMPONENTS  += app.example.littlevgl_simulate.lv_examples.lv_tests.lv_test_objx.lv_test_chart
$(NAME)_COMPONENTS  += app.example.littlevgl_simulate.lv_examples.lv_tests.lv_test_objx.lv_test_cont
$(NAME)_COMPONENTS  += app.example.littlevgl_simulate.lv_examples.lv_tests.lv_test_objx.lv_test_ddlist
$(NAME)_COMPONENTS  += app.example.littlevgl_simulate.lv_examples.lv_tests.lv_test_objx.lv_test_gauge
$(NAME)_COMPONENTS  += app.example.littlevgl_simulate.lv_examples.lv_tests.lv_test_objx.lv_test_img
$(NAME)_COMPONENTS  += app.example.littlevgl_simulate.lv_examples.lv_tests.lv_test_objx.lv_test_kb
$(NAME)_COMPONENTS  += app.example.littlevgl_simulate.lv_examples.lv_tests.lv_test_objx.lv_test_label
$(NAME)_COMPONENTS  += app.example.littlevgl_simulate.lv_examples.lv_tests.lv_test_objx.lv_test_led
$(NAME)_COMPONENTS  += app.example.littlevgl_simulate.lv_examples.lv_tests.lv_test_objx.lv_test_line
$(NAME)_COMPONENTS  += app.example.littlevgl_simulate.lv_examples.lv_tests.lv_test_objx.lv_test_list
$(NAME)_COMPONENTS  += app.example.littlevgl_simulate.lv_examples.lv_tests.lv_test_objx.lv_test_lmeter
$(NAME)_COMPONENTS  += app.example.littlevgl_simulate.lv_examples.lv_tests.lv_test_objx.lv_test_mbox
$(NAME)_COMPONENTS  += app.example.littlevgl_simulate.lv_examples.lv_tests.lv_test_objx.lv_test_page
$(NAME)_COMPONENTS  += app.example.littlevgl_simulate.lv_examples.lv_tests.lv_test_objx.lv_test_roller
$(NAME)_COMPONENTS  += app.example.littlevgl_simulate.lv_examples.lv_tests.lv_test_objx.lv_test_slider
$(NAME)_COMPONENTS  += app.example.littlevgl_simulate.lv_examples.lv_tests.lv_test_objx.lv_test_sw
$(NAME)_COMPONENTS  += app.example.littlevgl_simulate.lv_examples.lv_tests.lv_test_objx.lv_test_ta
$(NAME)_COMPONENTS  += app.example.littlevgl_simulate.lv_examples.lv_tests.lv_test_objx.lv_test_tabview
$(NAME)_COMPONENTS  += app.example.littlevgl_simulate.lv_examples.lv_tests.lv_test_objx.lv_test_win
$(NAME)_COMPONENTS  += app.example.littlevgl_simulate.lv_examples.lv_tests.lv_test_stress
$(NAME)_COMPONENTS  += app.example.littlevgl_simulate.lv_examples.lv_tests.lv_test_theme
endif