option(RGSCANNER_ENABLED "Replay gain scan support" ON)

if (RGSCANNER_ENABLED)
    find_file(DEADBEEF_RGSCANNER rg_scanner.so PATHS ${DEADBEEF_PREFIX}/lib/deadbeef)
    if (DEADBEEF_RGSCANNER)
        message(STATUS "Deadbeef Replay gain scanner plugin - found ${DEADBEEF_RGSCANNER}")
        set(${RGSCANNER_ENABLED} ON)
    else (DEADBEEF_RGSCANNER)
        message(STATUS "Deadbeef Replay gain scanner plugin - not found")
        set(${RGSCANNER_ENABLED} OFF)
    endif(DEADBEEF_RGSCANNER)
endif(RGSCANNER_ENABLED)

if (RGSCANNER_ENABLED)
    set(HEADERS ${HEADERS}
        plugins/RGScanner/include/rg_scanner.h
    )
endif (RGSCANNER_ENABLED)
