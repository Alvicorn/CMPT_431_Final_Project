#include <iostream>
#include <stdlib.h>


/**
 * @brief Class representing a directed, weighted edge 
 * 
 */
class Edge {

private:
    int input_vertex;
    int output_vertex;
    int weight;

public:
    Edge(int input_vertex, int output_vertex, int weight) {
        this->input_vertex = input_vertex;
        this->output_vertex = output_vertex;
        this->weight = weight;
    }
    int get_input_vertex() { return input_vertex; }
    int get_output_vertex() { return output_vertex; }
    int get_weight() { return weight; }
    bool is_self_loop() { return input_vertex == output_vertex; }

    // Overload << operator to print an Edge object
    friend std::ostream& operator<<(std::ostream& os, const Edge& edge) {
        os << "Input vertex: " << edge.input_vertex << ", ";
        os << "Output vertex: " << edge.output_vertex << ", ";
        os << "Weight: " << edge.weight;
        return os;
    }

    // Overload == operator to compare two Edge objects
    friend bool operator==(const Edge& edge1, const Edge& edge2) {
        return edge1.input_vertex == edge2.input_vertex &&
               edge1.output_vertex == edge2.output_vertex &&
               edge1.weight == edge2.weight;
    }
};
