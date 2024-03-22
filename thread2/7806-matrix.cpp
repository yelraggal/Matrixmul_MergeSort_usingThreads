#include <iostream>
#include <vector>
#include <pthread.h>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <fstream>

using namespace std;// cout std :: cout
using namespace chrono;

struct ThreadArgs {
    const vector<vector<int>>& matrix_a;//array list
    const vector<vector<int>>& matrix_b;
    vector<vector<int>>& result_matrix;
    int row;
    int col;
};


void* compute_element(void* args) { // dy el function el hb3tha lel thread
    ThreadArgs* thread_args = static_cast<ThreadArgs*>(args);
    
    int value = 0;
    // hlf 3la matrixb.size() 3dd el rows el f B , aw cols of A 
    // bdrb element wise
    for (size_t k = 0; k < thread_args->matrix_b.size(); ++k) {
        value += thread_args->matrix_a[thread_args->row][k] * thread_args->matrix_b[k][thread_args->col];
    }

    thread_args->result_matrix[thread_args->row][thread_args->col] = value;

    pthread_exit(nullptr);
}

void matrix_multiply_parallel_element(const vector<vector<int>>& matrix_a,
                               const vector<vector<int>>& matrix_b,
                               vector<vector<int>>& result_matrix) {
    // by3rf threads b 3dd el elements l fel op
    // row of A * col of B
    // ana 3ndi el matrix vector of vectors
    // f matrix [0] da btgebli awl cell f row vector ,, fb3den . size() 
    //ht2oli ana m3rf kam coulmn gwaha
    
    pthread_t threads[matrix_a.size() * matrix_b[0].size()];
    size_t thread_index = 0;

    // hlf 3la el result matrix amlaha
    for (size_t i = 0; i < matrix_a.size(); ++i) {
        for (size_t j = 0; j < matrix_b[0].size(); ++j) {

        // thread return type void* , 
        // ana bgm3 kol el args el 3yz ab3tha f struct w ab3t el struct da
            ThreadArgs* args = new ThreadArgs{matrix_a, matrix_b, result_matrix, static_cast<int>(i), static_cast<int>(j)};
            
            //pthread_create bta5od pointer lthread , null, function , args  i want to send (in form of struct)
            pthread_create(&threads[thread_index++], nullptr, compute_element, static_cast<void*>(args));
        }
    }
    // make the main thread waits all the intiallized threads
    for (size_t i = 0; i < thread_index; ++i) {
        pthread_join(threads[i], nullptr);
    }
}





void* compute_row(void* args) {
    ThreadArgs* thread_args = static_cast<ThreadArgs*>(args);

    // hlf 3la el result matrix amlaha 
    // my goal now is to calculate the row x , (x is sent in the args)
    //awl hga hlf 3la cols b 3shan da hwa zy 3dd cols el o/p
    for (size_t j = 0; j < thread_args->matrix_b[0].size(); ++j) {
        int value = 0;
        // dewl2ti msln in 1st itertion 3yz yhseb awl element
        //awl element da 3bra 3n sf f 3mod
        // size bta3 sf x 3mod hwa size (col in A) or (Rows in B)
        for (size_t k = 0; k < thread_args->matrix_b.size(); ++k) {
            value += thread_args->matrix_a[thread_args->row][k] * thread_args->matrix_b[k][j];
        }
        thread_args->result_matrix[thread_args->row][j] = value;
    }

    pthread_exit(nullptr);
}

void matrix_multiply_parallel_row(const vector<vector<int>>& matrix_a,
                               const vector<vector<int>>& matrix_b,
                               vector<vector<int>>& result_matrix) {
    // # of threads will be number of rows in the result matrix (same as number of rows in A)
    pthread_t threads[matrix_a.size()];

    // hlf 3la el row row fel result matrix amlah
    for (size_t i = 0; i < matrix_a.size(); ++i) {
    //msh mhtag fel args el col , ana hb3t rkm el row bs (ana 3yz ahseb row rkm kam fe result array)
        ThreadArgs* args = new ThreadArgs{matrix_a, matrix_b, result_matrix, static_cast<int>(i)};
       
        pthread_create(&threads[i], nullptr, compute_row, static_cast<void*>(args));
    }
    
    // make the main thread wait for child threads
    for (size_t i = 0; i < matrix_a.size(); ++i) {
        pthread_join(threads[i], nullptr);
    }
}












