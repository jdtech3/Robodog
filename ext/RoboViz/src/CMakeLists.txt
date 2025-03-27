add_library(utils OBJECT utils.cpp)
add_library(thedog OBJECT thedog.cpp)
add_library(sdl3_wrapper OBJECT sdl3_wrapper.cpp)

add_executable(roboviz roboviz.cpp
    $<TARGET_OBJECTS:sdl3_wrapper>
    $<TARGET_OBJECTS:thedog>
    $<TARGET_OBJECTS:utils>)
add_executable(orient orient.cpp
    $<TARGET_OBJECTS:sdl3_wrapper>
    $<TARGET_OBJECTS:thedog>
    $<TARGET_OBJECTS:utils>)
target_link_libraries(roboviz SDL3::SDL3 GLider::GLider glm::glm)
target_link_libraries(orient SDL3::SDL3 GLider::GLider glm::glm asio)
