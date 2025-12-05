cmake_minimum_required(VERSION 3.8)

project(conf)

file(GLOB JSON_FILES "*.json")
set(DEST_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../sys/conf/")

file(COPY ${JSON_FILES} DESTINATION ${DEST_DIR})
