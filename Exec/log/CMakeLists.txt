cmake_minimum_required(VERSION 3.8)

project(LogAPI)

# 设置该工程采用C++ 17标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(BUILD_SHARED_LIBS ON)
add_definitions(-DLOG_LIB)

message("lib config source cmake:${CMAKE_CURRENT_LIST_DIR}")
# 设置包含目录和链接库目录
include_directories(${CMAKE_CURRENT_LIST_DIR}/../../sys/ThirdParty.win/include)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	foreach(path ${DEBUG_LIBRARY_PATH})
	link_directories(${path})
	endforeach()
else()
	foreach(path ${RELEASE_LIBRARY_PATH})
	link_directories(${path})
	endforeach()
endif()

include_directories(${CMAKE_CURRENT_LIST_DIR}/include)
include_directories(${CMAKE_CURRENT_LIST_DIR}/../Common/include)
aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/ SOURCE_FILE)
if(DEFINED VLD_DEBUG)
	include_directories(${VLD_INCLUDE_PATH})
	link_directories(${VLD_LIB_PATH})
endif()
set(ALL_SOURCE_FILES 
	${SOURCE_FILE}
	)
message("lib common source files:${ALL_SOURCE_FILES}")	

set(HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
file(GLOB_RECURSE HEADERS ${HEADER_DIR}/*.h)
#add_custom_target(OutputHeaders ALL SOURCES ${HEADERS})

set(LIBRARY_OUTPUT_PATH "${CMAKE_CURRENT_LIST_DIR}/../../sys/lib")
message("lib config out path:${LIBRARY_OUTPUT_PATH}")

add_library(${PROJECT_NAME} SHARED ${ALL_SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} logger)
if(DEFINED VLD_DEBUG)
	target_link_libraries(${PROJECT_NAME} vld)
endif()
install(TARGETS ${PROJECT_NAME} 
		LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib)

install(FILES ${CMAKE_CURRENT_LIST_DIR}/include/*.h DESTINATION include)
