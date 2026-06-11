# ACKRail Business Roadmap

## Purpose

This document describes how ACKRail could evolve from a personal research
project into a viable company. It should be reviewed as the software,
simulation results, and market understanding improve.

The technical project alone is not yet a business. Commercial viability will
depend on solving a specific, expensive problem for a clearly identified
customer.

## Proposed Positioning

ACKRail should not initially be presented as a simulator for a speculative
vehicle concept. A stronger and more broadly useful positioning is:

> A simulation and decision-support platform for designing, testing, and
> comparing autonomous rail transport systems before deployment.

Potential users include:

- Rail and metro operators.
- Infrastructure managers.
- Rolling-stock and signalling manufacturers.
- Engineering and mobility consulting firms.
- Local authorities planning transport infrastructure.
- Research institutes and universities.
- Operators of automated shuttles and industrial rail systems.

The product must eventually demonstrate measurable value, such as:

- Increasing network capacity.
- Reducing passenger waiting and travel times.
- Reducing the number of vehicles required for a given demand.
- Preventing route conflicts and collisions.
- Improving resilience during incidents or peak demand.
- Comparing infrastructure and operating strategies.
- Estimating energy use and operating costs.

## Product Roadmap

### Stage 1: Technical Demonstrator

The objective is to prove that the simulation foundation works.

- Load real and experimental rail topologies.
- Calculate routes between connected stations.
- Simulate a fleet of biplace modules.
- Display vehicles moving across the network.
- Model speed, acceleration, braking, and stopping distance.
- Detect and prevent collisions and route conflicts.
- Produce repeatable scenarios using configurable random seeds.
- Record basic metrics such as completed trips, travel time, distance,
  utilization, and blocked vehicles.

At this stage, ACKRail is ready to be shown informally to engineers,
researchers, and incubators for feedback. It is not yet ready for investment.

### Stage 2: Credible Simulation Product

The objective is to model operational constraints well enough to support a
professional discussion.

- Represent track occupancy and safe separation.
- Manage junction reservations and conflicting routes.
- Model stations, dwell time, queues, and platform capacity.
- Model passenger demand by origin, destination, and time period.
- Support peak, off-peak, event, disruption, and degraded-mode scenarios.
- Compare at least two dispatching or traffic-control strategies.
- Export simulation results in a reusable format.
- Provide charts and a clear experiment summary.
- Document assumptions, units, limitations, and validation methods.
- Demonstrate that identical inputs produce comparable results.

At this stage, ACKRail may be ready for expert interviews, sector programs,
and discussions about a small proof of concept.

### Stage 3: Pilot-Ready Product

The objective is to solve one concrete problem for one professional partner.

- Select a precise use case with an operator or industrial partner.
- Import or reproduce the partner's network and operating constraints.
- Define success metrics before running the experiment.
- Validate simulation behavior with domain experts or reference data.
- Produce a measurable result and explain its limitations.
- Provide a stable demonstration and usable documentation.
- Establish ownership, licensing, confidentiality, and data terms.
- Estimate the work and cost required for a paid pilot.

Examples of useful results include:

- A capacity increase under a defined demand scenario.
- A reduction in delays or route conflicts.
- A reduction in the required fleet size.
- Better recovery after a station or track failure.
- A comparison showing when one control strategy outperforms another.

At this stage, the priority is obtaining a pilot, a letter of interest, or a
first contract rather than raising a large investment round.

### Stage 4: Fundable Company

The objective is to prove that ACKRail can become a repeatable business rather
than a single custom engineering project.

- Create a legal company with clear intellectual-property ownership.
- Define the customer, buyer, user, and purchasing process.
- Define a business model: licence, subscription, paid study, integration, or
  a combination of these.
- Complete at least one successful pilot.
- Obtain evidence of demand from additional prospects.
- Build a credible technical and commercial team.
- Define a product roadmap and financing plan.
- Explain the competitive advantage over existing simulation products.
- Show how the software can be reused across customers.

This is the appropriate stage for business angels, seed investors, and public
innovation funding. SNCF's 574 Invest generally targets later-stage startups,
from Series A, and should not be treated as the first source of funding.

## Who to Contact

### Now or During Stage 1

- **Pepite France**: student and recent-graduate entrepreneurship support.
- **Bpifrance Creation**: company-building guidance, market validation,
  legal-structure information, and financing orientation.
- **Local incubators**: business coaching, market discovery, and potential
  co-founder networks.
- **Rail engineers and researchers**: early validation of assumptions and
  identification of relevant operational problems.

