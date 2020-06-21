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
#define MODEL_PROTO_NAME   RESOURCE_DIR"/model/mobilenetv2-1.0.opt.tnnproto"
#define MODEL_NAME         RESOURCE_DIR"/model/mobilenetv2-1.0.opt.tnnmodel"
#define MODEL_WIDTH        224
#define MODEL_HEIGHT       224
#define MODEL_CHANNEL      3
#define LABEL_NAME         RESOURCE_DIR"/model/imagenet_labels.txt"

#ifdef CPU_IS_ARM
#define DEVICE_TYPE TNN_NS::DEVICE_ARM
#else
#define DEVICE_TYPE TNN_NS::DEVICE_NAIVE
#endif

/* Settings */
#define IMAGE_NAME   RESOURCE_DIR"/parrot.jpg"
#define LOOP_NUM_FOR_TIME_MEASUREMENT 100

/**** Macro Funtions ****/
#define CHECK(x)                              \
  if (!(x)) {                                                \
	fprintf(stderr, "Error at %s:%d\n", __FILE__, __LINE__); \
	exit(1);                                                 \
  }

static void readLabel(const char* filename, std::vector<std::string> & labels)
{
	std::ifstream ifs(filename);
	if (ifs.fail()) {
		printf("failed to read %s\n", filename);
		return;
	}
	std::string str;
	while (getline(ifs, str)) {
		labels.push_back(str);
	}
}

static std::string fdLoadFile(std::string path) {
	std::ifstream file(path, std::ios::in);
	if (file.is_open()) {
		file.seekg(0, file.end);
		int size      = file.tellg();
		char* content = new char[size];

		file.seekg(0, file.beg);
		file.read(content, size);
		std::string fileContent;
		fileContent.assign(content, size);
		delete[] content;
		file.close();
		return fileContent;
	} else {
		return "";
	}
}

int main(int argc, const char* argv[])
{
	/*** Initialize ***/
	/* read label */
	std::vector<std::string> labels;
	readLabel(LABEL_NAME, labels);

	/* Create interpreter */
	TNN_NS::Status status;
	TNN_NS::ModelConfig config;
	config.model_type = TNN_NS::MODEL_TYPE_TNN;
	const std::string proto = fdLoadFile(MODEL_PROTO_NAME);
	const std::string model = fdLoadFile(MODEL_NAME);
	config.params = {proto, model};
	auto net = std::make_shared<TNN_NS::TNN>();
	status = net->Init(config);
	// printf("%s\n", status.description().c_str());
	CHECK(status == TNN_NS::TNN_OK);

	TNN_NS::NetworkConfig network_config;
	network_config.library_path = {""};
	network_config.device_type  = DEVICE_TYPE;
	std::vector<int> nchw = {1, MODEL_CHANNEL, MODEL_HEIGHT, MODEL_WIDTH};
	TNN_NS::InputShapesMap shapeMap;
	shapeMap.insert(std::pair<std::string, TNN_NS::DimsVector>("input", nchw));
	auto instance = net->CreateInst(network_config, status, shapeMap);
	instance->SetCpuNumThreads(4);
	// printf("%s\n", status.description().c_str());
	CHECK(status == TNN_NS::TNN_OK);
	CHECK(instance != NULL);

	/***** Process for each frame *****/
	/*** Read image ***/
	cv::Mat originalImage = cv::imread(IMAGE_NAME);
	int imageWidth = originalImage.size[1];
	int imageHeight = originalImage.size[0];
	printf("image size: width = %d, height = %d\n", imageWidth, imageHeight);

	/*** Pre process ***/
	/* resize, colorconversion */
	cv::Mat inputImage;
	cv::resize(originalImage, inputImage, cv::Size(MODEL_WIDTH, MODEL_HEIGHT));
	cv::cvtColor(inputImage, inputImage, cv::COLOR_BGR2RGB);

	/* normalize */
	auto inputMat = std::make_shared<TNN_NS::Mat>(DEVICE_TYPE, TNN_NS::N8UC3, nchw, (uint8_t*)inputImage.data);
	CHECK(inputMat != NULL);
	TNN_NS::MatConvertParam inputCvtParam;
	inputCvtParam.scale = {1.0 / (255 * 0.229), 1.0 / (255 * 0.224), 1.0 / (255 * 0.225), 0.0};
	inputCvtParam.bias  = {-0.485 / 0.229, -0.456 / 0.224, -0.406 / 0.225, 0.0};
	status = instance->SetInputMat(inputMat, inputCvtParam);
	CHECK(status == TNN_NS::TNN_OK);

	/*** Inference ***/
	status = instance->Forward();
	CHECK(status == TNN_NS::TNN_OK);

	/*** Post process ***/
	/* Retreive results */
	std::shared_ptr<TNN_NS::Mat> outputScoresMat = nullptr;
	status = instance->GetOutputMat(outputScoresMat, tnn::MatConvertParam(), "", DEVICE_TYPE, TNN_NS::NCHW_FLOAT);
	CHECK(status == TNN_NS::TNN_OK);

	int size = outputScoresMat->GetChannel();
	float *outputScoresData = (float*)outputScoresMat->GetData();
	std::vector<std::pair<int, float>> tempValues(size);	// index, score
	for (int i = 0; i < size; ++i) {
		tempValues[i] = std::make_pair(i, outputScoresData[i]);
	}

	/* Find the highest score */
	std::sort(tempValues.begin(), tempValues.end(), [](std::pair<int, float> a, std::pair<int, float> b) { return a.second > b.second; });
	int length = size > 10 ? 10 : size;
	for (int i = 0; i < length; ++i) {
		printf("%s(%d): %f\n", labels[tempValues[i].first].c_str(), tempValues[i].first, tempValues[i].second);
	}

	/*** (Optional) Measure inference time ***/
	const auto& t0 = std::chrono::steady_clock::now();
	for (int i = 0; i < LOOP_NUM_FOR_TIME_MEASUREMENT; i++) {
		instance->Forward();
	}
	const auto& t1 = std::chrono::steady_clock::now();
	std::chrono::duration<double> timeSpan = t1 - t0;
	printf("Inference time = %f [msec]\n", timeSpan.count() * 1000.0 / LOOP_NUM_FOR_TIME_MEASUREMENT);

	/*** Finalize ***/

	return 0;
}
