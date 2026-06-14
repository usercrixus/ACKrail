# ACKRail Traffic Contract Rules

## Purpose

This document defines the traffic model implemented by ACKRail. The system is
hybrid:

- `TrafficManager` selects and contracts a complete route.
- One `NodeController` per node schedules departures onto outgoing links.
- An engine executes the complete contract without changing its route.

Safety has priority over route duration and network throughput.

## Infrastructure

1. A link connects exactly two nodes.
2. Every link is bidirectional.
3. Each direction uses a physically independent track.
4. Opposite-direction engines do not conflict and have independent schedules.
5. Same-direction engines must preserve their order and safety distance.
6. Overtaking is forbidden.
7. Engines may wait at nodes, but not on links.

## Responsibilities

### Traffic Manager

1. The traffic manager receives an origin, destination, and engine.
2. It asks node controllers for the earliest available entry time of each link
   considered by the route algorithm.
3. It selects the route with the earliest predicted arrival time.
4. It commits reservations for every link in the selected route.
5. It gives the engine a complete route contract before departure.
6. It coordinates simulation time but does not own directional-link
   schedules.

### Node Controller

1. Every node has one controller.
2. A node controller owns the schedules of departures from its node.
3. Each outgoing link has an independent departure schedule.
4. The controller reports the earliest entry time compatible with existing
   reservations.
5. The controller records the engine associated with every committed
   reservation.
6. The controller must not return an entry time that violates the required
   same-direction separation.

### Engine

1. An engine must receive a complete contract before departure.
2. It must wait at each node for the duration specified by its contract.
3. It must traverse links in the contracted order.
4. It must travel at its maximum speed while on a link.
5. It must stop while waiting at a node.
6. It must not enter a link before its contracted entry time.
7. It must not modify or skip a contracted step.

## Route Contract

1. A contract contains one step per route link.
2. Each step contains:
   - the waiting duration at the departure node;
   - the link traversal duration.
3. Conceptually, each committed step also identifies:
   - the engine;
   - the departure node;
   - the destination node;
   - the link and direction;
   - the absolute entry time.
4. All steps are committed before the engine starts its route.
5. A route is rejected if any step cannot be scheduled.
6. Contract cancellation and replanning are not implemented yet.

Example:

```text
wait 10 seconds -> traverse link A
wait 2 seconds  -> traverse link B
wait 0 seconds  -> traverse link C
```

## Link Weight And Route Selection

1. Link weight is time-dependent.
2. For an engine arriving at a node at time `t`:

```text
link weight(t) = wait until earliest available entry + traversal time
```

3. Traversal time is:

```text
link distance / engine maximum speed
```

4. The arrival time produced by one link is used when evaluating the next
   link.
5. The selected route minimizes predicted arrival time, not distance alone.
6. Existing future reservations may cause a longer physical route to be
   selected.

## Engine Dimensions And Separation

1. A biplace engine is 3 metres long.
2. The minimum empty distance between two engines is 3 metres.
3. The initial protected departure distance is therefore:

```text
engine length + security distance = 6 metres
```

4. With constant and equal link speeds, the minimum interval between two
   same-direction entries is:

```text
entry separation time =
    protected departure distance / engine maximum speed
```

5. The following engine waits at the departure node until this interval is
   available.
6. This rule assumes engines on the same directional track travel at the same
   constant speed.
7. Different speeds, acceleration, braking, failures, and emergency stopping
   require a stronger safety model and are outside the current implementation.

## Directional Scheduling

1. A reservation belongs to a departure node, link, and direction.
2. The reverse direction is controlled by the node at the other end.
3. A reservation in one direction must not delay the opposite direction.
4. Reservations on different outgoing links are independent.
5. Reservations on the same directional link are ordered by entry time.

## Contract Execution

1. Simulation time advances monotonically.
2. Waiting time is consumed before traversal begins.
3. Engine speed is zero during contracted waiting.
4. Engine speed equals maximum speed during traversal.
5. Excess time in a simulation tick may continue into the next contract step.
6. Actual travelled distance, rather than average speed, determines the
   engine's rendered position.
7. The engine becomes idle after completing the final link.

## Current Limitations

1. Contracts use exact times rather than entry windows.
2. Reservations are not cancelled when an engine is delayed or fails.
3. Link blockage and engine failure are not modelled.
4. Junction conflict areas are not modelled.
5. Station capacity and dwell time are not modelled.
6. All active engines are currently biplaces with the same maximum speed.
7. Safety is enforced through departure spacing, not continuous collision
   detection.
8. Old reservations are not yet removed from node schedules.

These limitations must be addressed before the model is treated as a
realistic railway safety system.
