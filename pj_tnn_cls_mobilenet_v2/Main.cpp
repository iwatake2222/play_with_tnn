/*** Include ***/
/* for general */
#include <stdio.h>
#include <algorithm>
#include <fstream>
#include <functional>
#include <memory>
#include <sstream>
#include <vector>
#include <chrono>

/* for OpenCV */
#include <opencv2/opencv.hpp>

/* for TNN */
#include "tnn/core/macro.h"
#include "tnn/core/tnn.h"
#include "tnn/utils/blob_converter.h"


/*** Macro ***/
/* Model parameters */
#define MODEL_NAME         RESOURCE_DIR"/model/mobilenetv2-1.0.opt.tnnmodel"
#define MODEL_PROTO_NAME   RESOURCE_DIR"/model/mobilenetv2-1.0.opt.tnnproto"
#define IMAGE_NAME   RESOURCE_DIR"/parrot.jpg"

/* Settings */
#define LOOP_NUM_FOR_TIME_MEASUREMENT 100

/**** Macro Funtions ****/
#define CHECK(x)                              \
  if (!(x)) {                                                \
    fprintf(stderr, "Error at %s:%d\n", __FILE__, __LINE__); \
    exit(1);                                                 \
  }

int main(int argc, const char* argv[])
{
	/*** Initialize ***/
	/* Create interpreter */
	TNN_NS::Status status;
	TNN_NS::ModelConfig config;
	config.model_type = TNN_NS::MODEL_TYPE_TNN;
	config.params = {MODEL_PROTO_NAME, MODEL_NAME};
	auto net = std::make_shared<TNN_NS::TNN>();
	status = net->Init(config);
	CHECK(status == TNN_NS::TNN_OK);

	TNN_NS::NetworkConfig network_config;
	network_config.library_path = {""};
	network_config.device_type  = TNN_NS::DEVICE_ARM;
	std::vector<int> nchw = {1, 3, 224, 224};
	TNN_NS::InputShapesMap shapeMap;
	shapeMap.insert(std::pair<std::string, TNN_NS::DimsVector>("input", nchw));
	auto instance = net->CreateInst(network_config, status, shapeMap);
	CHECK(status == TNN_NS::TNN_OK);
	CHECK(instance != NULL);

	auto input_mat = std::make_shared<TNN_NS::Mat>(TNN_NS::DEVICE_ARM, TNN_NS::N8UC3, nchw);
	CHECK(input_mat != NULL);
	CHECK(input_mat->GetData() != NULL);
	TNN_NS::MatConvertParam input_cvt_param;
	input_cvt_param.scale = {1.0 / (255 * 0.229), 1.0 / (255 * 0.224), 1.0 / (255 * 0.225), 0.0};
	input_cvt_param.bias  = {-0.485 / 0.229, -0.456 / 0.224, -0.406 / 0.225, 0.0};
	status = instance->SetInputMat(input_mat, input_cvt_param);
	CHECK(status != TNN_NS::TNN_OK);

	status = instance->ForwardAsync(nullptr);
	CHECK(status != TNN_NS::TNN_OK);

	std::shared_ptr<TNN_NS::Mat> output_mat_scores = nullptr;
	status = instance->GetOutputMat(output_mat_scores);
	CHECK(status != TNN_NS::TNN_OK);

	// std::shared_ptr<MNN::Interpreter> net(MNN::Interpreter::createFromFile(MODEL_NAME));
	// MNN::ScheduleConfig scheduleConfig;
	// scheduleConfig.type  = MNN_FORWARD_AUTO;
	// scheduleConfig.numThread = 4;
	// // BackendConfig bnconfig;
	// // bnconfig.precision = BackendConfig::Precision_Low;
	// // config.backendConfig = &bnconfig;
	// auto session = net->createSession(scheduleConfig);

	// /* Get model information */
	// auto input = net->getSessionInput(session, NULL);
	// int modelChannel = input->channel();
	// int modelHeight  = input->height();
	// int modelWidth   = input->width();
	// printf("model input size: widgh = %d , height = %d, channel = %d\n", modelWidth, modelHeight, modelChannel);

	// /***** Process for each frame *****/
	// /*** Read image ***/
	// cv::Mat originalImage = cv::imread(IMAGE_NAME);
	// int imageWidth = originalImage.size[1];
	// int imageHeight = originalImage.size[0];
	// printf("image size: width = %d, height = %d\n", imageWidth, imageHeight);
	
	// /*** Pre process (resize, colorconversion, normalize) ***/
	// MNN::CV::ImageProcess::Config imageProcessconfig;
	// imageProcessconfig.filterType = MNN::CV::BILINEAR;
	// float mean[3]     = {127.5f, 127.5f, 127.5f};
	// float normals[3] = {0.00785f, 0.00785f, 0.00785f};
	// std::memcpy(imageProcessconfig.mean, mean, sizeof(mean));
	// std::memcpy(imageProcessconfig.normal, normals, sizeof(normals));
	// imageProcessconfig.sourceFormat = MNN::CV::BGR;
	// imageProcessconfig.destFormat   = MNN::CV::BGR;

	// MNN::CV::Matrix trans;
	// trans.setScale((float)imageWidth/modelWidth, (float)imageHeight/modelHeight);

	// std::shared_ptr<MNN::CV::ImageProcess> pretreat(MNN::CV::ImageProcess::create(imageProcessconfig));
	// pretreat->setMatrix(trans);
	// pretreat->convert((uint8_t*)originalImage.data, imageWidth, imageHeight, 0, input);

	// /*** Inference ***/
	// net->runSession(session);

	// /*** Post process ***/
	// /* Retreive results */
	// auto output = net->getSessionOutput(session, NULL);
	// auto dimType = output->getDimensionType();

	// std::shared_ptr<MNN::Tensor> outputUser(new MNN::Tensor(output, dimType));
	// output->copyToHostTensor(outputUser.get());
	// auto size = outputUser->elementSize();
	// auto type = outputUser->getType();
	// printf("output size: size = %d\n", size);
	
	// std::vector<std::pair<int, float>> tempValues(size);
	// if (type.code == halide_type_float) {
	// 	auto values = outputUser->host<float>();
	// 	for (int i = 0; i < size; ++i) {
	// 		tempValues[i] = std::make_pair(i, values[i]);
	// 	}
	// } else if (type.code == halide_type_uint && type.bytes() == 1) {
	// 	auto values = outputUser->host<uint8_t>();
	// 	for (int i = 0; i < size; ++i) {
	// 		tempValues[i] = std::make_pair(i, values[i]);
	// 	}
	// } else {
	// 	printf("should not reach here\n");
	// }

	// /* Find the highest score */
	// std::sort(tempValues.begin(), tempValues.end(), [](std::pair<int, float> a, std::pair<int, float> b) { return a.second > b.second; });
	// int length = size > 10 ? 10 : size;
	// for (int i = 0; i < length; ++i) {
	// 	printf("%d: %f\n", tempValues[i].first, tempValues[i].second);
	// }


	// /*** (Optional) Measure inference time ***/
	// const auto& t0 = std::chrono::steady_clock::now();
	// for (int i = 0; i < LOOP_NUM_FOR_TIME_MEASUREMENT; i++) {
	// 	net->runSession(session);
	// }
	// const auto& t1 = std::chrono::steady_clock::now();
	// std::chrono::duration<double> timeSpan = t1 - t0;
	// printf("Inference time = %f [msec]\n", timeSpan.count() * 1000.0 / LOOP_NUM_FOR_TIME_MEASUREMENT);

	// /*** Finalize ***/
	// net->releaseSession(session);

	return 0;
}
