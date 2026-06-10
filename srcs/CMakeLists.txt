cmake_minimum_required(VERSION 3.21)

project(ACKrail VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Qt6 6.4 REQUIRED COMPONENTS Widgets)
find_package(Boost REQUIRED)

qt_standard_project_setup(REQUIRES 6.4)

qt_add_executable(ackrail
    engin/Biplace.cpp
    engin/Biplace.hpp
    engin/Engine.cpp
    engin/Engine.hpp
    engin/EngineFactory.cpp
    engin/EngineFactory.hpp
    geography/Geography.cpp
    geography/Geography.hpp
    main.cpp
    topology/Link.cpp
    topology/Link.hpp
    topology/Node.cpp
    topology/Node.hpp
    topology/Topology.cpp
    topology/Topology.hpp
    topology/TopologyWidget.cpp
    topology/TopologyWidget.hpp
)

set_source_files_properties(map/paris_metro.json PROPERTIES
    QT_RESOURCE_ALIAS map/paris_metro.json
)

qt_add_resources(ackrail "maps"
    PREFIX "/"
    FILES
        map/paris_metro.json
)

target_link_libraries(ackrail PRIVATE
    Boost::headers
    Qt6::Widgets
)

install(TARGETS ackrail
    BUNDLE DESTINATION .
    RUNTIME DESTINATION bin
)
