#include "TSP.hpp"

/**
 * Displays the edges and total distance of the tour.
 * Each edge is printed in the format: "EDGE start_id -> end_id | WEIGHT: weight".
 */
void TSP::Tour::display() const {
  for (size_t i = 1; i < path.size(); i++) {
    std::cout << "EDGE " << path[i-1].id << " -> " << path[i].id << " | WEIGHT : " << weights[i] << std::endl;
  }
  std::cout << "TOTAL DISTANCE: " << total_distance << std::endl;
}

/**
 * Reads a .tsp file and constructs a list of cities as nodes.
 * The file should have a section labeled "NODE_COORD_SECTION" followed by lines with the format: ID x-coordinate y-coordinate.
 * 
 * @param filename The path to the TSP file.
 * @return A list of `Node` objects representing cities and their coordinates.
 * @throws std::runtime_error If the file cannot be read or parsed.
 * 
 * @pre The file specified by `filename` exists and follows the TSP format.
 */
std::list<Node> TSP::constructCities(const std::string& filename) {
  // Read past metadata
  std::ifstream fin(filename);
  if (fin.fail()) {
    std::cerr << "ERROR: Could not read file: " << filename << std::endl;
    throw std::runtime_error("Failed to read file. Terminating.");
  }

  std::string line;
  do { std::getline(fin, line); }
  while (line.find("NODE_COORD_SECTION"));

  // Read data from file into Node list "cities"
  std::list<Node> cities;
  size_t ID;
  double x, y;
  while (!fin.eof()){
    if (!(fin >> ID >> x >> y)) break;
    cities.push_back(Node(ID, x, y));
  }
  return cities;
}

/**
 * Constructs a tour using the nearest neighbor heuristic for the traveling salesperson problem (TSP).
 * Starting from the city of the given ID, it iteratively visits the nearest unvisited city and returns to the start.
 *
 * @param cities A list of `Node` objects representing the cities to be visited.
 * @param start_id The unique identifier of the starting city.
 * @return A `TSP::Tour` object representing the path, edge weights, and total distance of the computed tour.
 *
 * @pre `start_id` must be a valid city ID within the range of IDs in `cities`.
 * @note The ith weight in `weights` is the distance traveled from the previous (i-1) city to the current i city (represented by the ith city in the returned Tour.path)
 *       As such, the first weight will ALWAYS equal 0, since there is no edge from the start city to itself
 *
 */

/*
  Gameplan:

  Node: An x,y coordinate with an identifier
    - contains a distance calculator -> returns size_t

  Tour contains:
  Vector of Nodes called path
  Vector of distances called weight
  a size_t of total distance

  Tour normal definition: A tour is a set of nodes that store the total distance
  as well as the indiviual one

Notes:
  Inital weights = 0 for the inital city -> so everytime we add a city we need to add a value, # of path = # of weights
  Total distance needs the return distance back to the inital one as well
  Maybe create a vector to store the cities already visited. -> add the id and check maybw

Inital design:
Create a new tour object
create a pointer of some kind to keep check of where the salesperson is
int check -> end program if int = cities (not including the return)
if not //Check if cities contains a node with the start_id
  return blank

add first item in list into the tour.path
iterate through the rest of the list in cities  and calculate the distance between each one
  // need to make sure we dont go back to the same cities again -> need a marker
pick the one that has the smaller distance using the node function
add that node into tour.path, add the length into tour.weights, add to the tour.total distance
update the pointer to the new node

repeat until all nodes are exhausted (marker is all marked)

add the return distance back to the city
(?) add the intial city back into the tour.path and tour.weight? => Yes

return tour;

Viszualiation:

Cities : 2,3,5,4,1
Start: 1

Tour:
Path: {1}
Weight: {0}
Total:
-----------------------------------------------------------------------------------------
New Notes:
No need to have "marker", can just delete from cities
Find the starting city by iterating and finding matching id
No need to have a new vector for cities visited -> already defined in tour path

New Design:
Create a tour obj and a pointer node for current
Find the starting id and push it into the tour obj along with it's distance
erase the starting id from cities and push back into the final

we start the problem:
while(cities not empty)
{
  2nd pointer representing another city
  for(cities.begin next -> cities end)
  {
    check min and set the new min
    set the new ptr
  }
  push the new city into the tour path, update weights and total dist
  set a new one and erase it from cities
}
return to starting
end;

visual for clairty:

cities: 1, 2, 5, 3, 4
starting 2;
Tour:
Path: 2, 1, 3, 4, 5

First test:
320 ms - JP
*/
TSP::Tour TSP::nearestNeighbor(std::list<Node> cities, const size_t &start_id)
{
  // Find the starting city through iterating cities and compare it to the param start id
  auto start_it = cities.end();
  for (auto it = cities.begin(); it != cities.end(); ++it)
  {
    if (it->id == start_id)
    {
      start_it = it;
      break;
    }
  }
  Node current = *start_it;
  cities.erase(start_it);

  TSP::Tour tour;
  // Inital conditions
  tour.path.push_back(current); // Add the starting city to the tour
  tour.weights.push_back(0);    // Initial weight is 0
  tour.total_distance = 0;

  while (!cities.empty())
  {
    // Find the nearest unvisited city
    auto nearest_it = cities.begin();
    size_t min_distance = current.distance(*nearest_it);

    for (auto it = std::next(cities.begin()); it != cities.end(); ++it)
    {
      // Check mins
      size_t dist = current.distance(*it);
      if (dist < min_distance)
      {
        min_distance = dist;
        nearest_it = it;
      }
    }

    // Update tour
    tour.path.push_back(*nearest_it);
    tour.weights.push_back(min_distance);
    tour.total_distance += min_distance;

    // Move to next city
    current = *nearest_it;
    cities.erase(nearest_it);
  }

  // Return to starting city
  size_t return_distance = current.distance(tour.path.front());
  tour.path.push_back(tour.path.front());
  tour.weights.push_back(return_distance);
  tour.total_distance += return_distance;

  return tour;
}