int main() {
    // Input filename
    string filename;
    cout << "Enter the filename: ";
    cin >> filename;

    // Open the file (Using i/p file stream)
    ifstream inputFile(filename);

    if (!inputFile.is_open()) {
        cerr << "File Not Exist" << endl;
        return 1;
    }
    
    size_t rows_a, cols_a, rows_b, cols_b;

    // read size of first matrix
    inputFile >> rows_a >> cols_a;
    //read values of first matrix
    // intiallize matrix b size that we read

    vector<vector<int>> matrix_a = vector<vector<int>>(rows_a, vector<int>(cols_a, 0))
vector<vector<int>>(rows, vector<int>(cols, 0))
    for (size_t i = 0; i < rows_a; ++i) {
        for (size_t j = 0; j < cols_a; ++j) {
            inputFile >> matrix_a[i][j];
        }
    }

    // read size of first matrix
    inputFile >> rows_b >> cols_b;
    //read values of first matrix
    // intiallize matrix b size that we read

        vector<vector<int>> matrix_b = vector<vector<int>>(rows_b, vector<int>(cols_b, 0))

    for (size_t i = 0; i < rows_b; ++i) {
        for (size_t j = 0; j < cols_b; ++j) {
            inputFile >> matrix_b[i][j];
        }
    }

    inputFile.close();

    // Check if matrices can be multiplied
    if (cols_a != rows_b) {
        cerr << "Error: The number of columns in matrix A must be equal to the number of rows in matrix B for multiplication.\n";
        return 1;
    }

    // Measure the time taken for matrix multiplication (element by element)
    auto start_time = steady_clock::now();
    //result size hyeb2a 3dd el rows of first matrix w columns bt3 2nd matrix
    vector<vector<int>> result_matrix(rows_a, vector<int>(cols_b, 0));
    matrix_multiply_parallel_element(matrix_a, matrix_b, result_matrix);
    
    auto end_time = steady_clock::now();
    chrono::duration<double> duration = chrono::duration_cast<chrono::duration<double>>(end_time - start_time);
    // use duration.count() lw 3yz atb3 el time in seconds

    // Print the result
    cout << "\nResult matrix (element by element):\n";
    for (size_t i=0;i < result_matrix.size();++i) {
        for (size_t j=0; j < result_matrix[i].size();++j) {
            cout << result_matrix[i][j] << " ";
        }
        //add new line
        cout << endl;
    }

    // Print the time taken
    // fixed bn3del byha fel format bt3 el floating point el tal3
    // bn6lyha 6 decimapl points bs
    cout << fixed << setprecision(6) << "\nTime taken for matrix multiplication (element by element): " << duration.count() << " Seconds." << endl;


    // 2nd approch

    // Measure the time taken for matrix multiplication (row by row)
    auto start_time_2 = steady_clock::now();
    
    vector<vector<int>> result_matrix_2(rows_a, vector<int>(cols_b, 0));
    matrix_multiply_parallel_row(matrix_a, matrix_b, result_matrix_2);
    auto end_time_2 = steady_clock::now();
    chrono::duration<double> duration_2 = chrono::duration_cast<chrono::duration<double>>(end_time_2 - start_time_2);

   
   // print output
    cout << "\nResult matrix (row by row):\n";
    for (size_t i=0;i < result_matrix_2.size();++i) {
        for (size_t j=0; j < result_matrix_2[i].size();++j) {
            cout << result_matrix_2[i][j] << " ";
        }
        //add new line
        cout << endl;
    }

    // Print the time taken by multiplication (row by row)
    cout << fixed << setprecision(6) << "\nTime taken for matrix multiplication (row by row): " << duration_2.count() << " Seconds." << endl;

    return 0;
}
