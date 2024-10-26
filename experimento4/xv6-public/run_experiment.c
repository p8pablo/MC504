// A Simple C program
#include "types.h"
#include "stat.h"
#include "user.h"
// #include <time.h>
// #include <fcntl.h>
// #include <unistd.h>

#define MAX_VERTICES 201
#define INF 1e9 // Representing infinite distance
#define LINE_LENGTH 101
#define MAX_LINES 100
#define RAND_MAX_32 ((1U << 30) - 1)
#define O_RDONLY  0
#define O_WRONLY  1
#define O_RDWR    2
#define O_CREAT  0x200
#define O_APPEND  0x400
#define TOTAL_LINES 100
#define SWAPS 50

int rseed = 1;
char lines[TOTAL_LINES][LINE_LENGTH];


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
void write_random_line_to_file(const char *filename, int fd)
{
    char line[LINE_LENGTH];

    generate_random_string(line, 100);

    // Append a newline character at the end
    line[100] = '\n';

    if (write(fd, line, LINE_LENGTH) != LINE_LENGTH) {
        printf(1, "Could not write on file.\n");
        return;
    }

}

// Function to update file with 50 new permutations
void permute_lines(const char *filename)
{
 
    // Open the file for reading and writing
    int fd = open(filename, O_RDWR | O_CREAT);
    if(fd < 0){
        printf(1, "Could not open file for permutation\n");
        return;
    }

    // Read all lines into an array
    for(int i = 0; i < TOTAL_LINES; i++) read(fd, lines[i], LINE_LENGTH);

    // Perform 50 swaps
    for(int i = 0; i < SWAPS; i++) {
        int a = randomrange(0, 99);
        int b = randomrange(0, 99);
        
        char temp[LINE_LENGTH];
        for(int j = 0; j < LINE_LENGTH; j++) temp[j] = lines[a][j];
        for(int j = 0; j < LINE_LENGTH; j++) lines[a][j] = lines[b][j];
        for(int j = 0; j < LINE_LENGTH; j++) lines[b][j] = temp[j];
    }

    // Reopen the file to rewrite all changed lines
    int fd_write = open(filename, O_WRONLY | O_CREAT);
    if(fd_write < 0){
        printf(1, "Error: Cannot open file for writing\n");
        return;
    }

    // Rewriting all lines into the file
    for(int i = 0; i < TOTAL_LINES; i++) {
        if(write(fd_write, lines[i], LINE_LENGTH) != LINE_LENGTH){
            printf(1, "Error: Failed to write line %d\n", i+1);
            close(fd_write);
            return;
        }
    }

    // Close the file descriptor
    close(fd);
}

void print_float(int x){
    // Integer and fractional parts for display
    int norm_factor = 1000;
    int integer_part = x / norm_factor;
    int fractional_part = x % norm_factor;

    // Print results without formatted padding
    printf(1, "%d.%d\n", integer_part, fractional_part);
}

int calculate_io_latency(int sum_latencies, int count_latencies, int min_io_latency, int max_io_latency){

    int norm_factor = 1000;

    int avg_latency_scaled = (sum_latencies * norm_factor) / (count_latencies);

    int norm_io_latency = norm_factor - ((avg_latency_scaled - (min_io_latency * norm_factor)) / (max_io_latency - min_io_latency));

    return norm_io_latency;
    // printf(1, "sum: %d, min: %d, max: %d\n", sum_latencies, min_io_latency, max_io_latency);
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

    int fd = open(filename, O_WRONLY | O_CREAT | O_APPEND);
    if (fd < 0) {
        printf(1, "Could open file\n");
        return;
    }

    // Write 100 random lines to file
    for (int i = 0; i < 100; i++)
    {
        write_random_line_to_file(filename, fd);
    }

    close(fd);

    // Permutating lines
    permute_lines(filename);

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
    # define MAX_IO 20

    // Flags to help calculate i/o latency system calls
    int min_io_latency = INF;
    int max_io_latency = 0;
    int sum_latencies = 0;
    int count_latencies = 0;

    for (int i = 0; i < io_count; i++)
    {

        int start_io_uptime = uptime();
        // printf(1, "%d) %d - ", i, start_io_uptime);

        if (fork() == 0)
        {
            
            io_bound_task();
        
            exit();
        }

        wait();

        int end_io_uptime = uptime();

        int diff = end_io_uptime - start_io_uptime;

        // printf(1, "%d (%d)\n", end_io_uptime, diff);
        sum_latencies += diff;
        count_latencies ++;

        if(diff > max_io_latency) max_io_latency = diff;
        if(diff < min_io_latency) min_io_latency = diff;
        
        wait();
    }

    printf(1, "Latencia de I/O Normalizada:\n");
    print_float(calculate_io_latency(sum_latencies, count_latencies, min_io_latency, max_io_latency));


    // Wait for all processes to finish
    for (int i = 0; i < cpu_count + io_count; i++)
    {
        wait();
    }
}

// passing command line arguments

int main(int argc, char *argv[])
{
    printf(1, "Comecando os Experimentos: \n");
    run_experiment(1, 20);
    exit();
}

// This code is contributed by sambhav228
