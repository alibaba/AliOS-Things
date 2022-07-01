#include <stdio.h>
#include <iostream>
#include "image.hpp"
#include "face_recognition_tool.hpp"
#include "face_recognition_112_v1_s16.hpp"
#include "face_recognition_112_v1_s8.hpp"

#define QUANT_TYPE 1
#define USE_FACE_DETECTOR 0

#if USE_FACE_DETECTOR
#include "human_face_detect_msr01.hpp"
#include "human_face_detect_mnp01.hpp"
#endif

extern "C" void app_main(void)
{
#if QUANT_TYPE
    // S16 model
    FaceRecognition112V1S16 recognizer;
#else
    // S8 model
    FaceRecognition112V1S8 recognizer;
#endif

#if USE_FACE_DETECTOR
    HumanFaceDetectMSR01 s1(0.1F, 0.5F, 10, 0.2F);
    HumanFaceDetectMNP01 s2(0.5F, 0.3F, 5);

    std::list<dl::detect::result_t> &candidates_316 = s1.infer((uint8_t *)image_bgr_316_1, {240, 320, 3});
    std::list<dl::detect::result_t> &results_316 = s2.infer((uint8_t *)image_bgr_316_1, {240, 320, 3}, candidates_316);
    assert(results_316.size() == 1);
    vector<int> landmarks_316_1 = results_316.front().keypoint;

    std::list<dl::detect::result_t> &candidates_319 = s1.infer((uint8_t *)image_bgr_319_1, {240, 320, 3});
    std::list<dl::detect::result_t> &results_319 = s2.infer((uint8_t *)image_bgr_319_1, {240, 320, 3}, candidates_319);
    assert(results_319.size() == 1);
    vector<int> landmarks_319_1 = results_319.front().keypoint;

#else
    vector<int> landmarks_316_1 = image_316_1_landmarks;
    vector<int> landmarks_319_1 = image_319_1_landmarks;
#endif

    // load ids from flash partition
    recognizer.set_partition(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, "fr");
    recognizer.set_ids_from_flash();

    // prepare image data
    Tensor<uint8_t> image_rgb888_316_1;
    image_rgb888_316_1.set_element((uint8_t *)image_bgr_316_1).set_shape({240, 320, 3}).set_auto_free(false);

    Tensor<uint8_t> image_rgb888_319_1;
    image_rgb888_319_1.set_element((uint8_t *)image_bgr_319_1).set_shape({240, 320, 3}).set_auto_free(false);

    Tensor<uint8_t> aligned_face_rgb888_316_2;
    aligned_face_rgb888_316_2.set_element((uint8_t *)aligned_face112_image_bgr_316_2).set_shape({112, 112, 3}).set_auto_free(false);

    Tensor<uint8_t> aligned_face_rgb888_316_3;
    aligned_face_rgb888_316_3.set_element((uint8_t *)aligned_face112_image_bgr_316_3).set_shape({112, 112, 3}).set_auto_free(false);

    Tensor<uint8_t> aligned_face_rgb888_319_2;
    aligned_face_rgb888_319_2.set_element((uint8_t *)aligned_face112_image_bgr_319_2).set_shape({112, 112, 3}).set_auto_free(false);

    Tensor<uint8_t> aligned_face_rgb888_319_3;
    aligned_face_rgb888_319_3.set_element((uint8_t *)aligned_face112_image_bgr_319_3).set_shape({112, 112, 3}).set_auto_free(false);

    // enroll id
    cout << "\nenroll id ...\n";
    string name_316 = "Sandra";
    string name_319 = "Jiong";
    int id316, id319 = -1;

    //using the bgr888 image with landmarks of human face to enroll face id to flash
    if (recognizer.get_enrolled_id_num() == 0)
    {
        id316 = recognizer.enroll_id(image_rgb888_316_1, landmarks_316_1, name_316, true);
        cout << "name: " << name_316 << ", id: " << id316 << "\n";

        id319 = recognizer.enroll_id(image_rgb888_319_1, landmarks_319_1, name_319, true);
        cout << "name: " << name_319 << ", id: " << id319 << "\n";
    }
    else
    {
        vector<face_info_t> ids = recognizer.get_enrolled_ids();
        for (std::vector<face_info_t>::iterator i = ids.begin(); i != ids.end(); ++i)
        {
            if((*i).name == name_316)
            {
                id316 = (*i).id;
                cout << "ids from flash, name: " << name_316 << ", id: " << id316 << "\n";
            }
            if((*i).name == name_319)
            {
                id319 = (*i).id;
                cout << "ids from flash, name: " << name_319 << ", id: " << id319 << "\n";
            }
        }
        
    }

    // recognize face
    cout << "\nrecognize face ...\n";

    //recognize the aligned face with format bgr888 (make sure: aligned_face.shape == recognizer.get_input_shape())
    face_info_t recognize_316 = recognizer.recognize(aligned_face_rgb888_316_2);
    cout << "[recognition result] id: " << recognize_316.id << ", name: " << recognize_316.name << ", similarity: " << recognize_316.similarity << "\n";

    face_info_t recognize_319 = recognizer.recognize(aligned_face_rgb888_319_2);
    cout << "[recognition result] id: " << recognize_319.id << ", name: " << recognize_319.name << ", similarity: " << recognize_319.similarity << "\n";

    // recognizer information
    cout << "\nrecognizer information ...\n";

    cout << "recognizer threshold: " << recognizer.get_thresh() << "\n";
    vector<int> input_shape = recognizer.get_input_shape();
    cout << "input shape: " << input_shape[0] << ", " << input_shape[1] << ", " << input_shape[2] << "\n";

    // face id information
    cout << "\nface id information ...\n";

    cout << "number of enrolled ids: " << recognizer.get_enrolled_id_num() << "\n";
    vector<face_info_t> ids = recognizer.get_enrolled_ids();
    for (std::vector<face_info_t>::iterator i = ids.begin(); i != ids.end(); ++i)
    {
        cout << "id: " << (*i).id << ", name: " << (*i).name << "\n";
    }

    // delete id and update the id information to flash
    cout << "\ndelete id ...\n";
    cout << "number of remaining ids: " << recognizer.delete_id(id319, true) << "\n";

    recognize_319 = recognizer.recognize(aligned_face_rgb888_319_2);
    cout << "[recognition result] id: " << recognize_319.id << ", name: " << recognize_319.name << ", similarity: " << recognize_319.similarity << "\n";

    // enroll id
    cout << "\nenroll id ...\n";
    id319 = recognizer.enroll_id(aligned_face_rgb888_319_3, name_319);
    cout << "name: " << name_319 << ", id: " << id319 << "\n";

    // write enrolled ids to flash partition. 
    cout << "write " << recognizer.write_ids_to_flash() << " ids to flash.\n";

    // recognize face
    cout << "\nrecognize face ...\n";
    recognize_316 = recognizer.recognize(aligned_face_rgb888_316_3);
    cout << "[recognition result] id: " << recognize_316.id << ", name: " << recognize_316.name << ", similarity: " << recognize_316.similarity << "\n";

    recognize_319 = recognizer.recognize(image_rgb888_319_1, landmarks_319_1);
    cout << "[recognition result] id: " << recognize_319.id << ", name: " << recognize_319.name << ", similarity: " << recognize_319.similarity << "\n";
}