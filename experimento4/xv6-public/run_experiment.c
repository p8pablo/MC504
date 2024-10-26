// A Simple C program
#include "types.h"
#include "stat.h"
#include "user.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_VERTICES 201
#define INF 1e9 // Representing infinite distance
#define LINE_LENGTH 100
#define MAX_LINES 100
#define RAND_MAX_32 ((1U << 30) - 1)
int rseed = 1;

struct Graph
{
    int num_vertices;
    int adjacency_matrix[MAX_VERTICES][MAX_VERTICES]; // Adjacency matrix
};

// Function to generate a pseudo-random number
int rand()
{
    return (rseed = (rseed * 214013 + 2531011) & RAND_MAX_32) >> 15;
}

// Function to adapt random number generated into defined range
int randomrange(int lo, int hi)
{

    int range = hi - lo + 1;
    return (rand() % (range) + lo);
}

// Generate random graph
struct Graph *generate_random_graph(int num_vertices)
{

    // Allocating space
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

    // Generating a random number of edges
    int num_edges = randomrange(50, 400);

    // Adding num_edges new edges to the graph
    int j = 0;
    while (j < num_edges)
    {

        // Generate two random vertices to be connected
        int v1 = randomrange(0, num_vertices - 1);
        int v2 = randomrange(0, num_vertices - 1);

        // printf(1, "%d -> %d\n", v1, v2);
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

// Function to generate a random string of characters
void generate_random_string(char *str, int length)
{
    static const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    for (int i = 0; i < length - 1; i++)
    {
        int key = rand() % (sizeof(charset) - 1);
        str[i] = charset[key];
    }
    str[length - 1] = '\0'; // Null-terminate the string
}

// Function to write a random line to the file
void write_random_line_to_file(const char *filename)
{
    FILE *file = fopen(filename, "a"); // Open file in append mode
    if (file == NULL)
    {
        perror("Could not open file");
        return;
    }

    char line[LINE_LENGTH];
    generate_random_string(line, LINE_LENGTH); // Generate random string
    fprintf(file, "%s\n", line);               // Write the string to the file
    fclose(file);                              // Close the file
}

void cpu_bound_task()
{
    // Generate number of vertices
    int num_vertices = randomrange(100, 200);

    // Generate a random graph
    struct Graph *graph = generate_random_graph(num_vertices);

    int start_vertex = 0;

    // Run Dijkstra's algorithm
    dijkstra(graph, start_vertex);
}

void io_bound_task()
{
    char filename[] = "testfile.txt";

    // Write 100 random lines to file
    for (int i = 0; i < 100; i++)
    {
        write_random_line_to_file(filename); // TODO
    }

    // Permute lines
    permute_lines(filename); // TODO

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
            exit(0);
        }
    }

    for (int i = 0; i < io_count; i++)
    {
        if (fork() == 0)
        {
            io_bound_task();
            exit(0);
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
    exit(0);
}

// This code is contributed by sambhav228
