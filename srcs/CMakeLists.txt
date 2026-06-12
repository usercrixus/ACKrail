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
    algorithm/Route.cpp
    algorithm/Route.hpp
    algorithm/RouteFinder.cpp
    algorithm/RouteFinder.hpp
    garage/engine/Biplace.cpp
    garage/engine/Biplace.hpp
    garage/engine/Engine.cpp
    garage/engine/Engine.hpp
    garage/engine/EngineFactory.cpp
    garage/engine/EngineFactory.hpp
    garage/Garage.cpp
    garage/Garage.hpp
    geography/Geography.cpp
    geography/Geography.hpp
    geography/Point.cpp
    geography/Point.hpp
    main.cpp
    simulator/TrafficGenerator.cpp
    simulator/TrafficGenerator.hpp
    simulator/TrafficManager.cpp
    simulator/TrafficManager.hpp
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

include(CTest)

if(BUILD_TESTING)
    add_executable(engine_tests
        algorithm/Route.cpp
        algorithm/Route.hpp
        garage/engine/Biplace.cpp
        garage/engine/Biplace.hpp
        garage/engine/Engine.cpp
        garage/engine/Engine.hpp
        garage/engine/EngineFactory.cpp
        garage/engine/EngineFactory.hpp
        garage/engine/EngineTest.cpp
        geography/Geography.cpp
        geography/Geography.hpp
        geography/Point.cpp
        geography/Point.hpp
        topology/Link.cpp
        topology/Link.hpp
        topology/Node.cpp
        topology/Node.hpp
    )

    target_link_libraries(engine_tests PRIVATE
        Boost::headers
        Qt6::Core
    )
    add_test(NAME engine_tests COMMAND engine_tests)

    add_executable(route_finder_tests
        algorithm/Route.cpp
        algorithm/Route.hpp
        algorithm/RouteFinder.cpp
        algorithm/RouteFinder.hpp
        algorithm/RouteFinderTest.cpp
        geography/Geography.cpp
        geography/Geography.hpp
        geography/Point.cpp
        geography/Point.hpp
        topology/Link.cpp
        topology/Link.hpp
        topology/Node.cpp
        topology/Node.hpp
        topology/Topology.cpp
        topology/Topology.hpp
    )

    target_link_libraries(route_finder_tests PRIVATE
        Boost::headers
        Qt6::Core
    )

    add_test(NAME route_finder_tests COMMAND route_finder_tests)
endif()

install(TARGETS ackrail
    BUNDLE DESTINATION .
    RUNTIME DESTINATION bin
)
