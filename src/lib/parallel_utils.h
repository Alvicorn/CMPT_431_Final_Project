#ifndef PARALLEL_UTILS_H
#define PARALLEL_UTILS_H

#include "../core/core_utils.h"
#include "Edge.h"
#include "Matrix_2D.h"
#include <atomic>
#include <condition_variable>
#include <limits.h>
#include <map>
#include <mutex>
#include <sstream>
#include <thread>
#include <vector>

#define DEFAULT_NUMBER_OF_THREADS "1"
#define THREAD_LOGS 0
// #define ADDITIONAL_TIMER_LOGS 0


// taken from CMPT 431 Spring 2024 Assignment 3
struct CustomBarrier {
  int num_of_threads_;
  int current_waiting_;
  int barrier_call_;
  std::mutex my_mutex_;
  std::condition_variable my_cv_;

  CustomBarrier(int t_num_of_threads)
      : num_of_threads_(t_num_of_threads), current_waiting_(0),
        barrier_call_(0) {}

  void wait() {
    std::unique_lock<std::mutex> u_lock(my_mutex_);
    int c = barrier_call_;
    current_waiting_++;
    if (current_waiting_ == num_of_threads_) {
      current_waiting_ = 0;
      // unlock and send signal to wake up
      barrier_call_++;
      u_lock.unlock();
      my_cv_.notify_all();
      return;
    }
    my_cv_.wait(u_lock, [&] { return (c != barrier_call_); });
    //  Condition has been reached. return
  }
};


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
        std::string csv_line) {
    
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

/**
 * @brief Function that will be used by a worker thread
*/
void parallize(
        Matrix_2D* curr_distances, Matrix_2D* prev_distances,
        CustomBarrier* barrier, double* time_taken, int* start_row, 
        int* end_row, int thread_id, int* n_threads) {
    
    timer local_timer;
    local_timer.start();
    int size = curr_distances->get_dimension();

    // compute the shortest paths
        for (int k = 0; k < size; k++) {
            for (int j = 0; j < size; j++) {
                for (int i = *start_row; i < *end_row; i++) {
                    try {
                        int current_path_weight = curr_distances->get_matrix_value(i, j);
                        int new_path_weight = prev_distances->get_matrix_value(i, k) 
                                              + prev_distances->get_matrix_value(k, j);

                        if (current_path_weight > new_path_weight) {
                            curr_distances->set_matrix_value(i, j, new_path_weight);
                        }
                    } catch (const IndexOutOfBoundsException& e) {
                        std::cout << "index was out of bounds!\n";
                    }
                }
            }
            barrier->wait();       
            if (thread_id == *n_threads - 1) {
                prev_distances->update_matrix(curr_distances);

            }     
            barrier->wait();
        }

    *time_taken = local_timer.stop();
}


bool all_pairs_parallel(std::vector<std::string> csv_lines, int n_threads) {
    
    
    std::map<int, std::vector<Edge>> edges;
    for (int i = 0; i < csv_lines.size(); i++) {
        edges[i];
        generate_edges(edges, i, csv_lines[i]);
    }
    
    // initalize the distances
    Matrix_2D* curr_distances = new Matrix_2D(csv_lines.size(), INFINITY_INT);
    Matrix_2D* prev_distances = new Matrix_2D(csv_lines.size(), INFINITY_INT);

    if (curr_distances == NULL || prev_distances == NULL) {
        std::cout << "Memory error!\n";
        return false;
    }

    // time the all-pairs shortest path computations
    timer parallel_timer;
    parallel_timer.start();


    for (int vertex = 0; vertex < csv_lines.size(); vertex++) {
        // self loops have a weight of 0
        curr_distances->set_matrix_value(vertex,vertex, 0); // self loops have a weight of 0
        prev_distances->set_matrix_value(vertex,vertex, 0); // self loops have a weight of 0
        for (Edge edge: edges[vertex]) {
            bool result;
            if (edge.is_self_loop()) { // self loops have a weight of 0
                result = curr_distances->set_matrix_value(
                    edge.get_input_vertex(), edge.get_input_vertex(), 0
                );
                result &= prev_distances->set_matrix_value(
                    edge.get_input_vertex(), edge.get_input_vertex(), 0
                );
            } else {
                result = curr_distances->set_matrix_value(
                    edge.get_input_vertex(), edge.get_output_vertex(), edge.get_weight()
                );
                result &= prev_distances->set_matrix_value(
                    edge.get_input_vertex(), edge.get_output_vertex(), edge.get_weight()
                );
            }
            if (!result) { return false; } // update failure
        }
    }

    // create threads and ditribute the work across n_threads
    int rows_per_thread = csv_lines.size() / n_threads;
    int remainder = csv_lines.size() % n_threads;

    std::thread threads[n_threads];
    int start_rows[n_threads];
    int end_rows[n_threads];
    for (int i = 0; i < n_threads; i++) {
        start_rows[i] = i * rows_per_thread;
        end_rows[i] = (i + 1) * rows_per_thread;
        if (i == n_threads - 1) {
            end_rows[i] += remainder;
        }
    }    

    CustomBarrier* barrier = new CustomBarrier(n_threads);
    double thread_time_taken[n_threads];

    for (int i = 0; i < n_threads; i++) {
        threads[i] = std::thread(
            parallize,
            curr_distances, 
            prev_distances, 
            barrier,
            &(thread_time_taken[i]),
            &(start_rows[i]), 
            &(end_rows[i]), 
            i, 
            &(n_threads)
        );
    }

    for (int i = 0; i < n_threads; i++) { threads[i].join(); }

    double time_taken = parallel_timer.stop();
    std::cout << "Final matrix\n";
    curr_distances->print_matrix();
    std::cout << "thread_id,\tstart_row,\tend_row,\ttime_taken\n";
    std::string delim = ",\t\t";
    for (int i = 0; i < n_threads; i++) {
        std::cout << i << delim << start_rows[i] << delim << end_rows[i] << delim << thread_time_taken[i] << std::endl;
    }    
    std::cout << "Total time taken: " << time_taken << std::endl; 
    delete curr_distances;
    delete prev_distances;
    delete barrier;
    return true;
}

#endif