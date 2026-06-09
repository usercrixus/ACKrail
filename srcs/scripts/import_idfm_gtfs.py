#!/usr/bin/env python3

import argparse
import csv
import io
import json
import shutil
import sys
import tempfile
import urllib.request
import zipfile


DEFAULT_GTFS_URL = (
    "https://www.data.gouv.fr/api/1/datasets/r/"
    "413988ed-d340-467b-8be2-7b999fcd207a"
)


class Route:
    def __init__(self, route_id, name, color):
        self.id = route_id
        self.name = name
        self.color = "#" + (color or "547083")


class Stop:
    def __init__(self, stop_id, name, latitude, longitude, parent_id, location_type):
        self.id = stop_id
        self.name = name
        self.latitude = latitude
        self.longitude = longitude
        self.parent_id = parent_id
        self.location_type = location_type


class Trip:
    def __init__(self, route_id, destination, direction):
        self.route_id = route_id
        self.destination = destination
        self.direction = direction

    def pattern_key(self):
        return self.route_id, self.destination, self.direction


class GtfsTopologyImporter:
    def __init__(self, archive):
        self.archive = archive
        self.routes = {}
        self.stops = {}
        self.trips = {}
        self.patterns = {}

    def import_topology(self):
        self._read_routes()
        self._read_stops()
        self._read_trips()
        self._read_stop_patterns()
        return self._build_topology()

    def _csv_rows(self, file_name):
        binary_file = self.archive.open(file_name)
        text_file = io.TextIOWrapper(binary_file, encoding="utf-8-sig", newline="")
        return csv.DictReader(text_file)

    def _read_routes(self):
        for row in self._csv_rows("routes.txt"):
            route_type = row["route_type"]
            route_name = row["route_short_name"].strip()
            is_metro = route_type == "1"
            is_rer = route_type == "2" and route_name in {"A", "B", "C", "D", "E"}

            if is_metro or is_rer:
                self.routes[row["route_id"]] = Route(
                    row["route_id"],
                    route_name,
                    row["route_color"].strip(),
                )

    def _read_stops(self):
        for row in self._csv_rows("stops.txt"):
            try:
                latitude = float(row["stop_lat"])
                longitude = float(row["stop_lon"])
            except ValueError:
                continue

            self.stops[row["stop_id"]] = Stop(
                row["stop_id"],
                row["stop_name"].strip(),
                latitude,
                longitude,
                row["parent_station"].strip(),
                row["location_type"].strip(),
            )

    def _read_trips(self):
        for row in self._csv_rows("trips.txt"):
            route_id = row["route_id"]
            if route_id not in self.routes:
                continue

            self.trips[row["trip_id"]] = Trip(
                route_id,
                row["trip_headsign"].strip(),
                row["direction_id"].strip(),
            )

    def _canonical_stop_id(self, stop_id):
        stop = self.stops.get(stop_id)
        if stop is None:
            return None
        if stop.parent_id and stop.parent_id in self.stops:
            return stop.parent_id
        return stop.id

    def _read_stop_patterns(self):
        current_trip_id = None
        current_sequence = []

        for row in self._csv_rows("stop_times.txt"):
            trip_id = row["trip_id"]
            if trip_id != current_trip_id:
                self._store_pattern(current_trip_id, current_sequence)
                current_trip_id = trip_id
                current_sequence = []

            if trip_id not in self.trips:
                continue

            stop_id = self._canonical_stop_id(row["stop_id"])
            if stop_id is not None and (not current_sequence or current_sequence[-1] != stop_id):
                current_sequence.append(stop_id)

        self._store_pattern(current_trip_id, current_sequence)

    def _store_pattern(self, trip_id, sequence):
        trip = self.trips.get(trip_id)
        if trip is None or len(sequence) < 2:
            return

        key = trip.pattern_key()
        previous = self.patterns.get(key)
        if previous is None or len(sequence) > len(previous):
            self.patterns[key] = list(sequence)

    def _build_topology(self):
        used_stop_ids = set()
        link_keys = set()
        links = []

        for route_id, sequence in self._maximal_patterns():
            route = self.routes[route_id]
            used_stop_ids.update(sequence)

            for from_stop_id, to_stop_id in zip(sequence, sequence[1:]):
                undirected_pair = tuple(sorted((from_stop_id, to_stop_id)))
                link_key = route.id, undirected_pair
                if link_key in link_keys:
                    continue

                link_keys.add(link_key)
                links.append(
                    {
                        "fromSourceId": from_stop_id,
                        "toSourceId": to_stop_id,
                        "line": route.name,
                        "color": route.color,
                    }
                )

        ordered_stop_ids = sorted(
            used_stop_ids,
            key=lambda stop_id: (
                self.stops[stop_id].name.casefold(),
                stop_id,
            ),
        )
        node_ids = {
            source_id: index
            for index, source_id in enumerate(ordered_stop_ids, start=1)
        }

        nodes = []
        for source_id in ordered_stop_ids:
            stop = self.stops[source_id]
            nodes.append(
                {
                    "id": node_ids[source_id],
                    "sourceId": source_id,
                    "name": stop.name,
                    "latitude": stop.latitude,
                    "longitude": stop.longitude,
                }
            )

        output_links = []
        for index, link in enumerate(
            sorted(
                links,
                key=lambda value: (
                    self._line_sort_key(value["line"]),
                    value["fromSourceId"],
                    value["toSourceId"],
                ),
            ),
            start=1,
        ):
            output_links.append(
                {
                    "id": index,
                    "from": node_ids[link["fromSourceId"]],
                    "to": node_ids[link["toSourceId"]],
                    "line": link["line"],
                    "color": link["color"],
                }
            )

        return {
            "name": "Paris and Île-de-France Métro + RER",
            "coordinateSystem": "WGS84",
            "source": "Île-de-France Mobilités GTFS",
            "sourceUrl": DEFAULT_GTFS_URL,
            "nodes": nodes,
            "links": output_links,
        }

    def _maximal_patterns(self):
        grouped_patterns = {}
        for key, sequence in self.patterns.items():
            route_id, destination, direction = key
            group_key = route_id, direction
            grouped_patterns.setdefault(group_key, []).append(sequence)

        maximal_patterns = []
        for group_key in sorted(grouped_patterns):
            candidates = sorted(
                grouped_patterns[group_key],
                key=lambda sequence: (-len(sequence), sequence),
            )
            retained = []

            for candidate in candidates:
                if any(self._is_subsequence(candidate, sequence) for sequence in retained):
                    continue
                retained.append(candidate)

            for sequence in retained:
                maximal_patterns.append((group_key[0], sequence))

        return maximal_patterns

    def _is_subsequence(self, candidate, sequence):
        candidate_index = 0
        for stop_id in sequence:
            if candidate[candidate_index] == stop_id:
                candidate_index += 1
                if candidate_index == len(candidate):
                    return True
        return False

    def _line_sort_key(self, name):
        if name in {"A", "B", "C", "D", "E"}:
            return 100, name
        normalized = name.replace("B", ".5")
        try:
            return float(normalized), name
        except ValueError:
            return 99, name


