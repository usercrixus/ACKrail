cmake_minimum_required(VERSION 3.21)

project(ACKrail VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Qt6 6.4 REQUIRED COMPONENTS OpenGL OpenGLWidgets Widgets)
qt_standard_project_setup(REQUIRES 6.4)

qt_add_executable(ackrail
    algorithm/Dijkstra.cpp
    algorithm/Dijkstra.hpp
    algorithm/Route.cpp
    algorithm/Route.hpp
    garage/engine/Biplace.cpp
    garage/engine/Biplace.hpp
    garage/engine/Engine.cpp
    garage/engine/Engine.hpp
    garage/engine/EnginePad.cpp
    garage/engine/EnginePad.hpp
    garage/Garage.cpp
    garage/Garage.hpp
    geography/Geography.cpp
    geography/Geography.hpp
    geography/Point.cpp
    geography/Point.hpp
    main.cpp
    simulator/TrafficGenerator.cpp
    simulator/TrafficGenerator.hpp
    simulator/NodeController.cpp
    simulator/NodeController.hpp
    simulator/TrafficManager.cpp
    simulator/TrafficManager.hpp
    simulator/TrafficSimulator.cpp
    simulator/TrafficSimulator.hpp
    topology/Link.cpp
    topology/Link.hpp
    topology/Node.cpp
    topology/Node.hpp
    topology/Topology.cpp
    topology/Topology.hpp
    view/TopologyWidget.cpp
    view/TopologyWidget.hpp
    view/helper/MapCamera.cpp
    view/helper/MapCamera.hpp
    view/helper/MapViewport.cpp
    view/helper/MapViewport.hpp
    view/widgets/HeaderWidget.cpp
    view/widgets/HeaderWidget.hpp
    view/widgets/EngineRenderer.cpp
    view/widgets/EngineRenderer.hpp
    view/widgets/LinkRenderer.cpp
    view/widgets/LinkRenderer.hpp
    view/widgets/MapWidget.cpp
    view/widgets/MapWidget.hpp
    view/widgets/NodeRenderer.cpp
    view/widgets/NodeRenderer.hpp
)

set_source_files_properties(map/paris_metro.json PROPERTIES
    QT_RESOURCE_ALIAS map/paris_metro.json
)

qt_add_resources(ackrail "maps"
    PREFIX "/"
    FILES
        map/paris_metro.json
)

qt_add_resources(ackrail "shaders"
    PREFIX "/shaders"
    BASE view/shaders
    FILES
        view/shaders/engine.frag
        view/shaders/engine.vert
        view/shaders/link.frag
        view/shaders/link.vert
        view/shaders/node.frag
        view/shaders/node.vert
)

target_link_libraries(ackrail PRIVATE
    Qt6::OpenGL
    Qt6::OpenGLWidgets
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
        garage/engine/EnginePad.cpp
        garage/engine/EnginePad.hpp
        garage/engine/EngineTest.cpp
        geography/Geography.cpp
        geography/Geography.hpp
        geography/Point.cpp
        geography/Point.hpp
        topology/Link.cpp
        topology/Link.hpp
        topology/Node.cpp
        topology/Node.hpp
        simulator/NodeController.cpp
        simulator/NodeController.hpp
    )

    target_link_libraries(engine_tests PRIVATE
        Qt6::Core
    )
    add_test(NAME engine_tests COMMAND engine_tests)

    add_executable(dijkstra_tests
        algorithm/Dijkstra.cpp
        algorithm/Dijkstra.hpp
        algorithm/DijkstraTest.cpp
        algorithm/Route.cpp
        algorithm/Route.hpp
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
        simulator/NodeController.cpp
        simulator/NodeController.hpp
    )

    target_link_libraries(dijkstra_tests PRIVATE
        Qt6::Core
    )

    add_test(NAME dijkstra_tests COMMAND dijkstra_tests)

    add_executable(traffic_manager_tests
        algorithm/Dijkstra.cpp
        algorithm/Dijkstra.hpp
        algorithm/Route.cpp
        algorithm/Route.hpp
        garage/engine/Biplace.cpp
        garage/engine/Biplace.hpp
        garage/engine/Engine.cpp
        garage/engine/Engine.hpp
        garage/engine/EnginePad.cpp
        garage/engine/EnginePad.hpp
        garage/Garage.cpp
        garage/Garage.hpp
        geography/Geography.cpp
        geography/Geography.hpp
        geography/Point.cpp
        geography/Point.hpp
        simulator/NodeController.cpp
        simulator/NodeController.hpp
        simulator/TrafficManager.cpp
        simulator/TrafficManager.hpp
        simulator/TrafficManagerTest.cpp
        topology/Link.cpp
        topology/Link.hpp
        topology/Node.cpp
        topology/Node.hpp
        topology/Topology.cpp
        topology/Topology.hpp
    )

    target_link_libraries(traffic_manager_tests PRIVATE
        Qt6::Core
    )

    add_test(NAME traffic_manager_tests COMMAND traffic_manager_tests)
endif()

install(TARGETS ackrail
    BUNDLE DESTINATION .
    RUNTIME DESTINATION bin
)
