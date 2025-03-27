find_path(asio_incl NAMES asio.hpp)
if(asio_incl)
    set(Asio_FOUND TRUE)
    add_library(asio INTERFACE)
    target_include_directories(asio INTERFACE ${asio_incl})
    if(WIN32)
        target_link_libraries(asio INTERFACE "ws2_32")
    endif()
else()
    set(Asio_FOUND FALSE)
endif()