def download_archive(url):
    print(f"Downloading official GTFS: {url}", file=sys.stderr)
    temporary_file = tempfile.TemporaryFile()
    with urllib.request.urlopen(url) as response:
        shutil.copyfileobj(response, temporary_file)
    temporary_file.seek(0)
    return temporary_file


def parse_arguments():
    parser = argparse.ArgumentParser(
        description="Generate ACKrail topology JSON from the official IDFM GTFS feed."
    )
    parser.add_argument(
        "--archive",
        help="Use an existing GTFS ZIP instead of downloading the current feed.",
    )
    parser.add_argument(
        "--output",
        default="map/paris_metro.json",
        help="Output JSON file.",
    )
    return parser.parse_args()


def main():
    arguments = parse_arguments()
    archive_source = (
        open(arguments.archive, "rb")
        if arguments.archive
        else download_archive(DEFAULT_GTFS_URL)
    )

    with archive_source:
        with zipfile.ZipFile(archive_source) as archive:
            topology = GtfsTopologyImporter(archive).import_topology()

    with open(arguments.output, "w", encoding="utf-8") as output_file:
        json.dump(topology, output_file, ensure_ascii=False, indent=2)
        output_file.write("\n")

    print(
        f"Generated {arguments.output}: "
        f"{len(topology['nodes'])} nodes, {len(topology['links'])} links"
    )


if __name__ == "__main__":
    main()
