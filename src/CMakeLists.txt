add_executable(${PROJECT_NAME})

target_sources(${PROJECT_NAME} PUBLIC  
    main.c main_screen.c 
    styles.c spectrum.c radio.c dsp.c util.c 
    waterfall.c rotary.c keyboard.c encoder.c
    events.c msg.c keypad.c params.c
    bands.c hkey.c clock.c info.c
    meter.c band_info.c tx_info.c
    audio.c mfk.c cw.c cw_decoder.c pannel.c
    cat.c rtty.c screenshot.c
)

add_subdirectory(fonts)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

target_compile_options(${PROJECT_NAME} PRIVATE -g -fno-omit-frame-pointer -fasynchronous-unwind-tables)
target_link_options(${PROJECT_NAME} PRIVATE -g -rdynamic)

#target_compile_options(${PROJECT_NAME} PRIVATE -fsanitize=address -fsanitize=undefined -fno-sanitize-recover)
#target_link_options(${PROJECT_NAME} PRIVATE -fsanitize=address -fsanitize=undefined -fno-sanitize-recover -static-libasan -static-libubsan)

target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)
target_link_libraries(${PROJECT_NAME} PRIVATE lvgl lvgl::drivers)
target_link_libraries(${PROJECT_NAME} PRIVATE aether_x6100_control)
target_link_libraries(${PROJECT_NAME} PRIVATE liquid m)
target_link_libraries(${PROJECT_NAME} PRIVATE sqlite3)
target_link_libraries(${PROJECT_NAME} PRIVATE pulse pulse-simple)
target_link_libraries(${PROJECT_NAME} PRIVATE png)
