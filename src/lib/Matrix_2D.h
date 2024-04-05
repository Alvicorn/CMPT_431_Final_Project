#include "../core/exceptions.h"
#include <iostream>
#include <stdlib.h>


/**
 * @brief Two dimensional int array 
 * 
 */
class Matrix_2D {

private:
    int dimension;
    int** matrix;

public:
    Matrix_2D(int dimension, int initial_value) {
        this->dimension = dimension;
        this->matrix = new int*[dimension];
        for (int i = 0; i < dimension; ++i) {
            matrix[i] = new int[dimension];
        }

        // Initialize the elements of the matrix to infinity
        for (int i = 0; i < dimension; i++) {
            for (int j = 0; j < dimension; j++) {
                matrix[i][j] = initial_value;
            }
        }
    }

    ~Matrix_2D() {
        for (int i = 0; i < this->dimension; i++) {
            delete[] matrix[i];
        }
        delete[] matrix;
    }

    /**
     * @brief Getter method for dimension
     * 
     * @return Dimension of the 2D matrix
     */
    int get_dimension() { return dimension; }
    
    /**
     * @brief Get the matrix value at the given index.
     * 
     * @param row Index of the matrix row.
     * @param column Index of the matrix column.
     * @return int value of the value at the [row][column].
     */
    int get_matrix_value(int row, int column) {
        if (row < 0 || row > this->dimension) {
            throw IndexOutOfBoundsException("Invalid row index: " + std::to_string(row));
        } else if (column < 0 || column > this->dimension) {
            throw IndexOutOfBoundsException("Invalid column index: " + std::to_string(column));
        } else {
            return this->matrix[row][column];
        }
    }

    /**
     * @brief Update a matrix value at a given index. 
     * 
     * @param row Index of the matrix row.
     * @param column Index of the matrix column.
     * @param new_value New value to be placed into the matrix.
     * @return true if the matrix was updated successfully, false otherwise.
     */
    bool set_matrix_value(int row, int column, int new_value) {
        if (row < 0 || row > this->dimension) {
            std::cerr << "Invalid row index: " << row << std::endl;
            return false;
        } else if (column < 0 || column > this->dimension) {
            std::cerr << "Invalid column index: " << column << std::endl;
            return false;
        } else {
            this->matrix[row][column] = new_value;
            return true;
        }
    }
    
    /**
     * @brief Print the matrix to the console for debugging 
     * 
     * @param matrix 2D int array
     * @param n Dimension of the 2D array
     */
    void print_matrix() {
        std::cout << "  |\t";
        for (int i = 0; i < this->dimension; i++) {
            std::cout << i << "\t";
        }
        std::cout << "\n\n";

        for (int i = 0; i < this->dimension; i++) {
            std::cout << i << " |\t";
            for (int j = 0; j < this->dimension; j++) {
                if(this->matrix[i][j]  >= INFINITY_INT) {
                    std::cout << "INF ";
                } else {
                    std::cout << this->matrix[i][j] << "\t";
                }
            }
            std::cout << std::endl;
        }
    }
};