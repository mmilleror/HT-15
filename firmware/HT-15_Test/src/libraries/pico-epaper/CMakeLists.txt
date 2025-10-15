cmake_minimum_required(VERSION 3.13)

if (NOT TARGET _pico_epaper_inclusion_marker)
    add_library(_pico_epaper_inclusion_marker INTERFACE)

    include(pico_sdk_import.cmake)

    project(pico_epaper)

    set(CMAKE_C_STANDARD 11)

    pico_sdk_init()

    pico_is_top_level_project(PICO_EPAPER_TOP_LEVEL_PROJECT)

    add_library(pico_epaper_included INTERFACE)
    target_compile_definitions(pico_epaper_included INTERFACE
            -DPICO_EPAPER=1
            )

    pico_add_platform_library(pico_epaper_included)

    if (NOT PICO_EPAPER_PATH)
        set(PICO_EPAPER_PATH ${CMAKE_CURRENT_LIST_DIR})
    endif ()
    set(PICO_EPAPER_PATH "${PICO_EPAPER_PATH}" CACHE PATH "Path to pico-epaper")

    add_library(pico_epaper INTERFACE)
    target_sources(pico_epaper INTERFACE src/epaper.c src/luts.c)
    target_link_libraries(pico_epaper INTERFACE pico_stdlib hardware_spi)
    target_include_directories(pico_epaper INTERFACE include)

    if (PICO_EPAPER_EXAMPLES_ENABLED OR PICO_EPAPER_TOP_LEVEL_PROJECT)
        add_executable(draw_patterns src/examples/draw_patterns.c)
        target_link_libraries(draw_patterns PRIVATE pico_stdlib pico_epaper)
        pico_add_extra_outputs(draw_patterns)
    endif ()

endif ()
