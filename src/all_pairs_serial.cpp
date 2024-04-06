#include <iomanip>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include "core/cxxopts.h"
#include "core/get_time.h"

#define DEFAULT_PATH "graph.txt"
#define DEFAULT_OUTPUT "output.txt"


int graph_size = 0;

struct Edge {
    int origin = 0;
    int destination = 0;
    int weight = 0;
};

struct Node {
    int node_id = 0;
    int out_degree = 0;
    std::vector<struct Edge*> edges = std::vector<struct Edge*>();
};

struct Graph {
    int max_weight = 0;
    std::vector<struct Node*> nodes = std::vector<struct Node*>();
    std::vector<std::vector<int>> results;
};

struct Graph* getInput(std::string file_path){
    // Open file and validate
    std::ifstream input_file(file_path);
    if(!input_file.good()){
        std::cout << "Couldn't open file " << file_path << "! Aborting.";
        abort();
    }

    struct Graph* graph = new Graph;
    std::string line;
    bool first_line = true;
    getline(input_file, line);

    // Parses lines in the input file one by one
    // Read line, read values on line, construct node, then construct edges.
    do {
        // Handle an empty input file
        if(line.empty() && first_line){
            std::cout << "Input file is empty! Aborting";
            abort();
        }
        first_line = false;

        // Convert line to stringstream to make parsing it easier
        std::stringstream ss(line);
        std::vector<std::string> edge_list;

        while(ss.good()) {
            std::string substr;
            getline(ss, substr, ',');
            edge_list.push_back(substr);
        }

        // All values on the line are now in a vector, 
        // node can now be constructed
        struct Node* node = new Node;
        node->node_id = graph->nodes.size();

        for(int i = 0; i < edge_list.size(); i = i + 2){
            struct Edge* edge = new Edge;
            edge->origin = node->node_id;
            edge->destination = stoi(edge_list[i]);
            edge->weight = stoi(edge_list[i + 1]);

            graph->max_weight += edge->weight;

            node->edges.push_back(edge);
            node->out_degree++;
        }

        graph->nodes.push_back(node);

        // Now get a new line
        // This has to come at the end to allow for do{}while() structure
        // Inevitably need some weird workaround to make first or last line work right,
        // this is the cleanest compromise I could find.
        getline(input_file, line);
    } while (!line.empty());

    graph_size = graph->nodes.size();

    input_file.close();

    return graph;
}

void outputResults(std::vector<std::vector<int>> results, std::string output_name, int max_weight){
    std::ofstream output_file(output_name);
    if(!output_file.good()){
        std::cout << "Couldn't open or create file " << output_name << ". Aborting.";
        abort();
    }

    // Output the header; top row of the results table
    output_file << "\t\t";
    for(int i = 0; i < graph_size; i++){
        output_file << i << "\t";
    }
    output_file << "\n\n";

    // Output the result rows
    // i: Current row
    // j: Current column
    bool na_found = false;

    for(int i = 0; i < graph_size; i++){
        output_file << i << ".\t\t";
        for(int j = 0; j < graph_size; j++){
            if(results[i][j] == max_weight + 1){
                na_found = true;
                output_file << "N/A\t";
            } else {
                output_file << results[i][j] << "\t";
            }
        }
        output_file << "\n";
    }

    if(na_found){
        output_file << "\nAny pairs showing \"N/A\" found no possible route between those two nodes.";
    }

    output_file.close();
}

void parseGraph(struct Graph* graph, std::string output_name){
    graph_size = graph->nodes.size();

    // Create an NxN array, where N is the number of nodes.
    // Used to store the distances. 
    //
    // This is a single line that's pretty hard to read.
    // It creates N vectors of N integers, each initialized larger than the heaviest possible route in the graph,
    // where N is the graph size. By then putting those vectors into a single vector,
    // we create an NxN array, used to store values.
    std::vector<std::vector<int>> distance(graph_size, std::vector<int>(graph_size, graph->max_weight + 1));

    // Now we need to iterate through each node on the graph to set up its starting weights.
    // This doesn't meaningfully change the runtime of the program.
    // O(V^3) is basically equivalent to O(V^3 + V + E) for sufficiently large values of V.
    for(int node_index = 0; node_index < graph_size; node_index++){
        int node_out_degree = graph->nodes[node_index]->out_degree;
        for(int edge_index = 0; edge_index < node_out_degree; edge_index++){
            int dest = graph->nodes[node_index]->edges[edge_index]->destination;
            int weight = graph->nodes[node_index]->edges[edge_index]->weight;

            distance[dest][node_index] = weight;
        }
    }

    // Loop control values for the Floyd-Warshall algorithm
    int i = 0; // Destination node ID
    int j = 0; // Source node ID
    int k = 0; // Iterations

    // We do N iterations through the graph.
    //
    // Before the start of a new iteration:
    //      We know the shortest distance between all pairs of nodes,
    //      only considering the nodes in the set {0, 1, ..., k - 1}
    //      as intermediate nodes between them.
    //
    // After an iteration:
    //      We add node k to the list of nodes we consider as an intermediate
    //      Shortest distances are updated if this node is relevant

    // Iterations
    for(k = 0; k < graph_size; k++){
        // Source nodes
        for(j = 0; j < graph_size; j++){
            // Destination nodes
            for(i = 0; i < graph_size; i++){
                // If k is on the shortest path between i and j, update distance [i][j].
                if (distance[i][k] + distance[k][j] < distance[i][j])
                    distance[i][j] = distance[i][k] + distance[k][j];
            }
        }
    }

    // Output the results
    outputResults(distance, output_name, graph->max_weight);
}

int main(int argc, char *argv[]) {
    std::cout << "All pairs shortest path: Serial version\n";

    // Initialize command line arguments
    cxxopts::Options options("Input_generator",
                             "Generates an input file to be used in the three all_pairs programs");
    options.add_options(
        "custom",
        {
            {"inputPath", "Path to input file",
                cxxopts::value<std::string>()->default_value(DEFAULT_PATH)},
            {"outputName", "Desired output file name",                 
                cxxopts::value<std::string>()->default_value(DEFAULT_OUTPUT)}
        });

    auto cl_options = options.parse(argc, argv);
    std::string input_path = cl_options["inputPath"].as<std::string>();
    std::string output_name = cl_options["outputName"].as<std::string>();

    timer timer;
    timer.start();

    // Get input file and convert it into a graph
    struct Graph* graph = getInput(input_path);

    // Process the graph.
    // To avoid issues with the results going out-of-scope,
    // parseGraph() also calls the output function.
    parseGraph(graph, output_name);

    std::cout << "Processing complete! Result has bee placed in " << output_name << "\n";
    std::cout << "Time taken: " << timer.stop();

    return 0;
}