The request should be for feedback, orientation, and domain knowledge, not
investment.

### During Stage 2

- **i-Trans**: French competitiveness cluster for connected and decarbonized
  mobility, with access to industrial networks and relevant calls for projects.
- **Railenium**: railway research institute and potential source of technical
  expertise, research partnerships, and validation.
- **SNCF startup and innovation teams**: introductions to relevant business
  units and possible use cases.
- **Other operators and manufacturers**: Keolis, RATP, Alstom, Siemens
  Mobility, Hitachi Rail, SYSTRA, and smaller automated-transport companies.

The request should be for an expert meeting, use-case discovery, access to
realistic constraints, or discussion of a limited proof of concept.

### During Stage 3

- A business unit that owns the identified operational problem.
- Public innovation programs and regional funding bodies.
- Initiative France or Reseau Entreprendre for mentoring and possible
  interest-free founder loans.
- Specialized incubators and accelerators.
- Prospective customers willing to fund or co-fund a pilot.

The request should describe the problem, proposed experiment, expected
deliverables, duration, cost, and success criteria.

### During Stage 4

- Mobility- or deeptech-focused business angels.
- Seed venture-capital funds.
- Corporate venture funds.
- Bpifrance innovation financing.
- Strategic industrial partners.
- Later, 574 Invest if the company has sufficient traction and strategic
  relevance to SNCF.

## Contact Readiness Checks

### Ready for Expert Feedback

- The application builds and runs reliably.
- A short demonstration can be completed without manual repair.
- The current capabilities and limitations can be explained honestly.
- At least one technical question is prepared for the expert.
- The request is specific and does not immediately ask for funding.

### Ready for an Incubator

- The problem and target customer are described in one paragraph.
- A working demonstrator or clear prototype exists.
- The founder can commit meaningful time to the project.
- The main commercial assumptions and unknowns are listed.
- There is a plan for interviewing potential users.

### Ready for a Professional Pilot Discussion

- One concrete customer problem has been identified.
- The simulator models the constraints relevant to that problem.
- Scenarios are repeatable and produce measurable outputs.
- A comparison or optimization result can be demonstrated.
- The software's limitations and validation status are documented.
- The proposed pilot has scope, deliverables, timing, and success criteria.

### Ready for Business Angels or Seed Funding

- A company exists or is ready to be incorporated.
- Intellectual-property ownership is clear.
- At least one pilot, customer, or strong letter of intent exists.
- A credible market and business model have been identified.
- The team covers both technical delivery and customer development.
- The requested funding amount is tied to specific milestones.

### Ready for 574 Invest or Another Corporate Investor

- The company has meaningful commercial traction.
- The product can be deployed for multiple customers.
- A SNCF business unit has demonstrated interest.
- The company has a credible growth strategy.
- The investment creates both financial potential and strategic value.

## What Not to Do

- Do not spend a year adding features without interviewing potential users.
- Do not claim railway safety or operational validity without expert
  validation.
- Do not present the simulation as a certified signalling or control system.
- Do not build only around one fictional vehicle if the simulation engine can
  address a broader market.
- Do not contact investors before defining the customer problem and business
  model.
- Do not give away intellectual-property rights during informal discussions.
- Do not assume that employment by an industrial company includes a purchase
  of ACKRail or its intellectual property.

## Immediate Action Plan

1. Complete the technical demonstrator.
2. Prepare a three-minute demonstration and a one-page project summary.
3. Interview at least 15 professionals from operators, manufacturers,
   engineering firms, and research organizations.
4. Record their problems, current tools, budgets, constraints, and purchasing
   processes.
5. Select one use case based on evidence rather than technical preference.
6. Adapt ACKRail to demonstrate that use case.
7. Seek an incubator and sector partner.
8. Define and propose a limited pilot.
9. Create the company when a pilot, grant, contract, or investment makes the
   legal structure necessary.

## Future Project Reviews

When reviewing whether ACKRail is ready to contact an organization, evaluate:

- The current repository and working demonstration.
- The relevant readiness checklist in this document.
- The exact organization and objective of the contact.
- The evidence available for technical credibility and market demand.
- The missing items that would materially improve the approach.

The result should be one of:

- **Contact now**: the project is sufficiently prepared for the stated goal.
- **Contact for feedback only**: useful discussion is possible, but a pilot or
  funding request would be premature.
- **Wait and complete specific milestones**: the missing evidence would make
  the contact substantially more effective.

This document should be updated whenever customer interviews, partnerships,
technical milestones, or financing opportunities change the strategy.
