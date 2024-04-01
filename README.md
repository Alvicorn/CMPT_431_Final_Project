# CMPT_431_Project
All-Pairs Shortest Path


## Notes and comments to be deleted later

* `all_pairs_serial.cpp`: serial version
* `all_pairs_parallel.cpp`: parallel version with multithreading
* `all_pairs_distributed.cpp`: distributed version with MPI

* `input_generator.cpp`: generates input files to be used with the above programs

## Input Generator usage:
`input_generator.cpp` is used to generate a graph in the format used by the other programs.
It takes the following arguments:

    * `--nNodes`: Takes an integer. Sets the number of nodes to be used in the graph. Defaults to 100. Cannot be 1 or less.
    * `--nEdges`: Takes an integer. Sets the number of outbound edges each node should have. Defaults to 5. Cannot be 0 or less.
    * `--randEdges`: Takes no parameter, just using this flag enables this mode. Randomizes the number of edges out of each node. When enabled, the number of edges on a node ranges from 0 to 2*nEdges, with a normal distribution centered on nEdges.
    * `--minWeight`: Takes an integer. Sets the minimum weight an edge could have. Defaults to 0. Cannot be negative, or greater than maxWeight.
    * `--maxWeight`: Takes an integer. Sets the maximum weight an edge could have. Cannot be negative, or less than minWeight. Defaults to 10. Edge weights are determined with a linear distribution.
    * `--fileName`: Takes a string. Sets a custom file name for the output graph to use. Defaults to "graph.txt". **WARNING: If the file already exists, this may overwrite its contents!**

Example usage:

`--nNodes 10 --nEdges 5 --minWeight 0 --maxWeight 20 --randEdges`

This will generate a graph with 10 nodes, each of which has a random number of edges (with an average value of 5). Each edge will have a weight between 0 and 20. Because no file name was given, the output will be in a file named `graph.txt`.