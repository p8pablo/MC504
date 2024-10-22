// A Simple C program
#include "types.h"
#include "stat.h"
#include "user.h"

#define MAX_VERTICES 100
#define INF 1e9 // Representing infinite distance
// Graph structure
struct Graph
{
    int num_vertices;
    int adjacency_matrix[MAX_VERTICES][MAX_VERTICES]; // Adjacency matrix
};

// Function to create a graph with a specific number of vertices
struct Graph *create_graph(int num_vertices)
{
    struct Graph *graph = (struct Graph *)malloc(sizeof(struct Graph));
    graph->num_vertices = num_vertices;

    // Initialize adjacency matrix with INF (no direct connection)
    for (int i = 0; i < num_vertices; i++)
    {
        for (int j = 0; j < num_vertices; j++)
        {
            graph->adjacency_matrix[i][j] = (i == j) ? 0 : INF;
        }
    }

    return graph;
}

// Generate random graph
struct Graph *generate_random_graph()
{   
    struct Graph *graph = (struct Graph *)malloc(sizeof(struct Graph));
    return graph;
}

// Dijkstra
void dijkstra(struct Graph *graph, int start_vertex)
{
    return;
}

void cpu_bound_task()
{
    // Generate a random graph
    struct Graph *graph = generate_random_graph(); // TODO
    int start_vertex = 0;
    // Run Dijkstra's algorithm
    dijkstra(graph, start_vertex); // TODO
}

void io_bound_task()
{
    char filename[] = "testfile.txt";

    // Write 100 random lines to file
    for (int i = 0; i < 100; i++)
    {
        // write_random_line_to_file(filename); // TODO
    }

    // Permute lines
    // permute_lines(filename); // TODO

    // Delete file
    unlink(filename);
}
void run_experiment(int cpu_count, int io_count)
{
    for (int i = 0; i < cpu_count; i++)
    {
        if (fork() == 0)
        {
            cpu_bound_task();
            exit();
        }
    }

    for (int i = 0; i < io_count; i++)
    {
        if (fork() == 0)
        {
            io_bound_task();
            exit();
        }
    }

    // Wait for all processes to finish
    for (int i = 0; i < cpu_count + io_count; i++)
    {
        wait();
    }
}

// passing command line arguments

int main(int argc, char *argv[])
{
    printf(1, "My first xv6 program learnt at GFG\n");
    run_experiment(1, 1);
    exit();
}

// This code is contributed by sambhav228
