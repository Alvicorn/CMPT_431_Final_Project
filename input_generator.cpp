#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <random>
#include "core/cxxopts.h"

#define DEFAULT_NODES "100"
#define DEFAULT_EDGES "5"
#define DEFAULT_RAND "false"
#define DEFAULT_MINWEIGHT "1"
#define DEFAULT_MAXWEIGHT "100"
#define DEFAULT_FILENAME "graph.txt"

// Checks that all values are reasonable - prevents having graphs with a negative number of nodes,
// a minimum weight greater than the maximum weight, or anything similar to that.
void valueCheck(int* n_nodes, int* n_edges, int* min_weight, int* max_weight){
    if(*n_nodes <= 1){
        std::cout << "Number of nodes cannot be 1 or less. Setting to default value.\n";
        *n_nodes = 100;
    }
    
    if(*n_edges < 1){
        std::cout << "Number of edges per node cannot be 0 or less. Setting to the default value.\n";
        *n_edges = 5;
    }

    if(*min_weight < 0){
        std::cout << "Minimum weight cannot be negative. Setting to the default value.\n";
        *min_weight = 1;
    }

    if(*max_weight < 0){
        std::cout << "Maximum weight cannot be negative. Setting to the default value.\n";
        *max_weight = 100;
    }

    if(*max_weight < *min_weight){
        std::cout << "Minimum weight cannot be greater than maximum weight. Setting maximum to minimum + 1.\n";
        *max_weight = *min_weight + 1;
    }
}

void generateGraph(int* n_nodes, int* n_edges, int* min_weight, int* max_weight, bool rand_edges, std::string file_name){
    std::ofstream output_file(file_name);
    if(!output_file.good()){
        std::cout << "Couldn't open or create file " << file_name << ". Aborting.";
        abort();
    }

    // Initialize random. Used for both generating weights and for rand_edges mode.
    srand (time(NULL));

    for(int current_node = 0; current_node < *n_nodes; current_node++){
        int edges_for_node = 0;

        if(rand_edges == true){
            // Generates a random number of edges for this node, anywhere from 0 to 2 * n_edges.
            // By doing a 50/50 roll 2*n_edges times, 
            // we get a normal distribution, centered on n_edges.
            //
            // A linear distribution would work too, and would be a bit faster to generate,
            // but this is still so fast it shouldn't matter for all but excessively large graphs.
            for(int i = 0; i < ((*n_edges) * 2); i++){
                if((rand() % 2) == 1){ edges_for_node++; }
            }
        } else {
            // If not using rand_edges mode, we can bypass the randomization.
            edges_for_node = *n_edges;
        }

        // Output graph layout to file. 
        // Formatted with each node on its own line, and edges and weights alternating,
        // separated by commas. So "2,1,3,4" on line 1 means node 1 has:
        //      * A link to node 2 with weight 1,
        //      * A link to node 3 with weight 4
        for(int current_edge = 0; current_edge < edges_for_node; current_edge++){
            int edge_destination;
            int edge_weight;

            // Pick a target node. Cannot make an edge point back to the origin.
            // If it tries to, reroll a new target node until we get a valid one.
            do {
                edge_destination = rand() % (*n_nodes);
            } while (edge_destination == current_node);

            // Generate an edge weight between min and max allowable weights
            int weight_range = *max_weight - *min_weight + 1;
            edge_weight = (rand() % weight_range) + *min_weight;

            output_file << edge_destination << "," << edge_weight;
            
            if(current_edge == edges_for_node - 1) {
                output_file << std::endl;
            } else {
                output_file << ",";
            }
        }
    }

    output_file.close();
}

int main(int argc, char *argv[]) {
    // Initialize command line arguments
    cxxopts::Options options("Input_generator",
                             "Generates an input file to be used in the three all_pairs programs");
    options.add_options(
        "custom",
        {
            {"nNodes", "Number nodes in the graph",
                cxxopts::value<int>()->default_value(DEFAULT_NODES)},
            {"nEdges", "Number of outbound edges per node",                 
                cxxopts::value<int>()->default_value(DEFAULT_EDGES)},
            {"randEdges", "Enable edge randomization mode.\nRandomizes the number of edges each node has. Follows normal distribution with an average of nEdges.\nTakes no arguments, just using the --randEdges flag enables this mode.",                 
                cxxopts::value<bool>()->default_value(DEFAULT_RAND)},
            {"minWeight", "Minimum weight any edge can have (cannot be negative).",
                cxxopts::value<int>()->default_value(DEFAULT_MINWEIGHT)},
            {"maxWeight", "Maximum weight any edge can have (cannot be negative or less than minWeight)",
                cxxopts::value<int>()->default_value(DEFAULT_MAXWEIGHT)},
            {"fileName", "Name of output file",
                cxxopts::value<std::string>()->default_value(DEFAULT_FILENAME)}
        });

    int* n_nodes = new int;
    int* n_edges = new int;
    int* min_weight = new int;
    int* max_weight = new int;

    auto cl_options = options.parse(argc, argv);
    *n_nodes = cl_options["nNodes"].as<int>();
    *n_edges = cl_options["nEdges"].as<int>();
    *min_weight = cl_options["minWeight"].as<int>();
    *max_weight = cl_options["maxWeight"].as<int>();
    bool rand_edges = cl_options["randEdges"].as<bool>();
    std::string file_name = cl_options["fileName"].as<std::string>();

    // Check if user values are reasonable
    valueCheck(n_nodes, n_edges, min_weight, max_weight);

    // Once the values are valid, create the graph
    generateGraph(n_nodes, n_edges, min_weight, max_weight, rand_edges, file_name);

    // Cleanup
    delete n_nodes;
    delete n_edges;
    delete min_weight;
    delete max_weight;
}
