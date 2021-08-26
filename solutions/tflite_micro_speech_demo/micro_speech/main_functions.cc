/* Copyright 2020 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/
#include "ulog/ulog.h"
#include "micro_speech/main_functions.h"

#include "micro_speech/audio_provider.h"
#include "micro_speech/command_responder.h"
#include "micro_speech/feature_provider.h"
#include "micro_speech/micro_features/micro_model_settings.h"
#include "micro_speech/micro_features/yes_micro_features_data.h"
#include "micro_speech/micro_features/no_micro_features_data.h"
#include "micro_speech/micro_features/model.h"
#include "micro_speech/recognize_commands.h"
#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
// #include "tensorflow/lite/micro/system_setup.h"
#include "tensorflow/lite/schema/schema_generated.h"
// #include "tensorflow/lite/micro/cortex_m_generic/debug_log_callback.h"
#include "tensorflow/lite/micro/testing/micro_test.h"

// Globals, used for compatibility with Arduino-style sketches.
namespace
{
    tflite::ErrorReporter *error_reporter = nullptr;
    const tflite::Model *model = nullptr;
    tflite::MicroInterpreter *interpreter = nullptr;
    TfLiteTensor *model_input = nullptr;
    FeatureProvider *feature_provider = nullptr;
    RecognizeCommands *recognizer = nullptr;
    int32_t previous_time = 0;

    // Create an area of memory to use for input, output, and intermediate arrays.
    // The size of this will depend on the model you're using, and may need to be
    // determined by experimentation.
    constexpr int kTensorArenaSize = 10 * 1024;
    uint8_t tensor_arena[kTensorArenaSize];
    int8_t feature_buffer[kFeatureElementCount];
    int8_t *model_input_buffer = nullptr;
} // namespace

void callback(const char *s)
{
    LOG("%s\n", s);
}

// The name of this function is important for Arduino compatibility.
void setup()
{
    //   tflite::InitializeTarget();

    //RegisterDebugLogCallback(callback);
    // Set up logging. Google style is to avoid globals or statics because of
    // lifetime uncertainty, but since this has a trivial destructor it's okay.
    // NOLINTNEXTLINE(runtime-global-variables)
    static tflite::MicroErrorReporter micro_error_reporter;
    error_reporter = &micro_error_reporter;

    // Map the model into a usable data structure. This doesn't involve any
    // copying or parsing, it's a very lightweight operation.
    model = tflite::GetModel(g_model);
    if (model->version() != TFLITE_SCHEMA_VERSION)
    {
        TF_LITE_REPORT_ERROR(error_reporter,
                             "Model provided is schema version %d not equal "
                             "to supported version %d.",
                             model->version(), TFLITE_SCHEMA_VERSION);
        return;
    }

    // Pull in only the operation implementations we need.
    // This relies on a complete list of all the ops needed by this graph.
    // An easier approach is to just use the AllOpsResolver, but this will
    // incur some penalty in code space for op implementations that are not
    // needed by this graph.
    //
    // tflite::AllOpsResolver resolver;
    // NOLINTNEXTLINE(runtime-global-variables)
    static tflite::MicroMutableOpResolver<4> micro_op_resolver(error_reporter);
    if (micro_op_resolver.AddDepthwiseConv2D() != kTfLiteOk)
    {
        return;
    }
    if (micro_op_resolver.AddFullyConnected() != kTfLiteOk)
    {
        return;
    }
    if (micro_op_resolver.AddSoftmax() != kTfLiteOk)
    {
        return;
    }
    if (micro_op_resolver.AddReshape() != kTfLiteOk)
    {
        return;
    }

    // Build an interpreter to run the model with.
    static tflite::MicroInterpreter static_interpreter(
        model, micro_op_resolver, tensor_arena, kTensorArenaSize, error_reporter);
    interpreter = &static_interpreter;

    // Allocate memory from the tensor_arena for the model's tensors.
    TfLiteStatus allocate_status = interpreter->AllocateTensors();
    if (allocate_status != kTfLiteOk)
    {
        TF_LITE_REPORT_ERROR(error_reporter, "AllocateTensors() failed");
        return;
    }

    // Get information about the memory area to use for the model's input.
    model_input = interpreter->input(0);
    if ((model_input->dims->size != 2) || (model_input->dims->data[0] != 1) ||
        (model_input->dims->data[1] !=
         (kFeatureSliceCount * kFeatureSliceSize)) ||
        (model_input->type != kTfLiteInt8))
    {
        TF_LITE_REPORT_ERROR(error_reporter,
                             "Bad input tensor parameters in model");
        return;
    }
    model_input_buffer = model_input->data.int8;

    // Prepare to access the audio spectrograms from a microphone or other source
    // that will provide the inputs to the neural network.
    // NOLINTNEXTLINE(runtime-global-variables)
    static FeatureProvider static_feature_provider(kFeatureElementCount,
                                                   feature_buffer);
    feature_provider = &static_feature_provider;

    static RecognizeCommands static_recognizer(error_reporter);
    recognizer = &static_recognizer;

    previous_time = 0;

    RespondCommandThreadInit();
}

// The name of this function is important for Arduino compatibility.
void loop()
{
    // Fetch the spectrogram for the current time.
    const int32_t current_time = LatestAudioTimestamp();
    int how_many_new_slices = 0;
    TfLiteStatus feature_status = feature_provider->PopulateFeatureData(
        error_reporter, previous_time, current_time, &how_many_new_slices);
    // LOG("current_time: %d, previous_time: %d, how_many_new_slices: %d\n", current_time, previous_time, how_many_new_slices);
    if (feature_status != kTfLiteOk)
    {
        TF_LITE_REPORT_ERROR(error_reporter, "Feature generation failed");
        return;
    }
    previous_time = current_time;
    // If no new audio samples have been received since last time, don't bother
    // running the network model.
    if (how_many_new_slices == 0)
    {
        //LOG("[lk added]how_many_new_slices is 0\n");
        return;
    }

    // Copy feature buffer to input tensor
    for (int i = 0; i < kFeatureElementCount; i++)
    {
        model_input_buffer[i] = feature_buffer[i];
    }

    // Run the model on the spectrogram input and make sure it succeeds.
    TfLiteStatus invoke_status = interpreter->Invoke();
    if (invoke_status != kTfLiteOk)
    {
        TF_LITE_REPORT_ERROR(error_reporter, "Invoke failed");
        return;
    }

    // Obtain a pointer to the output tensor
    TfLiteTensor *output = interpreter->output(0);
    // Determine whether a command was recognized based on the output of inference
    const char *found_command = nullptr;
    uint8_t score = 0;
    bool is_new_command = false;
    TfLiteStatus process_status = recognizer->ProcessLatestResults(
        output, current_time, &found_command, &score, &is_new_command);
    if (process_status != kTfLiteOk)
    {
        TF_LITE_REPORT_ERROR(error_reporter,
                             "RecognizeCommands::ProcessLatestResults() failed");
        return;
    }
    // Do something based on the recognized command. The default implementation
    // just prints to the error console, but you should replace this with your
    // own function for a real application.
    RespondToCommand(error_reporter, current_time, found_command, score,
                     is_new_command);
}
#if 0
void test_setup() {
  // Set up logging.
  tflite::MicroErrorReporter micro_error_reporter;

//  RegisterDebugLogCallback(callback);
  // Map the model into a usable data structure. This doesn't involve any
  // copying or parsing, it's a very lightweight operation.
  const tflite::Model* model = ::tflite::GetModel(g_model);
  if (model->version() != TFLITE_SCHEMA_VERSION) {
    TF_LITE_REPORT_ERROR(&micro_error_reporter,
                         "Model provided is schema version %d not equal "
                         "to supported version %d.\n",
                         model->version(), TFLITE_SCHEMA_VERSION);
  }

  // Pull in only the operation implementations we need.
  // This relies on a complete list of all the ops needed by this graph.
  // An easier approach is to just use the AllOpsResolver, but this will
  // incur some penalty in code space for op implementations that are not
  // needed by this graph.
  //
  // tflite::AllOpsResolver resolver;
  tflite::MicroMutableOpResolver<4> micro_op_resolver;
  micro_op_resolver.AddDepthwiseConv2D();
  micro_op_resolver.AddFullyConnected();
  micro_op_resolver.AddReshape();
  micro_op_resolver.AddSoftmax();

  // Create an area of memory to use for input, output, and intermediate arrays.
  const int tensor_arena_size = 10 * 1024;
  uint8_t tensor_arena[tensor_arena_size];

  // Build an interpreter to run the model with.
  tflite::MicroInterpreter interpreter(model, micro_op_resolver, tensor_arena,
                                       tensor_arena_size,
                                       &micro_error_reporter);
  interpreter.AllocateTensors();

  // Get information about the memory area to use for the model's input.
  TfLiteTensor* input = interpreter.input(0);

  // Make sure the input has the properties we expect.
  TF_LITE_MICRO_EXPECT_NE(nullptr, input);
  TF_LITE_MICRO_EXPECT_EQ(2, input->dims->size);
  TF_LITE_MICRO_EXPECT_EQ(1, input->dims->data[0]);
  TF_LITE_MICRO_EXPECT_EQ(1960, input->dims->data[1]);
  TF_LITE_MICRO_EXPECT_EQ(kTfLiteInt8, input->type);

  // Copy a spectrogram created from a .wav audio file of someone saying "Yes",
  // into the memory area used for the input.
  const int8_t* yes_features_data = g_yes_micro_f2e59fea_nohash_1_data;
  for (size_t i = 0; i < input->bytes; ++i) {
    input->data.int8[i] = yes_features_data[i];
  }

  // Run the model on this input and make sure it succeeds.
  TfLiteStatus invoke_status = interpreter.Invoke();
  if (invoke_status != kTfLiteOk) {
    TF_LITE_REPORT_ERROR(&micro_error_reporter, "Invoke failed\n");
  }
  TF_LITE_MICRO_EXPECT_EQ(kTfLiteOk, invoke_status);

  // Get the output from the model, and make sure it's the expected size and
  // type.
  TfLiteTensor* output = interpreter.output(0);
  TF_LITE_MICRO_EXPECT_EQ(2, output->dims->size);
  TF_LITE_MICRO_EXPECT_EQ(1, output->dims->data[0]);
  TF_LITE_MICRO_EXPECT_EQ(4, output->dims->data[1]);
  TF_LITE_MICRO_EXPECT_EQ(kTfLiteInt8, output->type);

  // There are four possible classes in the output, each with a score.
  const int kSilenceIndex = 0;
  const int kUnknownIndex = 1;
  const int kYesIndex = 2;
  const int kNoIndex = 3;

  // Make sure that the expected "Yes" score is higher than the other classes.
  uint8_t silence_score = output->data.int8[kSilenceIndex] + 128;
  uint8_t unknown_score = output->data.int8[kUnknownIndex] + 128;
  uint8_t yes_score = output->data.int8[kYesIndex] + 128;
  uint8_t no_score = output->data.int8[kNoIndex] + 128;
  TF_LITE_MICRO_EXPECT_GT(yes_score, silence_score);
  TF_LITE_MICRO_EXPECT_GT(yes_score, unknown_score);
  TF_LITE_MICRO_EXPECT_GT(yes_score, no_score);

  LOG("silence_score: %d, unknown_score: %d, yes_score: %d, no_score: %d\n", 
        silence_score, unknown_score, yes_score, no_score);
  // Now test with a different input, from a recording of "No".
  const int8_t* no_features_data = g_no_micro_f9643d42_nohash_4_data;
  for (size_t i = 0; i < input->bytes; ++i) {
    input->data.int8[i] = no_features_data[i];
  }

  // Run the model on this "No" input.
  invoke_status = interpreter.Invoke();
  if (invoke_status != kTfLiteOk) {
    TF_LITE_REPORT_ERROR(&micro_error_reporter, "Invoke failed\n");
  }
  TF_LITE_MICRO_EXPECT_EQ(kTfLiteOk, invoke_status);

  // Get the output from the model, and make sure it's the expected size and
  // type.
  output = interpreter.output(0);
  TF_LITE_MICRO_EXPECT_EQ(2, output->dims->size);
  TF_LITE_MICRO_EXPECT_EQ(1, output->dims->data[0]);
  TF_LITE_MICRO_EXPECT_EQ(4, output->dims->data[1]);
  TF_LITE_MICRO_EXPECT_EQ(kTfLiteInt8, output->type);

  // Make sure that the expected "No" score is higher than the other classes.
  silence_score = output->data.int8[kSilenceIndex] + 128;
  unknown_score = output->data.int8[kUnknownIndex] + 128;
  yes_score = output->data.int8[kYesIndex] + 128;
  no_score = output->data.int8[kNoIndex] + 128;
  TF_LITE_MICRO_EXPECT_GT(no_score, silence_score);
  TF_LITE_MICRO_EXPECT_GT(no_score, unknown_score);
  TF_LITE_MICRO_EXPECT_GT(no_score, yes_score);

  TF_LITE_REPORT_ERROR(&micro_error_reporter, "Ran successfully\n");
}
#endif