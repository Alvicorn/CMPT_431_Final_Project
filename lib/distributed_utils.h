#include "../core/core_utils.h"
#include "Edge.h"
#include "Matrix_2D.h"
#include <map>
#include <vector>
#include <sstream>
#include <mpi.h>

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

bool graph_init(Matrix_2D* distances, std::vector<std::string> csv_lines){
    std::map<int, std::vector<Edge>> edges;
    for (int i = 0; i < csv_lines.size(); i++) {
        edges[i];
        generate_edges(edges, i, csv_lines[i]);
    }

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
            if (!result) { 
                // If we failed to create the graph for some reason.
                // If this happens, we cannot continue, so shut down other processes.
                return false; 
            }
        }
    }
    return true;
}

void transmit_pack(Matrix_2D* distances, int send_buffer[]){
    int cur_cell = 0;

    for(int i = 0; i < distances->get_dimension(); i++){
        for(int j = 0; j < distances->get_dimension(); j++){
            send_buffer[cur_cell] = distances->get_matrix_value(i, j);
            cur_cell++;
        }
    }
}

void transmit_unpack(Matrix_2D* distances, int receive_buffer[]){
    int cur_cell = 0;

    for(int i = 0; i < distances->get_dimension(); i++){
        for(int j = 0; j < distances->get_dimension(); j++){
            distances->set_matrix_value(i, j, receive_buffer[cur_cell]); 
            cur_cell++;
        }
    }
}

bool all_pairs_distributed(std::vector<std::string> csv_lines) {
    MPI_Init(nullptr, nullptr);

    MPI_Status mpi_status;
    int rank;
    int world_size;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    timer distributed_timer;
    distributed_timer.start();
    
    Matrix_2D* distances = new Matrix_2D(csv_lines.size(), INFINITY_INT);

    int send_buffer[csv_lines.size() * csv_lines.size()];
    int receive_buffer[csv_lines.size() * csv_lines.size()];

    if(rank == 0){
        // initalize the distances. Only the "root" process has to do this,
        // since the root is about to broadcast its results to the other processes anyway.
        // If the initialization fails, then we can't proceed anyway, so just abort in that case.
        if(!graph_init(distances, csv_lines)){
            return false;
            MPI_Abort(MPI_COMM_WORLD, -1);
        }

        // Now we're going to need to broadcast this to the other processes.
        // Packing the results into a single 1D array makes transmission much easier.
        transmit_pack(distances, send_buffer);
    }

    MPI_Bcast(send_buffer, distances->get_dimension() * distances->get_dimension(), MPI_INT, 0, MPI_COMM_WORLD); 

    // Decompose the work across the processes
    // This form of decomposition means the last process in the list will do a disproportionate amount of work
    // in the case where there's a large number of processes compared to the size of the graph.
    // In that case, communication overhead will cost more than just waiting for that last process to complete,
    // so this isn't really a problem to begin with.
    int step_size = distances->get_dimension() / world_size;
    int start_k = step_size * rank;
    int stop_k = start_k + step_size;

    // Adjust k value to account for starting at 0, not 1
    stop_k--;
    // Final process gets end k value overwritten to avoid segfault
    if(rank == world_size - 1){ stop_k = distances->get_dimension(); }
    // All but first process gets start k overwritten to cross process boundaries reliably
    if(rank != 0){ start_k--; }

    int iterations;
    if(world_size > 2){
        iterations = 3;
    } else {
        iterations = world_size;
    }

    // Compute the shortest paths
    // This process must run at least twice:
    //      1. First iteration computes the values for the "local" values of k
    //      2. Then we broadcast our results to the other processes with MPI_Allreduce
    //      3. MPI_Allreduce gives us the best results for each node across every process
    //      4. Then run it again, serving to both check our work, 
    //         and handle process boundaries once the results get broadcast to the others
    //
    //      With 2 processes, running twice is sufficient, but otherwise we must run it 3 times.
    //
    // This seems inefficient on first glance, but this is still faster than the serial version
    // if we use enough processes.
    // Even with only 2 or 3 processes, this is still equivalent to the serial version,
    // because the step size will be reduced, and 3(N*N*(N/3)) = N^3 for all N.
    // With 4 or more processes, this is faster than the serial implementation. 
    for(int i = 0; i < iterations; i++){
        for (int k = start_k; k < stop_k; k++) {
            for (int j = 0; j < csv_lines.size(); j++) {
                for (int i = 0; i < csv_lines.size(); i++) {
                    if(i != j){
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
        }

        // By using MPI_Allreduce and MPI_MIN, we automatically get the minimum value for each value in the matrix,
        // and the result gets broadcast back to each process.
        MPI_Barrier(MPI_COMM_WORLD);

        transmit_pack(distances, send_buffer);
        MPI_Allreduce(send_buffer, receive_buffer, distances->get_dimension() * distances->get_dimension(), MPI_INT, MPI_MIN, MPI_COMM_WORLD);
        transmit_unpack(distances, receive_buffer);
    }

    // Output the results
    double time_taken = distributed_timer.stop();

    if(rank == 0){
        std::cout << "Final matrix\n";
        distances->print_matrix();    
        std::cout << "Time taken: " << time_taken << std::endl; 
    }

    delete distances;

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();
    return true;
}