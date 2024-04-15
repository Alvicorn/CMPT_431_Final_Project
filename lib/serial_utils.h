#include "../core/core_utils.h"
#include "Edge.h"
#include "Matrix_2D.h"
#include <map>
#include <vector>
#include <sstream>

/**
 * @brief Given a csv line, seperate them such that the ith character is
 *        the output vertex and the i+1th character is the wieght of the edge. 
 * 
 * @param edges A map of input vertices with a key of edges.
 * @param input_vertex Input vertex for the directed edge.
 * @param csv_line String of output vertices and weights.
 */
void generate_edges(
        std::map<int, std::vector<Edge>>& edges, 
        int input_vertex, 
        std::string csv_line
    ) {
    
    std::stringstream ss(csv_line);
    std::vector<int> values;

    int value;
    // Read values from stringstream separated by commas
    while (ss >> value) {
        values.push_back(value);
        if (ss.peek() == ',') {ss.ignore(); } // ignore the commas
    }
    
    int mod = 0;
    int weight;
    int output_vertex;
    while (!values.empty()) { // extract the edge weights and the output vertices
        if (mod == 0) { // value is an output vertex
            output_vertex = values.front();
        } else { // value is a weight
            weight = values.front();

            // create an edge
            edges[input_vertex].push_back(Edge(input_vertex, output_vertex, weight));
        }
        values.erase(values.begin());
        mod = (mod + 1) % 2;
    }
}


bool all_pairs_serial(std::vector<std::string> csv_lines) {
    
    std::map<int, std::vector<Edge>> edges;
    for (int i = 0; i < csv_lines.size(); i++) {
        edges[i];
        generate_edges(edges, i, csv_lines[i]);
    }
    
    // initalize the distances
    Matrix_2D* distances = new Matrix_2D(csv_lines.size(), INFINITY_INT);
    
    if (distances == NULL) {
        std::cout << "Memory error!\n";
        return false;
    }

    timer serial_timer;
    serial_timer.start();

    for (int vertex = 0; vertex < csv_lines.size(); vertex++) {
        distances->set_matrix_value(vertex,vertex, 0); // self loops have a weight of 0
        for (Edge edge: edges[vertex]) {
            bool result;
            if (edge.is_self_loop()) { // self loops have a weight of 0
                result = distances->set_matrix_value(
                    edge.get_input_vertex(), edge.get_input_vertex(), 0
                );
            } else {
                 result = distances->set_matrix_value(
                    edge.get_input_vertex(), edge.get_output_vertex(), edge.get_weight()
                 );
            }
            if (!result) { return false; } // update failure
        }
    }

    // compute the shortest paths
    for (int k = 0; k < csv_lines.size(); k++) {
        for (int j = 0; j < csv_lines.size(); j++) {
            for (int i = 0; i < csv_lines.size(); i++) {
                try {
                    int current_path_weight = distances->get_matrix_value(i, j);
                    int new_path_weight = distances->get_matrix_value(i, k) 
                                        + distances->get_matrix_value(k, j);

                    if (current_path_weight > new_path_weight) {
                        distances->set_matrix_value(i, j, new_path_weight);
                    }
                } catch (const IndexOutOfBoundsException& e) {
                    return false;
                }
            }
        }
    }

    double time_taken = serial_timer.stop();
    std::cout << "Final matrix\n";
    distances->print_matrix();    
    std::cout << "Time taken: " << time_taken << std::endl; 
    delete distances;
    return true;
}