#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>

#define MAX_CITIES 100
#define MAX_ROADS 1000

// Generate random roads between cities
void generate_roads(int num_cities, int num_roads, int roads[][3]) {
    srand(time(NULL));
    for (int i = 0; i < num_roads; i++) {
        int city1 = rand() % num_cities;
        int city2 = rand() % num_cities;
        if (city1 == city2) {
            city2 = (city2 + 1) % num_cities; // Ensure city1 != city2
        }
        int weight = rand() % 10 + 1; // Random weight between 1 and 10
        roads[i][0] = city1;
        roads[i][1] = city2;
        roads[i][2] = weight;
    }
}

// Solve the 0/1 Knapsack Problem
void knapsack(int num_roads, int road_weights[], int max_capacity, int selected_roads[]) {
    int dp[max_capacity + 1];
    for (int i = 0; i <= max_capacity; i++) {
        dp[i] = 0;
    }

    for (int i = 0; i < num_roads; i++) {
        for (int j = max_capacity; j >= road_weights[i]; j--) {
            if (dp[j - road_weights[i]] + 1 > dp[j]) {
                dp[j] = dp[j - road_weights[i]] + 1;
                selected_roads[i] = 1;
            }
        }
    }
}

// Create a graph from selected roads
void create_graph(int num_cities, int num_roads, int roads[][3], int selected_roads[], int graph[MAX_CITIES][MAX_CITIES]) {
    for (int i = 0; i < num_cities; i++) {
        for (int j = 0; j < num_cities; j++) {
            if (i == j) {
                graph[i][j] = 0; // Distance from a city to itself is 0
            } else {
                graph[i][j] = INT_MAX; // Initialize other distances to infinity
            }
        }
    }

    for (int i = 0; i < num_roads; i++) {
        if (selected_roads[i]) {
            int city1 = roads[i][0];
            int city2 = roads[i][1];
            int weight = roads[i][2];
            graph[city1][city2] = weight;
            graph[city2][city1] = weight; // Assuming bidirectional roads
        }
    }
}

// Apply Floyd-Warshall to find shortest paths
void floyd_warshall(int num_cities, int graph[MAX_CITIES][MAX_CITIES]) {
    for (int k = 0; k < num_cities; k++) {
        for (int i = 0; i < num_cities; i++) {
            for (int j = 0; j < num_cities; j++) {
                if (graph[i][k] != INT_MAX && graph[k][j] != INT_MAX &&
                    graph[i][k] + graph[k][j] < graph[i][j]) {
                    graph[i][j] = graph[i][k] + graph[k][j];
                }
            }
        }
    }
}

// Function to perform topological sort using DFS
void topologicalSortUtil(int city, int num_cities, int graph[MAX_CITIES][MAX_CITIES], int visited[], int stack[], int* stack_index) {
    visited[city] = 1;
    for (int i = 0; i < num_cities; i++) {
        if (graph[city][i] != INT_MAX && !visited[i]) {
            topologicalSortUtil(i, num_cities, graph, visited, stack, stack_index);
        }
    }
    stack[(*stack_index)++] = city;
}

void topologicalSort(int num_cities, int graph[MAX_CITIES][MAX_CITIES], int stack[]) {
    int visited[MAX_CITIES] = {0};
    int stack_index = 0;

    for (int i = 0; i < num_cities; i++) {
        if (!visited[i]) {
            topologicalSortUtil(i, num_cities, graph, visited, stack, &stack_index);
        }
    }
}

// Function to compute shortest paths using topological order
void shortestPathsTopological(int num_cities, int graph[MAX_CITIES][MAX_CITIES], int start_city, int dist[]) {
    int stack[MAX_CITIES];
    topologicalSort(num_cities, graph, stack);

    // Initialize distances
    for (int i = 0; i < num_cities; i++) {
        dist[i] = INT_MAX;
    }
    dist[start_city] = 0;

    // Process cities in topological order
    for (int i = num_cities - 1; i >= 0; i--) {
        int city = stack[i];
        if (dist[city] != INT_MAX) {
            for (int j = 0; j < num_cities; j++) {
                if (graph[city][j] != INT_MAX && dist[city] + graph[city][j] < dist[j]) {
                    dist[j] = dist[city] + graph[city][j];
                }
            }
        }
    }
}

