add_executable(${PROJECT_NAME})

target_sources(${PROJECT_NAME} PUBLIC  
    main.c mouse_cursor_icon.c main_screen.c 
    styles.c spectrum.c radio.c dsp.c util.c 
    waterfall.c rotary.c mouse.c
    events.c msg.c keypad.c
)

add_subdirectory(fonts)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)
target_link_libraries(${PROJECT_NAME} PRIVATE lvgl lvgl::drivers)
target_link_libraries(${PROJECT_NAME} PRIVATE aether_x6100_control)
target_link_libraries(${PROJECT_NAME} PRIVATE liquid)
