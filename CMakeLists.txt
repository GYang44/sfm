cmake_minimum_required (VERSION 3.0)

set(CMAKE_CXX_COMPILER "/usr/bin/g++")
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall")

link_directories(
	/usr/lib
	/usr/local/lib
)

include_directories(
	/usr/include
	/usr/local/include
	/usr/include/eigen3
	/usr/local/include/opencv4
)

set(projectName "sfmOut")

file(GLOB_RECURSE SRCS ./src/*.cpp ./src/*.hpp ./src/*.c ./src/*.h)

project (${projectName})

add_executable(${projectName} ${SRCS})

target_link_libraries(${projectName}
	-lopencv_aruco
	-lopencv_bgsegm
	-lopencv_bioinspired
	-lopencv_calib3d
	-lopencv_ccalib
	-lopencv_core
	-lopencv_cudaarithm
	-lopencv_cudabgsegm
	-lopencv_cudacodec
	-lopencv_cudafeatures2d
	-lopencv_cudafilters
	-lopencv_cudaimgproc
	-lopencv_cudalegacy
	-lopencv_cudaobjdetect
	-lopencv_cudaoptflow
	-lopencv_cudastereo
	-lopencv_cudawarping
	-lopencv_cudev
	-lopencv_datasets
	-lopencv_dnn
	-lopencv_dpm
	-lopencv_face
	-lopencv_features2d
	-lopencv_flann
	-lopencv_fuzzy
	#-lopencv_hdf
	-lopencv_highgui
	-lopencv_imgcodecs
	-lopencv_imgproc
	-lopencv_line_descriptor
	-lopencv_ml
	-lopencv_objdetect
	-lopencv_optflow
	-lopencv_phase_unwrapping
	-lopencv_photo
	-lopencv_plot
	-lopencv_reg
	-lopencv_rgbd
	-lopencv_saliency
	-lopencv_sfm
	-lopencv_shape
	-lopencv_stereo
	-lopencv_stitching
	-lopencv_structured_light
	-lopencv_superres
	-lopencv_surface_matching
	-lopencv_text
	-lopencv_tracking
	-lopencv_videoio
	-lopencv_video
	-lopencv_videostab
	#-lopencv_viz
	-lopencv_xfeatures2d
	-lopencv_ximgproc
	-lopencv_xobjdetect
	-lopencv_xphoto

	-larmadillo

	-lGL
	-lglut

	-lpthread
)
