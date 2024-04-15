#include "../lib/serial_utils.h"


bool test_generate_edges() {
    std::map<int, std::vector<Edge>> edges;
    int input_vertex = 0;
    std::string csv_line = "0,5,3,12,1,6";

    generate_edges(edges, input_vertex, csv_line);

    return edges[0].size() == 3
        && edges[0][0] == Edge(0, 0, 5)
        && edges[0][1] == Edge(0, 3, 12)
        && edges[0][2] == Edge(0, 1, 6);
}


void print_result(std::string test_name, bool test_status) {
    std::string success = test_status ? "PASS" : "FAIL";
    std::cout << "Test " << test_name << "..." << success << std::endl;
}


int main(int argc, char* argv[]) {
    print_result("generate_edges", test_generate_edges);

    return 0;   
}