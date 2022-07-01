#include <stdio.h>
#include <iostream>
#include "image.hpp"
#include "color_detector.hpp"

using namespace dl;
using namespace std;

extern "C" void app_main(void)
{
    // set color detection model.
    ColorDetector detector;
    detector.set_color_thresh_offset({2, 4, 8});
    detector.set_detection_shape({64, 64, 3});

    // set the Tensor of test image.
    Tensor<uint8_t> rgby;
    rgby.set_element((uint8_t *)rgby_image).set_shape(rgby_shape).set_auto_free(false);

    // get color threshold of each color block in the image.
    // In the test image, we divided the picture into four regions, corresponding to four colors: red, green, blue and yellow.
    detector.register_color(rgby, {0, 0, 128, 32}, 256, "red");
    detector.register_color(rgby, {0, 32, 128, 64}, 256, "green");
    detector.register_color(rgby, {0, 64, 128, 96}, 256, "blue");
    detector.register_color(rgby, {0, 96, 128, 128}, 256, "yellow");

    // get the registered colors 
    std::vector<color_info_t> registered_colors = detector.get_registered_colors();
    printf("the information of registered colors: \n");
    printf("name: %s, \tthresh: %d, %d, %d, %d, %d, %d\n", registered_colors[0].name.c_str(), registered_colors[0].color_thresh[0], registered_colors[0].color_thresh[1], registered_colors[0].color_thresh[2], registered_colors[0].color_thresh[3], registered_colors[0].color_thresh[4], registered_colors[0].color_thresh[5]);
    printf("name: %s, \tthresh: %d, %d, %d, %d, %d, %d\n", registered_colors[1].name.c_str(), registered_colors[1].color_thresh[0], registered_colors[1].color_thresh[1], registered_colors[1].color_thresh[2], registered_colors[1].color_thresh[3], registered_colors[1].color_thresh[4], registered_colors[1].color_thresh[5]);
    printf("name: %s, \tthresh: %d, %d, %d, %d, %d, %d\n", registered_colors[2].name.c_str(), registered_colors[2].color_thresh[0], registered_colors[2].color_thresh[1], registered_colors[2].color_thresh[2], registered_colors[2].color_thresh[3], registered_colors[2].color_thresh[4], registered_colors[2].color_thresh[5]);
    printf("name: %s, \tthresh: %d, %d, %d, %d, %d, %d\n", registered_colors[3].name.c_str(), registered_colors[3].color_thresh[0], registered_colors[3].color_thresh[1], registered_colors[3].color_thresh[2], registered_colors[3].color_thresh[3], registered_colors[3].color_thresh[4], registered_colors[3].color_thresh[5]);

    // set new area threshold.
    detector.set_area_thresh({64});

    // detect the four colors in the test picture based on the threshold set above.
    std::vector<std::vector<color_detect_result_t>> &results888 = detector.detect(rgby);
    // print the detection results.
    detector.print_detection_results("RGB888");

    // copy the RGB565 data as the input image.
    uint16_t *rgby_565 = (uint16_t *)malloc(rgby_shape[0] * rgby_shape[1] * sizeof(uint16_t));
    assert(rgby_565 != NULL);
    memcpy(rgby_565, rgby_565_image, rgby_shape[0] * rgby_shape[1] * sizeof(uint16_t));

    // detect the four colors in the test picture based on the threshold set above.
    std::vector<std::vector<color_detect_result_t>> &results565 = detector.detect(rgby_565, rgby_shape);
    // print the detection results.
    detector.print_detection_results("RGB565");

    // delete the "red" color
    int color_num = detector.delete_color("red");
    printf("remained colors num: %d\n", color_num);

    // detect "blue" and "yellow"  
    detector.detect(rgby, {1, 2});

    // print the detection results.
    detector.print_detection_results("Blue, Yellow");
    // print the segmentation results.
    detector.print_segmentation_results("Blue, Yellow");
}