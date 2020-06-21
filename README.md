# Play with TNN
Sample projects to use TNN (https://github.com/Tencent/TNN )

## Target environment
- (Windows(MSVC) (x64))
	- Build for Windows failed in my environment
- Linux (x64)
- Linux (armv7) e.g. Raspberry Pi 3,4
- Linux (aarch64) e.g. Jetson Nano
- *Native build only (Cross build is not supported)


## How to build application code
```
cd play_with_tnn/
git submodule init
git submodule update

cd pj_tnn_cls_mobilenet_v2
mkdir build && cd build
cmake ..
make

./main
```

If you use Visual Studio, please use cmake-gui to generate project files.

## How to create pre-built TNN library
pre-built TNN library is stored in third_party/TNN_prebuilt . Please use the following commands if you want to build them by yourself.


### Linux
```
cd pj_tnn_cls_mobilenet_v2
mkdir build && cd build
cmake .. -DUSE_PREBUILT_TNN=off
make

./main
```

# Acknowledgements
- This project includes TNN (https://github.com/Tencent/TNN ) as a submodule
- The models are retrieved from the followings:
	- mobilenetv2-1.0
		- direct link(may not work in the future): https://s3.amazonaws.com/onnx-model-zoo/mobilenet/mobilenetv2-1.0/mobilenetv2-1.0.onnx
		- URL: https://github.com/onnx/models/tree/master/vision/classification/mobilenet
		- URL(version specified): https://github.com/onnx/models/tree/8d50e3f598e6d5c67c7c7253e5a203a26e731a1b/vision/classification/mobilenet
