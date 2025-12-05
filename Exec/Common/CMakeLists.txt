cmake_minimum_required(VERSION 3.8)

project(commonAPI)

# 设置该工程采用C++ 17标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(BUILD_SHARED_LIBS ON)

add_definitions(-DCOMMON_LIB)

message("lib config source cmake:${CMAKE_CURRENT_LIST_DIR}")
# 设置包含目录和链接库目录
include_directories(${CMAKE_CURRENT_LIST_DIR}/../../sys/ThirdParty.win/include)
include_directories(${CMAKE_CURRENT_LIST_DIR}/)
include_directories(${CMAKE_CURRENT_LIST_DIR}/include)
include_directories(${CMAKE_CURRENT_LIST_DIR}/../UdmMessage/include)
include_directories(${CMAKE_CURRENT_LIST_DIR}/../log/include)
aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/ COMMON_FILE)
if(DEFINED VLD_DEBUG)
	include_directories(${VLD_INCLUDE_PATH})
	link_directories(${VLD_LIB_PATH})
endif()


set(ALL_SOURCE_FILES 
	${COMMON_FILE}
	)
message("lib common source files:${ALL_SOURCE_FILES}")	

if(CMAKE_BUILD_TYPE STREQUAL "Release")
	foreach(path ${RELEASE_LIBRARY_PATH})
		link_directories(${path})
	endforeach()
elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
	foreach(path ${DEBUG_LIBRARY_PATH})
		link_directories(${path})
	endforeach()
endif()
set(HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
file(GLOB_RECURSE HEADERS ${HEADER_DIR}/*.h)
add_custom_target(OutputHeaders ALL SOURCES ${HEADERS})
set(LIBRARY_OUTPUT_PATH "${CMAKE_CURRENT_LIST_DIR}/../../sys/lib")
add_library(${PROJECT_NAME} SHARED ${ALL_SOURCE_FILES})
add_dependencies(${PROJECT_NAME} UdmMessageAPI_static)

target_link_libraries(${PROJECT_NAME}  jsoncpp UdmMessageAPI_static LogAPI)
if(DEFINED VLD_DEBUG)
target_link_libraries(${PROJECT_NAME} vld)
endif()


install(TARGETS ${PROJECT_NAME}
		LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib)

install(FILES ${CMAKE_CURRENT_LIST_DIR}/include/*.h DESTINATION include)