add_subdirectory(metal)

add_executable(metal_model_viewer main.cpp)
target_link_libraries(metal_model_viewer PRIVATE METAL_CPP)
