add_library(EventBuilderCore STATIC)
target_include_directories(EventBuilderCore SYSTEM PUBLIC ../../vendor/spdlog/include ${ROOT_INCLUDE_DIRS} 
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/../
)

target_precompile_headers(EventBuilderCore PRIVATE ../EventBuilder.h)

target_sources(EventBuilderCore PRIVATE
    CompassRun.h
    FlagHandler.cpp
    Stopwatch.cpp
    FlagHandler.h
    OrderChecker.cpp
    Stopwatch.h
    OrderChecker.h
    CompassFile.cpp
    EVBApp.cpp
    ProgressCallback.h
    ShiftMap.cpp
    CompassFile.h
    EVBApp.h
    Logger.cpp
    RunCollector.cpp
    ShiftMap.h
    CompassHit.h
    Logger.h
    RunCollector.h
    SlowSort.cpp
    CompassRun.cpp
    SlowSort.h
    EVBParameters.h
    EVBWorkspace.h
    EVBWorkspace.cpp
)

target_link_libraries(EventBuilderCore PUBLIC
    EVBDict
    ${ROOT_LIBRARIES}
)

set_target_properties(EventBuilderCore PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${EVB_LIBRARY_DIR})