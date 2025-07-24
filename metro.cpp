#include <bits/stdc++.h>
using namespace std;

class Station {
public:
    unordered_map<string, int> neighbors;  //<Name of station, dist to reach this station>
};

class DijkstraNode {
public:
    string stationName;
    string pathSoFar;
    int totalCost;

    bool operator<(const DijkstraNode& other) const {
        return totalCost > other.totalCost; // for min-heap
    }
};

class PathNode {
public:
    string stationName;
    string pathSoFar;
    int totalDistance;
    int totalTime;
};

class MetroGraph {
public:
    unordered_map<string, Station*> allstations;

    ~MetroGraph() {
        for (auto& entry : allstations)
            delete entry.second;
    }

    int countStations() {
        return allstations.size();
    }

    int countConnections() {
        int connectionCount = 0;
        for (auto& entry : allstations)
            connectionCount += entry.second->neighbors.size();
        return connectionCount / 2;
    }

    void deleteStation(const string& name) {
        if (!hasStation(name)) return;
        for (auto& neighbor : allstations[name]->neighbors) {
            allstations[neighbor.first]->neighbors.erase(name);
        }
        delete allstations[name];
        allstations.erase(name);
    }

    void deleteConnection(const string& from, const string& to) {
        if (hasConnection(from, to)) {
            allstations[from]->neighbors.erase(to);
            allstations[to]->neighbors.erase(from);
        }
    }
    
    void addStation(const string& name) {
        allstations[name] = new Station();
    }

    bool hasStation(const string& name) {
        return allstations.find(name) != allstations.end();
    }

    void addConnection(const string& from, const string& to, int distance) {
        if (!hasStation(from) || !hasStation(to)) return;
        allstations[from]->neighbors[to] = distance;
        allstations[to]->neighbors[from] = distance;
    }

    bool hasConnection(const string& from, const string& to) {
        return hasStation(from) && hasStation(to) && allstations[from]->neighbors.count(to);
    }

    bool existsPath(string from, string to, unordered_map<string, bool>& visited) {
        if (hasConnection(from, to)) return true;
        visited[from] = true;
        for (auto& neighbor : allstations[from]->neighbors) {
            if (!visited[neighbor.first]) {
                if (existsPath(neighbor.first, to, visited)) return true;
            }
        }
        return false;
    }

    int calculateDijkstra(string start, string end, bool timeMode) {
        unordered_map<string, DijkstraNode> nodeMap;
        priority_queue<DijkstraNode> pq;

        for (auto& entry : allstations) {
            DijkstraNode node;
            node.stationName = entry.first;
            node.totalCost = (entry.first == start ? 0 : INT_MAX);
            node.pathSoFar = (entry.first == start ? start : "");
            nodeMap[entry.first] = node;
            pq.push(node);
        }

        while (!pq.empty()) {
            DijkstraNode current = pq.top(); 
            pq.pop();
            if (current.stationName == end) return current.totalCost;

            for (auto& neighbor : allstations[current.stationName]->neighbors) {
                // Calculate cost based on distance or time
                // For time, we assume 180 seconds for boarding and 80 seconds per 1 unit distance
                int cost = current.totalCost + (timeMode ? 180 + 80 * neighbor.second : neighbor.second);

                if (cost < nodeMap[neighbor.first].totalCost) {
                    nodeMap[neighbor.first].totalCost = cost;
                    nodeMap[neighbor.first].pathSoFar = current.pathSoFar + neighbor.first;
                    pq.push(nodeMap[neighbor.first]);
                }
            }
        }
        return -1;
    }

    string shortestPath(string start, string end, bool timeMode) {
       unordered_map<string, bool> visited;
       stack<PathNode> st;

       PathNode startNode{start, start + "  ", 0, 0};
       st.push(startNode);

       int minValue = INT_MAX;
       string resultPath = "";

       while (!st.empty()) {
           PathNode current = st.top();
           st.pop();

           if (visited[current.stationName]) continue;
           visited[current.stationName] = true;

           int currentValue = timeMode ? current.totalTime : current.totalDistance;

           if (current.stationName == end && currentValue < minValue) {
            minValue = currentValue;
            resultPath = current.pathSoFar;
            continue;
            }

        for (auto& neighbor : allstations[current.stationName]->neighbors) {
            if (!visited[neighbor.first]) {
                PathNode next;
                next.stationName = neighbor.first;
                next.pathSoFar = current.pathSoFar + neighbor.first + "  ";

                if (timeMode) {
                    next.totalTime = current.totalTime + 180 + 80 * neighbor.second;
                    next.totalDistance = current.totalDistance;
                } else {
                    next.totalDistance = current.totalDistance + neighbor.second;
                    next.totalTime = current.totalTime;
                }

                st.push(next);
            }
        }
    }

    if (timeMode) resultPath += to_string((int)floor((double)minValue / 60));  // minutes
    else resultPath += to_string(minValue); // km

    return resultPath;
    }