// Report results
void print_results(int num_cities, int num_roads, int roads[][3], int selected_roads[], int graph[MAX_CITIES][MAX_CITIES]) {
    printf("Selected Roads:\n");
    for (int i = 0; i < num_roads; i++) {
        if (selected_roads[i]) {
            printf("City %d -> City %d (Weight: %d)\n", roads[i][0], roads[i][1], roads[i][2]);
        }
    }

    printf("\nShortest Paths Between All Cities (Floyd-Warshall):\n");
    for (int i = 0; i < num_cities; i++) {
        for (int j = 0; j < num_cities; j++) {
            if (graph[i][j] == INT_MAX) {
                printf("INF\t");
            } else {
                printf("%d\t", graph[i][j]);
            }
        }
        printf("\n");
    }

    printf("\nShortest Paths from City 0 (Topological Sort):\n");
    int dist[MAX_CITIES];
    shortestPathsTopological(num_cities, graph, 0, dist);
    for (int i = 0; i < num_cities; i++) {
        if (dist[i] == INT_MAX) {
            printf("City 0 -> City %d: INF\n", i);
        } else {
            printf("City 0 -> City %d: %d\n", i, dist[i]);
        }
    }
}

int main() {
    int num_cities, num_roads;
    int max_capacity = 50; // Maximum capacity for knapsack

    // User input for number of cities and roads
    printf("Enter the number of cities: ");
    scanf("%d", &num_cities);
    printf("Enter the number of roads: ");
    scanf("%d", &num_roads);

    if (num_cities <= 0 || num_roads <= 0 || num_cities > MAX_CITIES || num_roads > MAX_ROADS) {
        printf("Invalid input size. Please try again.\n");
        return 1;
    }

    int roads[MAX_ROADS][3]; // Array to store roads (city1, city2, weight)
    int road_weights[MAX_ROADS]; // Array to store road weights
    int selected_roads[MAX_ROADS] = {0}; // Array to store selected roads (1 = selected, 0 = not selected)
    int graph[MAX_CITIES][MAX_CITIES]; // Adjacency matrix for the graph

    // Step 1: Generate Road Information
    clock_t start = clock();
    generate_roads(num_cities, num_roads, roads);
    clock_t end = clock();
    double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Time taken to generate roads: %f seconds\n", time_taken);

    // Step 2: Extract road weights
    for (int i = 0; i < num_roads; i++) {
        road_weights[i] = roads[i][2];
    }

    // Step 3: Solve the 0/1 Knapsack Problem
    start = clock();
    knapsack(num_roads, road_weights, max_capacity, selected_roads);
    end = clock();
    time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Time taken to solve the 0/1 Knapsack problem: %f seconds\n", time_taken);

    // Step 4: Create a Road Network
    start = clock();
    create_graph(num_cities, num_roads, roads, selected_roads, graph);
    end = clock();
    time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Time taken to create the road network: %f seconds\n", time_taken);

    // Step 5: Apply Floyd-Warshall Algorithm
    start = clock();
    floyd_warshall(num_cities, graph);
    end = clock();
    time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Time taken to apply Floyd-Warshall algorithm: %f seconds\n", time_taken);

    // Step 6: Apply Topological Sort (Optional)
    start = clock();
    int dist[MAX_CITIES];
    shortestPathsTopological(num_cities, graph, 0, dist);
    end = clock();
    time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Time taken to apply Topological Sort: %f seconds\n", time_taken);

    // Step 7: Report Results
    print_results(num_cities, num_roads, roads, selected_roads, graph);

    return 0;
}
