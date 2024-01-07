cmake_minimum_required(VERSION 3.5)
project(discord_gif_extractor VERSION 1.1 DESCRIPTION "A tool to rip out gif links from your favorites")

add_subdirectory(lib/base64pp)
add_subdirectory(lib/argparse)
find_package(Protobuf REQUIRED)
find_package(absl REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(CURL REQUIRED)

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROJECT_SOURCE_DIR}/src/proto/FrecencyUserSettings.proto)

set(OBJECTS
	src/gif_extractor.cc
	${PROTO_SRCS}
)

add_library(${PROJECT_NAME}_lib SHARED
	${OBJECTS}
)

add_executable(${PROJECT_NAME}
	src/main.cc
	${OBJECTS}
)

set(LINK_LIBRARIES
	base64pp
	curl
	${Protobuf_LIBRARIES}
	absl::log_internal_message absl::log_internal_check_op
	absl::raw_logging_internal absl::status
)

target_link_libraries(${PROJECT_NAME} ${LINK_LIBRARIES})
target_link_libraries(${PROJECT_NAME}_lib ${LINK_LIBRARIES})

include_directories(${PROJECT_NAME}
	lib/argparse/include
	lib/base64pp/base64pp/include/base64pp
	${CURL_INCLUDE_DIR}
	${PROJECT_BINARY_DIR}
	${PROTO_HDRS}
)

set_target_properties(${PROJECT_NAME}_lib PROPERTIES
	CXX_STANDARD 20
	CXX_STANDARD_REQUIRED ON
)

set_target_properties(${PROJECT_NAME} PROPERTIES
	CXX_STANDARD 20
	CXX_STANDARD_REQUIRED ON
)

