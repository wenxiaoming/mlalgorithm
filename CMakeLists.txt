cmake_minimum_required(VERSION 2.6)

project(MLAlgorithm CXX)

set(PROJ_EXEC_NAME mlalgorithm)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

file(GLOB_RECURSE HEADERS "${PROJECT_SOURCE_DIR}/*/*.h")
include_directories(${HEADERS})

file(GLOB_RECURSE SRCS "${PROJECT_SOURCE_DIR}/apriori/*.cpp"
                        "${PROJECT_SOURCE_DIR}/cart/*.cpp"
                        "${PROJECT_SOURCE_DIR}/decisiontree/*.cpp"
                        "${PROJECT_SOURCE_DIR}/fpgrowth/*.cpp"
                        "${PROJECT_SOURCE_DIR}/knn/*.cpp"
                        "${PROJECT_SOURCE_DIR}/naivebayes/*.cpp"
                        "${PROJECT_SOURCE_DIR}/svm/*.cpp")

file(GLOB_RECURSE MAIN_SRCS "${PROJECT_SOURCE_DIR}/mlalgorithm_test.cpp")

message(STATUS "SRCS: ${SRCS}")
message(STATUS "HEADERS: ${HEADERS}")
message(STATUS "MAIN_SRCS: ${MAIN_SRCS}")

add_executable(${PROJ_EXEC_NAME} ${SRCS} ${MAIN_SRCS})