    vector<string> parsePathWithInterchanges(const string& str) {
    vector<string> tokens;
    regex re(" {2,}");  // Split by 2 or more spaces
    sregex_token_iterator it(str.begin(), str.end(), re, -1);
    sregex_token_iterator end;

    while (it != end) {
        tokens.push_back(*it++);
    }

    if (tokens.size() < 2) return {};

    string finalCost = tokens.back();  // distance or time
    tokens.pop_back();                 // remove cost

    int totalStations = tokens.size();
    int interchanges = max(0, totalStations - 1);

    vector<string> result = tokens;
    result.push_back(to_string(interchanges));
    result.push_back(finalCost);
    return result;
    }

    void showMetroMap() {
        cout << "\t Delhi Metro Map\n";
        cout << "\t------------------\n";
        cout << "----------------------------------------------------\n";
        for (auto& entry : allstations) {
            cout << entry.first << " =>\n";
            for (auto& neighbor : entry.second->neighbors) {
                cout << "\t" << neighbor.first << "\t";
                if (neighbor.first.length() < 16) cout << "\t";
                if (neighbor.first.length() < 8) cout << "\t";
                cout << neighbor.second << "\n";
            }
        }
        cout << "\t------------------\n";
        cout << "---------------------------------------------------\n";
    }

    void listAllStations() {
        cout << "\n***********************************************************************\n\n";
        int idx = 1;
        for (auto& entry : allstations) {
            cout << idx++ << ". " << entry.first << endl;
        }
        cout << "\n***********************************************************************\n\n";
    }

    string matchStationInput(string userInput) {
        transform(userInput.begin(), userInput.end(), userInput.begin(), ::tolower);
        for (auto& pair : allstations) {
            string stationLower = pair.first;
            transform(stationLower.begin(), stationLower.end(), stationLower.begin(), ::tolower);
            if (stationLower.find(userInput) != string::npos)
                return pair.first;
        }
        return "";
    }

    void initializeMetroMap() {
        addStation("Noida Sector 62");
        addStation("Botanical Garden");
        addStation("Yamuna Bank");
        addStation("Rajiv Chowk");
        addStation("Vaishali");
        addStation("Moti Nagar");
        addStation("Janak Puri West");
        addStation("Dwarka Sector 21");
        addStation("Huda City Center");
        addStation("Saket");
        addStation("Vishwavidyalaya");
        addStation("Chandni Chowk");
        addStation("New Delhi");
        addStation("AIIMS");
        addStation("Shivaji Stadium");
        addStation("DDS Campus");
        addStation("IGI Airport");
        addStation("Rajouri Garden");
        addStation("Netaji Subhash Place");
        addStation("Punjabi Bagh West");

        addConnection("Noida Sector 62", "Botanical Garden", 8);
        addConnection("Botanical Garden", "Yamuna Bank", 10);
        addConnection("Yamuna Bank", "Vaishali", 8);
        addConnection("Yamuna Bank", "Rajiv Chowk", 6);
        addConnection("Rajiv Chowk", "Moti Nagar", 9);
        addConnection("Moti Nagar", "Janak Puri West", 7);
        addConnection("Janak Puri West", "Dwarka Sector 21", 6);
        addConnection("Huda City Center", "Saket", 15);
        addConnection("Saket", "AIIMS", 6);
        addConnection("AIIMS", "Rajiv Chowk", 7);
        addConnection("Rajiv Chowk", "New Delhi", 1);
        addConnection("New Delhi", "Chandni Chowk", 2);
        addConnection("Chandni Chowk", "Vishwavidyalaya", 5);
        addConnection("New Delhi", "Shivaji Stadium", 2);
        addConnection("Shivaji Stadium", "DDS Campus", 7);
        addConnection("DDS Campus", "IGI Airport", 8);
        addConnection("Moti Nagar", "Rajouri Garden", 2);
        addConnection("Punjabi Bagh West", "Rajouri Garden", 2);
        addConnection("Punjabi Bagh West", "Netaji Subhash Place", 3);
    }
};

