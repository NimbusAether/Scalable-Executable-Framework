cmake_minimum_required(VERSION 3.8)

project(udmAPI)

# 设置该工程采用C++ 17标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
add_definitions(-DUDM_LIB)


message("lib udm source cmake:${CMAKE_CURRENT_LIST_DIR}")
# 设置包含目录和链接库目录
include_directories(${CMAKE_CURRENT_LIST_DIR}/../../sys/ThirdParty.win/include)
include_directories(${CMAKE_CURRENT_LIST_DIR}/../../sys/ThirdParty.win/include/libuv)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	foreach(path ${DEBUG_LIBRARY_PATH})
	link_directories(${path})
	endforeach()
else()
	foreach(path ${RELEASE_LIBRARY_PATH})
	link_directories(${path})
	endforeach()
endif()

include_directories(${CMAKE_CURRENT_LIST_DIR}/../log/include)
include_directories(${CMAKE_CURRENT_LIST_DIR}/../Common/include)
include_directories(${CMAKE_CURRENT_LIST_DIR})
include_directories(${CMAKE_CURRENT_LIST_DIR}/IPCUdm)
include_directories(${CMAKE_CURRENT_LIST_DIR}/NetUdm)
include_directories(${CMAKE_CURRENT_LIST_DIR}/Business)
include_directories(${CMAKE_CURRENT_LIST_DIR}/../Config)
include_directories(${CMAKE_CURRENT_LIST_DIR}/../UdmMessage/include)

aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/../Common COMMON_SOURCE_FILE)
aux_source_directory(${CMAKE_CURRENT_LIST_DIR} UDM_SOURCE_FILE)
aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/IPCUdm IPCUDM_SOURCE_FILE)
aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/NetUdm NETUDM_SOURCE_FILE)
aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/Business UDMBUS_SOURCE_FILE)
set(ALL_SOURCE_FILES 
	${UDM_SOURCE_FILE}
	${IPCUDM_SOURCE_FILE}
	${NETUDM_SOURCE_FILE}
	${UDMBUS_SOURCE_FILE}
	)
message("lib udm source files:${ALL_SOURCE_FILES}")	
if(DEFINED VLD_DEBUG)
	include_directories(${VLD_INCLUDE_PATH})
	link_directories(${VLD_LIB_PATH})
endif()	
set(OUT_INCLUDE_FILES ${CMAKE_CURRENT_LIST_DIR}/UdmFactory.h ${CMAKE_CURRENT_LIST_DIR}/Udm.h)	
set(LIBRARY_OUTPUT_PATH "${CMAKE_CURRENT_LIST_DIR}/../../sys/lib")
message("lib udm out path:${LIBRARY_OUTPUT_PATH}")	



add_library(${PROJECT_NAME} SHARED ${ALL_SOURCE_FILES})
add_dependencies(${PROJECT_NAME} UdmMessageAPI_static)
add_dependencies(${PROJECT_NAME} LogAPI)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	target_link_libraries(${PROJECT_NAME} libzmq-v143-mt-gd-4_3_5 LogAPI commonAPI.lib UdmMessageAPI_static SerializeConfigAPI configAPI uv)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
	target_link_libraries(${PROJECT_NAME} libzmq-v143-mt-4_3_5 LogAPI commonAPI.lib UdmMessageAPI_static SerializeConfigAPI configAPI uv)
endif()
if(DEFINED VLD_DEBUG)
	target_link_libraries(${PROJECT_NAME} vld)
endif()

install(TARGETS ${PROJECT_NAME}
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib)

install(FILES ${CMAKE_CURRENT_LIST_DIR}/*.h DESTINATION include)