#include <bits/stdc++.h>
using namespace std;

class Station {
public:
    unordered_map<string, int> neighbors;
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
    unordered_map<string, Station*> stations;

    ~MetroGraph() {
        for (auto& entry : stations)
            delete entry.second;
    }

    int countStations() {
        return stations.size();
    }

    int countConnections() {
        int connectionCount = 0;
        for (auto& entry : stations)
            connectionCount += entry.second->neighbors.size();
        return connectionCount / 2;
    }

    void deleteStation(const string& name) {
        if (!hasStation(name)) return;
        for (auto& neighbor : stations[name]->neighbors) {
            stations[neighbor.first]->neighbors.erase(name);
        }
        delete stations[name];
        stations.erase(name);
    }

    void deleteConnection(const string& from, const string& to) {
        if (hasConnection(from, to)) {
            stations[from]->neighbors.erase(to);
            stations[to]->neighbors.erase(from);
        }
    }
    
    void addStation(const string& name) {
        stations[name] = new Station();
    }

    void addConnection(const string& from, const string& to, int distance) {
        if (!hasStation(from) || !hasStation(to)) return;
        stations[from]->neighbors[to] = distance;
        stations[to]->neighbors[from] = distance;
    }

    bool hasStation(const string& name) {
        return stations.find(name) != stations.end();
    }

    bool hasConnection(const string& from, const string& to) {
        return hasStation(from) && hasStation(to) && stations[from]->neighbors.count(to);
    }

    bool existsPath(string from, string to, unordered_map<string, bool>& visited) {
        if (hasConnection(from, to)) return true;
        visited[from] = true;
        for (auto& neighbor : stations[from]->neighbors) {
            if (!visited[neighbor.first]) {
                if (existsPath(neighbor.first, to, visited)) return true;
            }
        }
        return false;
    }

    int calculateDijkstra(string start, string end, bool timeMode) {
        unordered_map<string, DijkstraNode> nodeMap;
        priority_queue<DijkstraNode> pq;

        for (auto& entry : stations) {
            DijkstraNode node;
            node.stationName = entry.first;
            node.totalCost = (entry.first == start ? 0 : INT_MAX);
            node.pathSoFar = (entry.first == start ? start : "");
            nodeMap[entry.first] = node;
            pq.push(node);
        }

        while (!pq.empty()) {
            DijkstraNode current = pq.top(); pq.pop();
            if (current.stationName == end) return current.totalCost;

            for (auto& neighbor : stations[current.stationName]->neighbors) {
                int cost = current.totalCost + (timeMode ? 120 + 40 * neighbor.second : neighbor.second);
                if (cost < nodeMap[neighbor.first].totalCost) {
                    nodeMap[neighbor.first].totalCost = cost;
                    nodeMap[neighbor.first].pathSoFar = current.pathSoFar + neighbor.first;
                    pq.push(nodeMap[neighbor.first]);
                }
            }
        }
        return -1;
    }

    string shortestDistancePath(string start, string end) {
        unordered_map<string, bool> visited;
        stack<PathNode> st;
        PathNode startNode{start, start + "  ", 0, 0};
        st.push(startNode);
        int minDist = INT_MAX;
        string resultPath = "";

        while (!st.empty()) {
            PathNode current = st.top(); st.pop();
            if (visited[current.stationName]) continue;
            visited[current.stationName] = true;
            if (current.stationName == end && current.totalDistance < minDist) {
                minDist = current.totalDistance;
                resultPath = current.pathSoFar;
                continue;
            }
            for (auto& neighbor : stations[current.stationName]->neighbors) {
                if (!visited[neighbor.first]) {
                    PathNode next{neighbor.first, current.pathSoFar + neighbor.first + "  ", current.totalDistance + neighbor.second, 0};
                    st.push(next);
                }
            }
        }
        resultPath += to_string(minDist);
        return resultPath;
    }

    string shortestTimePath(string start, string end) {
        unordered_map<string, bool> visited;
        stack<PathNode> st;
        PathNode startNode{start, start + "  ", 0, 0};
        st.push(startNode);
        int minTime = INT_MAX;
        string resultPath = "";

        while (!st.empty()) {
            PathNode current = st.top(); st.pop();
            if (visited[current.stationName]) continue;
            visited[current.stationName] = true;
            if (current.stationName == end && current.totalTime < minTime) {
                minTime = current.totalTime;
                resultPath = current.pathSoFar;
                continue;
            }
            for (auto& neighbor : stations[current.stationName]->neighbors) {
                if (!visited[neighbor.first]) {
                    PathNode next;
                    next.stationName = neighbor.first;
                    next.pathSoFar = current.pathSoFar + neighbor.first + "  ";
                    next.totalTime = current.totalTime + 120 + 40 * neighbor.second;
                    st.push(next);
                }
            }
        }
        resultPath += to_string((int)ceil((double)minTime / 60));
        return resultPath;
    }

    vector<string> parsePathWithInterchanges(const string& str) {
        vector<string> result;
        stringstream ss(str);
        string token;
        vector<string> tokens;
        while (getline(ss, token, ' ')) if (!token.empty()) tokens.push_back(token);
        result.push_back(tokens[0]);
        int interchangeCount = 0;
        for (int i = 1; i < (int)tokens.size() - 1; ++i) {
            size_t idx = tokens[i].find('~');
            string line1 = tokens[i].substr(idx + 1);
            if (line1.size() == 2) {
                string prev = tokens[i - 1].substr(tokens[i - 1].find('~') + 1);
                string next = tokens[i + 1].substr(tokens[i + 1].find('~') + 1);
                if (prev == next) result.push_back(tokens[i]);
                else {
                    result.push_back(tokens[i] + " ==> " + tokens[i + 1]);
                    ++i; interchangeCount++;
                }
            } else result.push_back(tokens[i]);
        }
        result.push_back(to_string(interchangeCount));
        result.push_back(tokens.back());
        return result;
    }

    void showMetroMap() {
        cout << "\t Delhi Metro Map\n";
        cout << "\t------------------\n";
        cout << "----------------------------------------------------\n";
        for (auto& entry : stations) {
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
        for (auto& entry : stations) {
            cout << idx++ << ". " << entry.first << endl;
        }
        cout << "\n***********************************************************************\n\n";
    }

    string matchStationInput(string userInput) {
        transform(userInput.begin(), userInput.end(), userInput.begin(), ::tolower);
        for (auto& pair : stations) {
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
                cout << "Enter source station: ";
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
                cout << "Enter source station: ";
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
                cout << "Enter source station: ";
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
                    auto path = metro.parsePathWithInterchanges(metro.shortestDistancePath(from, to));
                    cout << "Path:\n";
                    for (int i = 0; i < (int)path.size() - 2; i++)
                        cout << path[i] << " -> ";
                    cout << "\nDistance: " << path.back() << " KM, Interchanges: " << *(path.end() - 2) << "\n";
                }
                break;
            }

            case 6: {
                string from, to;
                cout << "Enter source station: ";
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
                    auto path = metro.parsePathWithInterchanges(metro.shortestTimePath(from, to));
                    cout << "Path:\n";
                    for (int i = 0; i < (int)path.size() - 2; i++)
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