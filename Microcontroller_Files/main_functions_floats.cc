/* Copyright 2018 The TensorFlow Authors. All Rights Reserved.

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

#include "tensorflow/lite/experimental/micro/examples/micro_speech/main_functions.h"
//#include "tensorflow/lite/experimental/micro/examples/micro_speech/command_responder.h"
#include "tensorflow/lite/experimental/micro/examples/micro_speech/feature_provider.h"
#include "tensorflow/lite/experimental/micro/examples/micro_speech/micro_features/tiny_conv_micro_features_model_data.h"
#include "tensorflow/lite/experimental/micro/examples/micro_speech/recognize_commands.h"
#include "tensorflow/lite/experimental/micro/kernels/micro_ops.h"
#include "tensorflow/lite/experimental/micro/micro_error_reporter.h"
#include "tensorflow/lite/experimental/micro/micro_interpreter.h"
#include "tensorflow/lite/experimental/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/version.h"
#include "tensorflow/lite/experimental/micro/examples/micro_speech/micro_features/no_micro_features_data.h"
#include "tensorflow/lite/experimental/micro/examples/micro_speech/micro_features/yes_micro_features_data.h"




// Globals, used for compatibility with Arduino-style sketches.
namespace {
tflite::ErrorReporter* error_reporter = nullptr;
const tflite::Model* model = nullptr;
//tflite::MicroInterpreter* interpreter = nullptr;
TfLiteTensor* model_input = nullptr;
FeatureProvider* feature_provider = nullptr;
RecognizeCommands* recognizer = nullptr;
int32_t previous_time = 0;

// Create an area of memory to use for input, output, and intermediate arrays.
// The size of this will depend on the model you're using, and may need to be
// determined by experimentation.
constexpr int kTensorArenaSize = 200 * 1024;
uint8_t tensor_arena[kTensorArenaSize];
}  // namespace


void setup() {
  // Set up logging. Google style is to avoid globals or statics because of
  // lifetime uncertainty, but since this has a trivial destructor it's okay.
  // NOLINTNEXTLINE(runtime-global-variables)
  static tflite::MicroErrorReporter micro_error_reporter;
  error_reporter = &micro_error_reporter;

  // Map the model into a usable data structure. This doesn't involve any
  // copying or parsing, it's a very lightweight operation.
  model = tflite::GetModel(g_tiny_conv_micro_features_model_data);
  if (model->version() != TFLITE_SCHEMA_VERSION) {
    error_reporter->Report(
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
 //  tflite::ops::micro::AllOpsResolver resolver;
  // NOLINTNEXTLINE(runtime-global-variables)
  static tflite::MicroMutableOpResolver micro_mutable_op_resolver;

  micro_mutable_op_resolver.AddBuiltin(
      tflite::BuiltinOperator_QUANTIZE,
      tflite::ops::micro::Register_QUANTIZE());


  micro_mutable_op_resolver.AddBuiltin(
      tflite::BuiltinOperator_DEPTHWISE_CONV_2D,
      tflite::ops::micro::Register_DEPTHWISE_CONV_2D(), 1, 3);

  micro_mutable_op_resolver.AddBuiltin(
      tflite::BuiltinOperator_FULLY_CONNECTED,
      tflite::ops::micro::Register_FULLY_CONNECTED(),1 , 4);

  micro_mutable_op_resolver.AddBuiltin(
      tflite::BuiltinOperator_SOFTMAX,
      tflite::ops::micro::Register_SOFTMAX(), 1, 2);



  // micro_mutable_op_resolver.AddBuiltin(
 //     tflite::BuiltinOperator_MAX_POOL_2D,
 //     tflite::ops::micro::Register_MAX_POOL_2D());     

   micro_mutable_op_resolver.AddBuiltin(
      tflite::BuiltinOperator_CONV_2D,
      tflite::ops::micro::Register_CONV_2D(), 1, 3);  

   micro_mutable_op_resolver.AddBuiltin(
      tflite::BuiltinOperator_AVERAGE_POOL_2D,
      tflite::ops::micro::Register_AVERAGE_POOL_2D(), 1, 2);   

  micro_mutable_op_resolver.AddBuiltin(
      tflite::BuiltinOperator_RELU,
      tflite::ops::micro::Register_RELU()); 




  micro_mutable_op_resolver.AddBuiltin(
      tflite::BuiltinOperator_DEQUANTIZE,
      tflite::ops::micro::Register_DEQUANTIZE(), 1, 2); 



  // Build an interpreter to run the model with.
 // static tflite::MicroInterpreter static_interpreter(
 //     model, micro_mutable_op_resolver, tensor_arena, kTensorArenaSize,
 //     error_reporter);
 // interpreter = &static_interpreter;

// Bringing interpreter from micro_speech

  // Build an interpreter to run the model with.
  tflite::MicroInterpreter interpreter(model, micro_mutable_op_resolver,
                                       tensor_arena, kTensorArenaSize,
                                       error_reporter);


  // Allocate memory from the tensor_arena for the model's tensors.
  TfLiteStatus allocate_status = interpreter.AllocateTensors();

  if (allocate_status != kTfLiteOk) {
    error_reporter->Report("AllocateTensors() failed");
    return;
  }

  // Get information about the memory area to use for the model's input.
  model_input = interpreter.input(0);
  error_reporter->Report("model dim size=%d", model_input->dims->size);
  error_reporter->Report("model dim data0=%d", model_input->dims->data[0]);
  error_reporter->Report("model dim data1=%d", model_input->dims->data[1]);
  error_reporter->Report("model dim data2=%d", model_input->dims->data[2]);
  error_reporter->Report("model type=%d", model_input->type);
  error_reporter->Report("model input=%d", model_input->data.f[0]);

  if ((model_input->dims->size != 4) || (model_input->dims->data[0] != 1) ||
      (model_input->dims->data[1] != kFeatureSliceCount) ||
      (model_input->dims->data[2] != kFeatureSliceSize) ||
      (model_input->type != kTfLiteFloat32)) {
    error_reporter->Report("Bad input tensor parameters in model");
    return;
  }


// Copy a spectrogram created from a .wav audio file 
  // into the memory area used for the input.
  
  
  //const uint8_t* features_data = g_yes_micro_f2e59fea_nohash_1_data;
  
  


  const float* features_data = g_yes_micro_f2e59fea_nohash_1_data;
  error_reporter->Report("getting input data");
  //const uint8_t* features_data = g_no_micro_f9643d42_nohash_4_data;
  //error_reporter->Report("bytes: %d", model_input->bytes)
  for (int i = 0; i < model_input->bytes; ++i) {
    model_input->data.f[i] = features_data[i];
  }

  error_reporter->Report("started invoking");
  // Run the model on this input and make sure it succeeds.
  TfLiteStatus invoke_status = interpreter.Invoke();
  error_reporter->Report("finished invoking");
  if (invoke_status != kTfLiteOk) {
    error_reporter->Report("Invoke failed\n");
  }
  
  // Get the output from the model, and make sure it's the expected size and
  // type.
  TfLiteTensor* output = interpreter.output(0);
  error_reporter->Report("output type=%d", output->type);
  error_reporter->Report("output: %f", output->data.f[0]);
    error_reporter->Report("output: %d", output->data.uint8[0]);
  error_reporter->Report("output: %d", output->quantization);

  // There are four possible classes in the output, each with a score.
  const int kSilenceIndex = 0;
  const int kUnknownIndex = 1;
  const int kFootStepsIndex = 2;
  const int kGlassBreakingIndex = 3;
  //const int kDoorKnockIndex = 4;
 // const int kClappingIndex = 5;
 // const int kDogIndex = 6;
 // const int kClockAlarmIndex = 7;

  // Make sure that the expected "Yes" score is higher than the other classes.
  float silence_score = output->data.f[kSilenceIndex];
  float unknown_score = output->data.f[kUnknownIndex];
  float footsteps_score = output->data.f[kFootStepsIndex];
  float glassbreaking_score = output->data.f[kGlassBreakingIndex];
  //uint8_t doorknock_score = output->data.uint8[kDoorKnockIndex];
  //uint8_t clapping_score = output->data.uint8[kClappingIndex];
  //uint8_t dog_score = output->data.uint8[kDogIndex];
  //uint8_t clock_alarm_score = output->data.uint8[kClockAlarmIndex];

  //error_reporter->Report("glass=%d", output->type); 
//error_reporter->Report("Houston Heard %s (%d) @%dms", found_command, score,
//                           current_time);

//error_reporter->Report("Softmax: silence=%d, unknown=%d, footsteps=%d, glassbreaking=%d, door_knock=%d, clapping=%d, dog=%d, clock_alarm=%d", 
//silence_score, unknown_score, footsteps_score, glassbreaking_score, doorknock_score, clapping_score, dog_score, clock_alarm_score);

float norm_silence = silence_score * 100;
float norm_unknown = unknown_score * 100;
float norm_footsteps = footsteps_score  * 100;
float norm_glassbreaking = glassbreaking_score * 100;


// 'footsteps', 'door_knock', 'clock_alarm', 'glass breaking'
error_reporter->Report("Softmax: footsteps= %f , door_knock= %f , clock_alarm= %f , glass_breaking= %f", 
norm_silence, norm_unknown, norm_footsteps, norm_glassbreaking);

}


//void loop() {

// nothing to do here

//}

 


