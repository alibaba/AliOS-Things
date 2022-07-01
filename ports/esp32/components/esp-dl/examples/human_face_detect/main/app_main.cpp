#include <stdio.h>
#include "image.hpp"
#include "human_face_detect_msr01.hpp"
#include "human_face_detect_mnp01.hpp"
#include "dl_tool.hpp"

#define TWO_STAGE 1 /*<! 1: detect by two-stage which is more accurate but slower(with keypoints). */
                    /*<! 0: detect by one-stage which is less accurate but faster(without keypoints). */

extern "C" void app_main(void)
{
    dl::tool::Latency latency;

    // initialize
#if TWO_STAGE
    HumanFaceDetectMSR01 s1(0.1F, 0.5F, 10, 0.2F);
    HumanFaceDetectMNP01 s2(0.5F, 0.3F, 5);
#else // ONE_STAGE
    HumanFaceDetectMSR01 s1(0.3F, 0.5F, 10, 0.2F);
#endif

    // inference
    latency.start();
#if TWO_STAGE
    std::list<dl::detect::result_t> &candidates = s1.infer((uint8_t *)IMAGE_ELEMENT, {IMAGE_HEIGHT, IMAGE_WIDTH, IMAGE_CHANNEL});
    std::list<dl::detect::result_t> &results = s2.infer((uint8_t *)IMAGE_ELEMENT, {IMAGE_HEIGHT, IMAGE_WIDTH, IMAGE_CHANNEL}, candidates);
#else // ONE_STAGE
    std::list<dl::detect::result_t> &results = s1.infer((uint8_t *)IMAGE_ELEMENT, {IMAGE_HEIGHT, IMAGE_WIDTH, IMAGE_CHANNEL});
#endif
    latency.end();
    latency.print("Inference latency");

    // display
    int i = 0;
    for (std::list<dl::detect::result_t>::iterator prediction = results.begin(); prediction != results.end(); prediction++, i++)
    {
        printf("[%d] score: %f, box: [%d, %d, %d, %d]\n", i, prediction->score, prediction->box[0], prediction->box[1], prediction->box[2], prediction->box[3]);

#if TWO_STAGE
        printf("    left eye: (%3d, %3d), ", prediction->keypoint[0], prediction->keypoint[1]);
        printf("right eye: (%3d, %3d)\n", prediction->keypoint[6], prediction->keypoint[7]);
        printf("    nose: (%3d, %3d)\n", prediction->keypoint[4], prediction->keypoint[5]);
        printf("    mouth left: (%3d, %3d), ", prediction->keypoint[2], prediction->keypoint[3]);
        printf("mouth right: (%3d, %3d)\n\n", prediction->keypoint[8], prediction->keypoint[9]);
#endif
    }
}