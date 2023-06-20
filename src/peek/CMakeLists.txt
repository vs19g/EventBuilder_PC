add_executable(Peek)

target_include_directories(Peek SYSTEM PUBLIC ${ROOT_INCLUDE_DIRS} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_sources(Peek PRIVATE
    main.cpp
)

target_link_libraries(Peek EVBDict EventBuilderCore ${ROOT_LIBRARIES})

set_target_properties(Peek PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${EVB_BINARY_DIR})