add_library(EVBDict SHARED)

target_include_directories(EVBDict SYSTEM PUBLIC ${ROOT_INCLUDE_DIRS} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
ROOT_GENERATE_DICTIONARY(evb_dict DataStructs.h LINKDEF LinkDef_evb.h MODULE EVBDict)

target_sources(EVBDict PRIVATE
    DataStructs.h
    DataStructs.cpp
)
target_link_libraries(EVBDict ${ROOT_LIBRARIES})
set_target_properties(EVBDict PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${EVB_LIBRARY_DIR})

add_custom_command(TARGET EVBDict POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_CURRENT_BINARY_DIR}/libEVBDict_rdict.pcm
    ${EVB_LIBRARY_DIR}/libEVBDict_rdict.pcm
)
