cmake_minimum_required(VERSION 3.8)

project(SerializeConfigAPI)

# 设置该工程采用C++ 17标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(BUILD_SHARED_LIBS ON)
set(BUILD_STATIC_LIBS ON)
add_definitions(-DSERIALIZE_LIB)

message("SerializeConfigAPI cmake:${CMAKE_CURRENT_LIST_DIR}")
include_directories(${CMAKE_CURRENT_LIST_DIR}/)
include_directories(${CMAKE_CURRENT_LIST_DIR}/include)
include_directories(${CMAKE_CURRENT_LIST_DIR}/../../sys/ThirdParty.win/include)
if(DEFINED VLD_DEBUG)
	include_directories(${VLD_INCLUDE_PATH})
	link_directories(${VLD_LIB_PATH})
endif()
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	foreach(path ${DEBUG_LIBRARY_PATH})
	link_directories(${path})
	endforeach()
else()
	foreach(path ${RELEASE_LIBRARY_PATH})
	link_directories(${path})
	endforeach()
endif()

aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/Json JSON_FILE)
set(ALL_SOURCE_FILES ${JSON_FILE})
message("SerializeConfigAPI source files:${ALL_SOURCE_FILES}")

set(OUT_INCLUDE_FILES ${CMAKE_CURRENT_LIST_DIR}/include/config.h ${CMAKE_CURRENT_LIST_DIR}/include/Serialization.h)
set(LIBRARY_OUTPUT_PATH "${CMAKE_CURRENT_LIST_DIR}/../../sys/lib")


add_library(${PROJECT_NAME} SHARED ${ALL_SOURCE_FILES})

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	target_link_libraries(${PROJECT_NAME} jsoncpp)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
	target_link_libraries(${PROJECT_NAME} jsoncpp)
endif()

if(DEFINED VLD_DEBUG)
	target_link_libraries(${PROJECT_NAME} vld)
endif()
install(TARGETS ${PROJECT_NAME}
		LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib)

install(FILES ${CMAKE_CURRENT_LIST_DIR}/*.h DESTINATION include)
