set(TNN_DIR ${CMAKE_SOURCE_DIR}/../third_party/TNN)
target_include_directories(${PROJECT_NAME} PUBLIC ${TNN_DIR}/include)

set(USE_PREBUILT_TNN on CACHE BOOL "Use Prebuilt TNN? [on/off]")
if(USE_PREBUILT_TNN)
	if(MSVC_VERSION)
		message(FATAL_ERROR "[TNN] unsupported target. ${BUILD_SYSTEM}")
		# file(COPY ${CMAKE_CURRENT_LIST_DIR}/../TNN_prebuilt/x64_windows/VS2017/Debug/MNN.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Debug)
		# file(COPY ${CMAKE_CURRENT_LIST_DIR}/../TNN_prebuilt/x64_windows/VS2017/RelWithDebInfo/MNN.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/RelWithDebInfo)
		# file(COPY ${CMAKE_CURRENT_LIST_DIR}/../TNN_prebuilt/x64_windows/VS2017/Release/MNN.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Release)
		# file(COPY ${CMAKE_CURRENT_LIST_DIR}/../TNN_prebuilt/x64_windows/VS2017/MinSizeRel/MNN.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/MinSizeRel)
		# # file(COPY ${CMAKE_CURRENT_LIST_DIR}/../TNN_prebuilt/x64_windows/MNNd.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
		# if((MSVC_VERSION GREATER_EQUAL 1910) AND (MSVC_VERSION LESS 1920))
		# 	target_link_libraries(${PROJECT_NAME}
		# 		$<$<CONFIG:Debug>:${CMAKE_CURRENT_LIST_DIR}/../TNN_prebuilt/x64_windows/VS2017/Debug/MNN.lib>
		# 		$<$<CONFIG:RelWithDebInfo>:${CMAKE_CURRENT_LIST_DIR}/../TNN_prebuilt/x64_windows/VS2017/RelWithDebInfo/MNN.lib>
		# 		$<$<CONFIG:Release>:${CMAKE_CURRENT_LIST_DIR}/../TNN_prebuilt/x64_windows/VS2017/Release/MNN.lib>
		# 		$<$<CONFIG:MinSizeRel>:${CMAKE_CURRENT_LIST_DIR}/../TNN_prebuilt/x64_windows/VS2017/MinSizeRel/MNN.lib>
		# 	)
		# else()
		# 	message(FATAL_ERROR "[MNN] unsupported MSVC version")
		# endif()
	else()
		target_link_libraries(${ProjectName}
			# $<$<STREQUAL:${BUILD_SYSTEM},x64_windows>:${CMAKE_SOURCE_DIR}/../third_party/TNN_prebuilt/x64_windows/lib/ncnn.lib>
			$<$<STREQUAL:${BUILD_SYSTEM},x64_linux>:${CMAKE_SOURCE_DIR}/../third_party/TNN_prebuilt/x64_linux/libTNN.so>
			$<$<STREQUAL:${BUILD_SYSTEM},armv7>:${CMAKE_SOURCE_DIR}/../third_party/TNN_prebuilt/armv7/libTNN.so>
			$<$<STREQUAL:${BUILD_SYSTEM},aarch64>:${CMAKE_SOURCE_DIR}/../third_party/TNN_prebuilt/aarch64/libTNN.so>
		)

		# note: should use ln -s
		add_custom_target(TNN_LIB_COPY ALL
			COMMAND "cp" "${CMAKE_SOURCE_DIR}/../third_party/TNN_prebuilt/${BUILD_SYSTEM}/libTNN.so" "${CMAKE_CURRENT_BINARY_DIR}/."
			COMMAND "cp" "${CMAKE_CURRENT_BINARY_DIR}/libTNN.so" "${CMAKE_CURRENT_BINARY_DIR}/libTNN.so.0"
			COMMAND "cp" "${CMAKE_CURRENT_BINARY_DIR}/libTNN.so" "${CMAKE_CURRENT_BINARY_DIR}/libTNN.so.0.1.0.0"
		)
	endif()