int main() {
    MetroGraph metro;
    metro.initializeMetroMap();

    int choice;
    while (true) {
        cout << "\n~~ LIST OF ACTIONS ~~\n";
        cout << "1. List All Stations\n";
        cout << "2. Show Metro Map\n";
        cout << "3. Get Shortest Distance\n";
        cout << "4. Get Shortest Time\n";
        cout << "5. Get Shortest Path (Distance)\n";
        cout << "6. Get Shortest Path (Time)\n";
        cout << "7. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;
        cin.ignore();

        if (choice == 7) break;

        unordered_map<string, bool> visited;

        switch (choice) {
            case 1:
                metro.listAllStations();
                break;

            case 2:
                metro.showMetroMap();
                break;

            case 3: {
                string from, to;
                cout << "\nEnter source station: ";
                getline(cin, from);
                cout << "Enter destination station: ";
                getline(cin, to);

                from = metro.matchStationInput(from);
                to = metro.matchStationInput(to);

                if (from.empty() || to.empty()) {
                    cout << "Invalid station name(s). Try again.\n";
                    continue;
                }

                if (!metro.existsPath(from, to, visited))
                    cout << "No path found.\n";
                else
                    cout << "Shortest Distance: " << metro.calculateDijkstra(from, to, false) << " KM\n";
                break;
            }

            case 4: {
                string from, to;
                cout << "\nEnter source station: ";
                getline(cin, from);
                cout << "Enter destination station: ";
                getline(cin, to);

                from = metro.matchStationInput(from);
                to = metro.matchStationInput(to);

                if (from.empty() || to.empty()) {
                    cout << "Invalid station name(s). Try again.\n";
                    continue;
                }

                if (!metro.existsPath(from, to, visited))
                    cout << "No path found.\n";
                else
                    cout << "Shortest Time: " << metro.calculateDijkstra(from, to, true) / 60 << " minutes\n";
                break;
            }

            case 5: {
                string from, to;
                cout << "\nEnter source station: ";
                getline(cin, from);
                cout << "Enter destination station: ";
                getline(cin, to);

                from = metro.matchStationInput(from);
                to = metro.matchStationInput(to);

                if (from.empty() || to.empty()) {
                    cout << "Invalid station name(s). Try again.\n";
                    continue;
                }

                if (!metro.existsPath(from, to, visited)) {
                    cout << "No path found.\n";
                } else {
                    auto path = metro.parsePathWithInterchanges(metro.shortestPath(from, to,false));
                    // ex: path = ["Rajiv Chowk", "Central Secretariat", "Kashmere Gate", "2", "12"]
                    // i.e. path = [stations in strings, interchanges count, distance in KM]

                    cout << "Path:\n";
                    for (int i = 0; i < path.size() - 2; i++)
                        cout << path[i] << " -> ";
                    cout << "\nDistance: " << path.back() << " KM, Interchanges: " << *(path.end() - 2) << "\n";
                }
                break; 
            }

            case 6: {
                string from, to;
                cout << "\nEnter source station: ";
                getline(cin, from);
                cout << "Enter destination station: ";
                getline(cin, to);

                from = metro.matchStationInput(from);
                to = metro.matchStationInput(to);

                if (from.empty() || to.empty()) {
                    cout << "Invalid station name(s). Try again.\n";
                    continue;
                }

                if (!metro.existsPath(from, to, visited)) {
                    cout << "No path found.\n";
                } else {
                    auto path = metro.parsePathWithInterchanges(metro.shortestPath(from, to, true));
                    // ex: path = ["Rajiv Chowk", "Central Secretariat", "Kashmere Gate", "2", "12"]
                    // i.e. path = [stations in strings, interchanges count, time in minutes]
                    cout << "Path:\n";
                    for (int i = 0; i < path.size() - 2; i++)
                        cout << path[i] << " -> ";
                    cout << "\nTime: " << path.back() << " minutes, Interchanges: " << *(path.end() - 2) << "\n";
                }
                break;
            }

            default:
                cout << "Invalid choice.\n";
        }
    }
    return 0;
}
