cmake_minimum_required(VERSION 3.8)

project(configAPI)

# 设置该工程采用C++ 17标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(BUILD_SHARED_LIBS ON)
set(BUILD_STATIC_LIBS ON)
add_definitions(-DCONFIG_LIB)

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


include_directories(${CMAKE_CURRENT_LIST_DIR}/../Config)
include_directories(${CMAKE_CURRENT_LIST_DIR}/../SerializeConfig/include)
include_directories(${CMAKE_CURRENT_LIST_DIR}/../SerializeConfig/Json)
include_directories(${CMAKE_CURRENT_LIST_DIR}/../Common/include)
if(DEFINED VLD_DEBUG)
	include_directories(${VLD_INCLUDE_PATH})
	link_directories(${VLD_LIB_PATH})
endif()
aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/../Config CONFIG_FILE)

set(ALL_SOURCE_FILES 
	${CONFIG_FILE}
	)
message("lib config source files:${ALL_SOURCE_FILES}")	
	
set(OUT_INCLUDE_FILES ${CMAKE_CURRENT_LIST_DIR}/include/Config.h)
set(LIBRARY_OUTPUT_PATH "${CMAKE_CURRENT_LIST_DIR}/../../sys/lib")
message("lib config out path:${LIBRARY_OUTPUT_PATH}")

add_library(${PROJECT_NAME} SHARED ${ALL_SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} jsoncpp SerializeConfigAPI)
if(DEFINED VLD_DEBUG)
	target_link_libraries(${PROJECT_NAME} vld)
endif()
install(TARGETS ${PROJECT_NAME}
		LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib)
install(FILES ${CMAKE_CURRENT_LIST_DIR}/*.h DESTINATION include)