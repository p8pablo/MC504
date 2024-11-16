#include "types.h"
#include "stat.h"
#include "user.h"

#define MAX_VERTICES 201
#define INF 1e9 // Representing infinite distance

int rseed = 1;
#define RAND_MAX_32 ((1U << 30) - 1)
int total_m_over_time = 0;

struct Graph
{
    int num_vertices;
    int adjacency_matrix[MAX_VERTICES][MAX_VERTICES]; // Adjacency matrix
};

// Function to generate a pseudo-random number
int random()
{
    return (rseed = (rseed * 214013 + 2531011) & RAND_MAX_32) >> 15;
}

// Function to adapt random number generated into defined range
int randomrange(int lo, int hi)
{

    int range = hi - lo + 1;
    return (random() % (range) + lo);
}

// Generate random graph
struct Graph *generate_random_graph(int num_vertices)
{

    // using uptime() to calculate memory overhead timings
    int start_allocation_time = uptime();
    
    // Allocating space
    struct Graph *graph = (struct Graph *)malloc(sizeof(struct Graph));
    
    // Calculating time to allocate
    total_m_over_time += uptime() - start_allocation_time;

    graph->num_vertices = num_vertices;

    // Initialize adjacency matrix with INF (no direct connection)
    for (int i = 0; i < num_vertices; i++)
    {
        for (int j = 0; j < num_vertices; j++)
        {
            graph->adjacency_matrix[i][j] = (i == j) ? 0 : INF;
        }
    }

    // Generating a random number of edges
    int num_edges = randomrange(50, 400);

    // Adding num_edges new edges to the graph
    int j = 0;
    while (j < num_edges)
    {

        // Generate two random vertices to be connected
        int v1 = randomrange(0, num_vertices - 1);
        int v2 = randomrange(0, num_vertices - 1);

        // Assuring the edge does not connect a node to itself
        if (v1 != v2)
        {
            graph->adjacency_matrix[v1][v2] = 1;
            j++;
        }
    }

    return graph;
}

// Minimum distance to be used for Dijkstra
int minDistance(int dist[], int sptSet[], int num_vertices)
{
    int min = INF, min_index = -1;
    for (int v = 0; v < num_vertices; v++)
    {
        if (sptSet[v] == 0 && dist[v] <= min)
        {
            min = dist[v];
            min_index = v;
        }
    }
    return min_index;
}

// Dijkstra
void dijkstra(struct Graph *graph, int start_vertex)
{
    int dist[MAX_VERTICES];

    int num_vertices = graph->num_vertices;
    int sptSet[MAX_VERTICES];

    for (int i = 0; i < num_vertices; i++)
    {
        dist[i] = INF;
        sptSet[i] = 0;
    }

    dist[start_vertex] = 0;

    for (int count = 0; count < num_vertices - 1; count++)
    {
        int u = minDistance(dist, sptSet, num_vertices);
        sptSet[u] = 1;

        for (int v = 0; v < num_vertices; v++)
        {
            if (!sptSet[v] && graph->adjacency_matrix[u][v] &&
                dist[u] != INF && dist[u] + graph->adjacency_matrix[u][v] < dist[v])
            {
                dist[v] = dist[u] + graph->adjacency_matrix[u][v];
            }
        }
    }
}

int cpu_bound_task()
{
    for (int i = 0; i < 1000; i++){
        // Generate number of vertices
        int num_vertices = randomrange(100, 200);

        // Generate a random graph
        struct Graph *graph = generate_random_graph(num_vertices);

        int start_vertex = 0;

        // Calculating time to deallocate
        int start_time_free = uptime();
        free(graph);
        total_m_over_time += uptime() - start_time_free;
        
        // Run Dijkstra's algorithm
        dijkstra(graph, start_vertex);
    }

    return total_m_over_time;
}