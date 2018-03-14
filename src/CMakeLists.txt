aux_source_directory(PvaDetector PVASRCS)
message(${PVASRCS})
add_library(pva SHARED ${PVASRCS})

aux_source_directory(utils UTILSSRCS)
message(${UTILSSRCS})
add_library(tools SHARED ${UTILSSRCS})
target_link_libraries(tools ${OpenCV_LIBS})# added by myf


aux_source_directory(myfUtils MyfUtils_SRC)
add_library( myfUtils
        SHARED
        ${MyfUtils_SRC})
target_link_libraries( myfUtils
        ${OpenCV_LIBS})
