#include <iostream>
#include <vector>
#include <queue>
#include <limits>
#include <cmath>
#include <iomanip>

using namespace std;

vector<vector<int>> givenEdges;

int getGreenLightTime(int numCars)
{
    // assume green light time includes yellow light time
    // assume average car body length is 4.5 meters
    // assume average car speed is 30 km/h = 8.333 m/s
    // assume average car gap is 2 meters
    // then the time needed for N cars to pass is N*(4.5+2)/(8.333) seconds
    return ceil(abs(numCars) * (4.5 + 2) / 8.333);
}

int getRedLightTime(int numCars, int totalTime)
{
    return totalTime - getGreenLightTime(numCars);
}

struct Edge
{
    int source, destination, capacity, flow;
};

class Graph
{
    int numVertices;
    vector<Edge> edges;
    vector<vector<int>> adjacencyList;

public:
    Graph(int V)
    {
        numVertices = V;
        adjacencyList.resize(numVertices);
    }

    void addEdge(int source, int destination, int capacity)
    {
        givenEdges.push_back({source, destination, capacity});
        Edge e1 = {source, destination, capacity, 0};
        Edge e2 = {destination, source, 0, 0};
        edges.push_back(e1);
        edges.push_back(e2);
        adjacencyList[source].push_back(edges.size() - 2);      // index
        adjacencyList[destination].push_back(edges.size() - 1); // index
    }

    bool bfs(int source, int sink, vector<int> &parent)
    {
        vector<bool> visited(numVertices, false);
        queue<int> q;
        q.push(source);
        visited[source] = true;
        parent[source] = -1;

        while (!q.empty())
        {
            int u = q.front();
            q.pop();

            for (int i : adjacencyList[u])
            {
                Edge &e = edges[i];
                if (!visited[e.destination] && e.capacity > e.flow)
                {
                    q.push(e.destination);
                    parent[e.destination] = i;
                    visited[e.destination] = true;
                }
            }
        }

        return visited[sink];
    }

    int fordFulkerson(int source, int sink)
    {
        vector<int> parent(numVertices);
        int maxFlow = 0;

        while (bfs(source, sink, parent))
        {
            int pathFlow = numeric_limits<int>::max();

            for (int v = sink; v != source; v = edges[parent[v]].source)
            {
                int i = parent[v];
                pathFlow = min(pathFlow, edges[i].capacity - edges[i].flow);
            }

            for (int v = sink; v != source; v = edges[parent[v]].source)
            {
                int i = parent[v];
                edges[i].flow += pathFlow;
                edges[i ^ 1].flow -= pathFlow;
            }

            maxFlow += pathFlow;
        }

        return maxFlow;
    }

    void reduceFlow(int source, int sink)
    {
        int maxFlow = fordFulkerson(source, sink);

        for (int i = 0; i < edges.size(); i += 2)
        {
            Edge &e = edges[i];
            int originalFlow = e.flow;

            while (true)
            {

                e.flow = originalFlow - 1;
                int newFlow = fordFulkerson(source, sink);

                if (newFlow != maxFlow)
                {
                    e.flow = originalFlow;
                }
                break;
            }
        }
    }

    void printEdges()
    {

        cout << "\n\nGiven edges after minimizing the flow without affecting the maximum flow: \n";

        int k = 1;
        for (const Edge &e : edges)
        {
            for (int i = 0; i < givenEdges.size(); i++)
            {
                if (givenEdges[i][0] == e.source && givenEdges[i][1] == e.destination)
                {
                    float timeSavedRatio = 1.00 * (getGreenLightTime(givenEdges[i][2]) - getGreenLightTime(e.flow)) / getGreenLightTime(givenEdges[i][2]);
                    if (timeSavedRatio > 1 || timeSavedRatio < 0)
                    {
                        timeSavedRatio = 1;
                    }

                    cout << fixed << setprecision(3) << k++ << "\tSRC: " << e.source << ", DEST: " << e.destination << ", Flow: " << e.flow << ", Req Green Light Time: " << getGreenLightTime(e.flow) << " sec, Time saved for Pedestrians: " << getGreenLightTime(givenEdges[i][2]) - getGreenLightTime(e.flow) << ", Ratio of Time Saved: " << timeSavedRatio << endl;
                    givenEdges[i][0] = -1;
                    givenEdges[i][1] = -1;
                }
            }
        }

        cout << "\n";
    }
};

void runAll(Graph g)
{

    int source = 0;
    int sink = 5;

    // Run the Ford-Fulkerson algorithm to find the maximum flow
    int maxFlow = g.fordFulkerson(source, sink);

    // Reduce the flow on each edge
    g.reduceFlow(source, sink);

    cout << "\nMaximum flow: " << maxFlow;

    // Print the source, destination, and flow of each edge
    g.printEdges();
}

int main()
{

    cout << "\nApplications:";
    cout << "\n1- Saving time for pedesterians and reducing wasted green light time for cars";
    cout << "\n2- Reducing unnecessary lanes and road costs";
    cout << "\n3- Finding each road minimum flow for a total desired maximum flow from SRC to DEST";
    cout << "\n4- Can dynamically adapt and avoid accidents or road closures through setting edge capacity to zero";
    cout << "\n5- Can use clusters to save Yellow Light time. (Soltted vs Unslotted timing)";
    cout << "\n6- Can make it easier for emergency vehicles to pass since car spread crossings are minimized (especially with clusters slotted version), and also all opposite direction roads are empty at red light\n\n\n";

    cout << "\nAssumptions:";
    cout << "\n1- Average car body length is 4.5 meters";
    cout << "\n2- Average car speed is 30 km/h = 8.333 m/s";
    cout << "\n3- Average car gap is 2 meters";
    cout << "\n4- Single lane roads. Can easily support multi-lane through dividing the flow by the number of lanes\n\n\n";

    cout << "\n\nExample of 6 roads of flow 20:\n";
    Graph g1(6);
    g1.addEdge(0, 1, 20);
    g1.addEdge(0, 2, 20);
    g1.addEdge(1, 2, 20);
    g1.addEdge(1, 3, 20);
    g1.addEdge(2, 1, 20);
    g1.addEdge(2, 4, 20);
    g1.addEdge(3, 2, 20);
    g1.addEdge(3, 5, 20);
    g1.addEdge(4, 3, 20);
    g1.addEdge(4, 5, 20);
    runAll(g1);

    cout << "\n\n\n";

    cout << "\n\nExample of 6 roads of different flows:\n";
    Graph g2(6);
    g2.addEdge(0, 1, 16);
    g2.addEdge(0, 2, 13);
    g2.addEdge(1, 2, 10);
    g2.addEdge(1, 3, 12);
    g2.addEdge(2, 1, 4);
    g2.addEdge(2, 4, 14);
    g2.addEdge(3, 2, 9);
    g2.addEdge(3, 5, 20);
    g2.addEdge(4, 3, 7);
    g2.addEdge(4, 5, 4);
    runAll(g2);

    return 0;
}
