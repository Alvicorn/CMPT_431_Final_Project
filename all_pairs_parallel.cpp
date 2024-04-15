#include "lib/parallel_utils.h"


/**
 * @brief Parallel implementation of the All-Pairs shortest path 
 *        algorithm. The executable takes two parameter (--inputFile, --nThreads)
 *        and returns a matrix of the shortest path's wieghts to get from
 *        one vertex to the next. 
 * 
 */
int main(int argc, char *argv[]) {
    std::cout << std::scientific << std::setprecision(TIME_PRECISION); 
    cxxopts::Options options(
        "all pairs serial version",
        "Calculate all-pairs shortest path using serial execution"
    );
    options.add_options(
        "",
        {
            {
                "nThreads", "Number of Threads",
                cxxopts::value<uint>()->default_value(DEFAULT_NUMBER_OF_THREADS)
            },
            {
                "inputFile", "Input graph file path",
                cxxopts::value<std::string>()->default_value("./inputs/graph.txt")
            }
        }
    );

    auto cl_options = options.parse(argc, argv);
    std::string input_file_path = cl_options["inputFile"].as<std::string>();
    uint n_threads = cl_options["nThreads"].as<uint>();
    
    // parse the input file
    std::ifstream inputFile(input_file_path); 
    if (!inputFile.is_open()) { // Check if the file opened successfully
        std::cout << "Unable to open file (" << input_file_path << ")" << std::endl;
        return 1;
    }

    std::vector<std::string> lines;
    std::string line;
    while (std::getline(inputFile, line)) {
        if (!line.empty()) { lines.push_back(line); }
    }

    if (lines.size() == 0) {
        std::cout << "Empty Input Graph!" << std::endl;

    }else if (!all_pairs_parallel(lines, n_threads)) {
        std::cout << "Something went wrong!" << std::endl;
    }

    inputFile.close(); // Close the file
    return 0;
}