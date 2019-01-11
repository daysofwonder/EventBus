file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/log)

add_dow_unit_test(
    TARGET EventBusTests
    SRC
        main.cpp
        EventCollectorTests.cpp
        NotifierTests.cpp
    LIB
        Dexode::EventBus
)

target_include_directories(EventBusTests
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src
)
