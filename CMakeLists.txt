cmake_minimum_required(VERSION 3.5)
project(discord_gif_extractor VERSION 1.0 DESCRIPTION "A tool to rip out gif links from your favorites")

add_subdirectory(lib/base64pp)
find_package(Protobuf REQUIRED)
find_package(absl REQUIRED)
find_package(nlohmann_json REQUIRED)

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${CMAKE_SOURCE_DIR}/src/proto/FrecencyUserSettings.proto)

add_executable(${PROJECT_NAME}
	src/main.cc
	${PROTO_SRCS}
)

target_link_libraries(${PROJECT_NAME}
	base64pp
	${Protobuf_LIBRARIES}
	absl::log_internal_message absl::log_internal_check_op
	absl::raw_logging_internal absl::status
)

target_include_directories(${PROJECT_NAME} PRIVATE
	lib/base64pp/base64pp/include/base64pp
	${CMAKE_BINARY_DIR}
	${PROTO_HDRS}
)

set_target_properties(${PROJECT_NAME} PROPERTIES
	CXX_STANDARD 20
	CXX_STANDARD_REQUIRED ON
)
