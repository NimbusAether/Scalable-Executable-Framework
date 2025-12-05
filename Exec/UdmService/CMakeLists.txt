cmake_minimum_required(VERSION 3.8)

project(udmServiceAPI)
# 设置该工程采用C++ 17标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


message("lib udm source cmake:${CMAKE_CURRENT_LIST_DIR}")
# 设置包含目录和链接库目录
include_directories(${CMAKE_CURRENT_LIST_DIR}/../../sys/ThirdParty.win/include)
include_directories(${CMAKE_CURRENT_LIST_DIR}/../../sys/ThirdParty.win/include/libzmq)
include_directories(${CMAKE_CURRENT_LIST_DIR}/../../sys/ThirdParty.win/include/libuv)
include_directories(${CMAKE_CURRENT_LIST_DIR}/../Common/include)
include_directories(${CMAKE_CURRENT_LIST_DIR}/../udm)
include_directories(${CMAKE_CURRENT_LIST_DIR}/../Config)
include_directories(${CMAKE_CURRENT_LIST_DIR}/)
include_directories(${CMAKE_CURRENT_LIST_DIR}/udm)
include_directories(${CMAKE_CURRENT_LIST_DIR}/../log/include)
include_directories(${CMAKE_CURRENT_LIST_DIR}/../UdmMessage/include)

aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/ BASE_SOURCE)
aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/udm SERVICE_SOURCE)
if(DEFINED VLD_DEBUG)
	include_directories(${VLD_INCLUDE_PATH})
	link_directories(${VLD_LIB_PATH})
endif()
set(ALL_SOURCE_FILES 
	${BASE_SOURCE}
	${SERVICE_SOURCE}
	)
message("lib udm source files:${ALL_SOURCE_FILES}")	
	
set(OUT_INCLUDE_FILES ${CMAKE_CURRENT_LIST_DIR}/UdmService.h ${CMAKE_CURRENT_LIST_DIR}/UdmServiceFactory.h
	${CMAKE_CURRENT_LIST_DIR}/udm/ZeroMQServiceFactory.h)	
	
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	foreach(path ${DEBUG_LIBRARY_PATH})
	link_directories(${path})
	endforeach()
else()
	foreach(path ${RELEASE_LIBRARY_PATH})
	link_directories(${path})
	endforeach()
endif()

add_library(${PROJECT_NAME} SHARED ${ALL_SOURCE_FILES})
add_dependencies(${PROJECT_NAME} commonAPI)
add_dependencies(${PROJECT_NAME} LogAPI)
add_dependencies(${PROJECT_NAME} UdmMessageAPI_static)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	target_link_libraries(${PROJECT_NAME}  libzmq-v143-mt-gd-4_3_5 LogAPI
		commonAPI.lib UdmMessageAPI_static uv)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
	target_link_libraries(${PROJECT_NAME}  libzmq-v143-mt-4_3_5 LogAPI
		commonAPI.lib UdmMessageAPI_static uv)
endif()
if(DEFINED VLD_DEBUG)
	target_link_libraries(${PROJECT_NAME} vld)
endif()

install(TARGETS ${PROJECT_NAME}
LIBRARY DESTINATION lib
ARCHIVE DESTINATION lib)

set(LIBRARY_OUTPUT_PATH "${CMAKE_CURRENT_LIST_DIR}/../../sys/lib")
message("lib udm out path:${LIBRARY_OUTPUT_PATH}")


install(FILES ${CMAKE_CURRENT_LIST_DIR}/*.h DESTINATION include)