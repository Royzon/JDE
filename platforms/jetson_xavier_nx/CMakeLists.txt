cmake_minimum_required(VERSION 3.5.1)

project(jde)

find_package(CUDA REQUIRED)
find_package(OpenCV REQUIRED)
# set(CUDA_NVCC_FLAGS -gencode arch=compute_72,code=sm_72;-G;-g)
set(CUDA_NVCC_PLAGS ${CUDA_NVCC_PLAGS};-std=c++11;-g;-G;-gencode;arch=compute_72;code=sm_72)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -Wall -std=c++11 -fopenmp -O3 -g")

message(${PROJECT_SOURCE_DIR}/src)

include_directories(
    /usr/local/cuda/include
    /usr/local/cuda/samples/common/inc
    ~/DL_frame/tensorRT/TensorRT-7.0.0.11/include
    /usr/include/aarch64-linux-gnu
    ${OpenCV_INCLUDE_DIRS}
    ${PROJECT_SOURCE_DIR}/src
)

link_directories(
    /usr/local/cuda/lib64
    /usr/local/cuda/targets/aarch64-linux/lib
    ~/DL_frame/tensorRT/TensorRT-7.0.0.11/lib
    /usr/lib/aarch64-linux-gnu
    /usr/local/lib64
    /usr/local/lib
    ${PROJECT_SOURCE_DIR}/build
)

file(GLOB_RECURSE libsrcs ${PROJECT_SOURCE_DIR}/src/*.cpp)
list(FILTER libsrcs EXCLUDE REGEX "mot-test")
list(FILTER libsrcs EXCLUDE REGEX "jde-test")
list(FILTER libsrcs EXCLUDE REGEX "jdecoderv2-test")
list(FILTER libsrcs EXCLUDE REGEX "preprocess-test")

file(GLOB_RECURSE libgpusrcs ${PROJECT_SOURCE_DIR}/src/*.cu)
cuda_add_library(jdecoder SHARED ${libgpusrcs})
target_link_libraries(jdecoder -lnvinfer -lnvinfer_plugin -lcudart)

add_library(mot SHARED ${libsrcs})
add_dependencies(mot jdecoder)
target_link_libraries(
    mot
    -ljdecoder
    -lnvinfer
    -lnvinfer_plugin
    -lnvonnxparser
    -lcudart
    -lcublas
    -lcudnn
    -lpthread
    -ldl
    ${OpenCV_LIBS}
)

add_executable(jde-test ${PROJECT_SOURCE_DIR}/src/jde-test.cpp ${libsrcs})
add_dependencies(jde-test jdecoder)
target_link_libraries(
    jde-test
    -ljdecoder
    -lnvinfer
    -lnvinfer_plugin
    -lnvonnxparser
    -lcudart
    -lcublas
    -lcudnn
    -lpthread
    -ldl
    ${OpenCV_LIBS}
)

add_executable(mot-test ${PROJECT_SOURCE_DIR}/src/mot-test.cpp)
add_dependencies(mot-test mot)
target_link_libraries(
    mot-test
    -lmot
    ${OpenCV_LIBS}
)

add_executable(jdecoderv2-test ${PROJECT_SOURCE_DIR}/src/jdecoderv2-test.cpp)
add_dependencies(jdecoderv2-test jdecoder)
target_link_libraries(
    jdecoderv2-test
    -ljdecoder
    -lnvinfer
    -lcudart
    ${OpenCV_LIBS})

add_executable(preprocess-test ${PROJECT_SOURCE_DIR}/src/preprocess-test.cpp ${PROJECT_SOURCE_DIR}/src/utils.cpp)
target_link_libraries(
    preprocess-test
    -lnvinfer
    -lcudart
    ${OpenCV_LIBS})