else()
	if(MSVC_VERSION)
		message(FATAL_ERROR "[TNN] unsupported target. ${BUILD_SYSTEM}")
	else()
		# set(CMAKE_SYSTEM_NAME Linux CACHE STRING "System Name " FORCE)
		set(TNN_TEST_ENABLE OFF CACHE BOOL "Enable Test" FORCE)
		if(${BUILD_SYSTEM} STREQUAL "x64_linux")
			set(TNN_CPU_ENABLE ON CACHE BOOL "Enable Cpu" FORCE)
			set(TNN_X86_ENABLE OFF CACHE BOOL "Enable X86" FORCE)
			set(DEBUG OFF CACHE BOOL "DEBUG" FORCE)
			# set(CMAKE_C_COMPILER /usr/bin/cc CACHE BOOL "CMAKE_C_COMPILER" FORCE)
			# set(CMAKE_CXX_COMPILER /usr/bin/c++ CACHE BOOL "CMAKE_CXX_COMPILER" FORCE)
			set(TNN_ARM_ENABLE OFF CACHE BOOL "Enable Arm" FORCE)
			set(TNN_OPENMP_ENABLE ON CACHE BOOL "Enable OpenMP" FORCE)
			set(TNN_OPENCL_ENABLE OFF CACHE BOOL "Enable OpenCL" FORCE)
			set(TNN_QUANTIZATION_ENABLE OFF CACHE BOOL "Enable Quantization" FORCE)
			set(TNN_UNIT_TEST_ENABLE OFF CACHE BOOL "Enable Test" FORCE)
			set(TNN_BUILD_SHARED ON CACHE BOOL "Enable Shared Library" FORCE)
		elseif(${BUILD_SYSTEM} STREQUAL "aarch64")
			set(TNN_CPU_ENABLE OFF CACHE BOOL "Enable Cpu" FORCE)
			set(TNN_X86_ENABLE OFF CACHE BOOL "Enable X86" FORCE)
			set(DEBUG OFF CACHE BOOL "DEBUG" FORCE)
			# set(CMAKE_C_COMPILER /usr/bin/cc CACHE BOOL "CMAKE_C_COMPILER" FORCE)
			# set(CMAKE_CXX_COMPILER /usr/bin/c++ CACHE BOOL "CMAKE_CXX_COMPILER" FORCE)
			set(TNN_ARM_ENABLE ON CACHE BOOL "Enable Arm" FORCE)
			set(TNN_OPENMP_ENABLE ON CACHE BOOL "Enable OpenMP" FORCE)
			set(TNN_OPENCL_ENABLE OFF CACHE BOOL "Enable OpenCL" FORCE)
			set(TNN_QUANTIZATION_ENABLE OFF CACHE BOOL "Enable Quantization" FORCE)
			set(TNN_UNIT_TEST_ENABLE OFF CACHE BOOL "Enable Test" FORCE)
			set(TNN_BUILD_SHARED ON CACHE BOOL "Enable Shared Library" FORCE)
		elseif(${BUILD_SYSTEM} STREQUAL "armv7")
			set(TNN_CPU_ENABLE OFF CACHE BOOL "Enable Cpu" FORCE)
			set(TNN_X86_ENABLE OFF CACHE BOOL "Enable X86" FORCE)
			set(DEBUG OFF CACHE BOOL "DEBUG" FORCE)
			# set(CMAKE_C_COMPILER /usr/bin/cc CACHE BOOL "CMAKE_C_COMPILER" FORCE)
			# set(CMAKE_CXX_COMPILER /usr/bin/c++ CACHE BOOL "CMAKE_CXX_COMPILER" FORCE)
			set(TNN_ARM_ENABLE ON CACHE BOOL "Enable Arm" FORCE)
			set(TNN_OPENMP_ENABLE ON CACHE BOOL "Enable OpenMP" FORCE)
			set(TNN_OPENCL_ENABLE OFF CACHE BOOL "Enable OpenCL" FORCE)
			set(TNN_QUANTIZATION_ENABLE OFF CACHE BOOL "Enable Quantization" FORCE)
			set(TNN_UNIT_TEST_ENABLE OFF CACHE BOOL "Enable Test" FORCE)
			set(TNN_BUILD_SHARED ON CACHE BOOL "Enable Shared Library" FORCE)
		else()
			message(FATAL_ERROR "[TNN] unsupported target. ${BUILD_SYSTEM}")
		endif()
	endif()

	add_subdirectory(${TNN_DIR} TNN)
	target_link_libraries(${PROJECT_NAME} TNN)
endif()
