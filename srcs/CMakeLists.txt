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
    algorithm/StaticRouteCostMatrix.cpp
    algorithm/StaticRouteCostMatrix.hpp
    DesignPatter/IndexedRandomPool.hpp
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
    simulator/SimulationSession.cpp
    simulator/SimulationSession.hpp
    simulator/TrafficBalancer.cpp
    simulator/TrafficBalancer.hpp
    simulator/TrafficGenerator.cpp
    simulator/TrafficGenerator.hpp
    simulator/TrafficEventManager.cpp
    simulator/TrafficEventManager.hpp
    simulator/NodeController.cpp
    simulator/NodeController.hpp
    simulator/PassengerDispatcher.cpp
    simulator/PassengerDispatcher.hpp
    simulator/TrafficManager.cpp
    simulator/TrafficManager.hpp
    simulator/TrafficRouteManager.cpp
    simulator/TrafficRouteManager.hpp
    simulator/TrafficSimulator.cpp
    simulator/TrafficSimulator.hpp
    statistics/EngineStatistics.cpp
    statistics/EngineStatistics.hpp
    statistics/helper/CompletedTrip.hpp
    statistics/helper/EngineReport.hpp
    statistics/helper/NumericSummary.cpp
    statistics/helper/NumericSummary.hpp
    statistics/helper/StationReport.hpp
    statistics/helper/StationsGlobalReport.hpp
    statistics/SimulationStatistics.cpp
    statistics/SimulationStatistics.hpp
    statistics/SimulationStatisticsCollector.cpp
    statistics/SimulationStatisticsCollector.hpp
    statistics/StationStatistics.cpp
    statistics/StationStatistics.hpp
    topology/Link.cpp
    topology/Link.hpp
    topology/Node.cpp
    topology/Node.hpp
    topology/Topology.cpp
    topology/Topology.hpp
    view/AckRailWindow.cpp
    view/AckRailWindow.hpp
    view/dialogs/SimulationConfigDialog.cpp
    view/dialogs/SimulationConfigDialog.hpp
    view/dialogs/StatisticsDialog.cpp
    view/dialogs/StatisticsDialog.hpp
    view/SimulationController.cpp
    view/SimulationController.hpp
    view/simulation/SimulationWidget.cpp
    view/simulation/SimulationWidget.hpp
    view/helper/MapCamera.cpp
    view/helper/MapCamera.hpp
    view/helper/MapViewport.cpp
    view/helper/MapViewport.hpp
    view/simulation/EmptyPageWidget.cpp
    view/simulation/EmptyPageWidget.hpp
    view/simulation/header/HeaderWidget.cpp
    view/simulation/header/HeaderWidget.hpp
    view/simulation/engine/EngineRenderer.cpp
    view/simulation/engine/EngineRenderer.hpp
    view/simulation/topology/LinkRenderer.cpp
    view/simulation/topology/LinkRenderer.hpp
    view/simulation/map/MapWidget.cpp
    view/simulation/map/MapWidget.hpp
    view/simulation/topology/NodeRenderer.cpp
    view/simulation/topology/NodeRenderer.hpp
    view/statistics/EngineStatisticsWidget.cpp
    view/statistics/EngineStatisticsWidget.hpp
    view/statistics/GlobalStationStatisticsWidget.cpp
    view/statistics/GlobalStationStatisticsWidget.hpp
    view/statistics/StationStatisticsWidget.cpp
    view/statistics/StationStatisticsWidget.hpp
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

qt_add_resources(ackrail "assets"
    PREFIX "/assets"
    BASE view/asset
    FILES
        view/asset/biplace.png
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
        algorithm/StaticRouteCostMatrix.cpp
        algorithm/StaticRouteCostMatrix.hpp
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

    add_executable(indexed_random_pool_tests
        DesignPatter/IndexedRandomPool.hpp
        DesignPatter/IndexedRandomPoolTest.cpp
    )

    add_test(NAME indexed_random_pool_tests COMMAND indexed_random_pool_tests)

    add_executable(traffic_manager_tests
        algorithm/Dijkstra.cpp
        algorithm/Dijkstra.hpp
        algorithm/Route.cpp
        algorithm/Route.hpp
        algorithm/StaticRouteCostMatrix.cpp
        algorithm/StaticRouteCostMatrix.hpp
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
        simulator/PassengerDispatcher.cpp
        simulator/PassengerDispatcher.hpp
        simulator/TrafficEventManager.cpp
        simulator/TrafficEventManager.hpp
        simulator/TrafficBalancer.cpp
        simulator/TrafficBalancer.hpp
        simulator/TrafficManager.cpp
        simulator/TrafficManager.hpp
        simulator/TrafficRouteManager.cpp
        simulator/TrafficRouteManager.hpp
        simulator/TrafficManagerTest.cpp
        statistics/EngineStatistics.cpp
        statistics/EngineStatistics.hpp
        statistics/helper/CompletedTrip.hpp
        statistics/helper/EngineReport.hpp
        statistics/helper/NumericSummary.cpp
        statistics/helper/NumericSummary.hpp
        statistics/helper/StationReport.hpp
        statistics/helper/StationsGlobalReport.hpp
        statistics/SimulationStatistics.cpp
        statistics/SimulationStatistics.hpp
        statistics/SimulationStatisticsCollector.cpp
        statistics/SimulationStatisticsCollector.hpp
        statistics/StationStatistics.cpp
        statistics/StationStatistics.hpp
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
