#include <bits/stdc++.h>
using namespace std;

__global__ void matrix_multiplication(int *A, int *B, int *C, int N,int M,int P,int number_of_matrix,int number_of_core){



    int i = threadIdx.x;//rank
    int start_matrix_index = (number_of_matrix/ number_of_core) * i;
    int end_matrix_index = (number_of_matrix / number_of_core) * (i + 1);
    if(i == number_of_core - 1) end_matrix_index = number_of_matrix;

    for(int x = start_matrix_index; x < end_matrix_index; x++){

        for(int i = 0; i < N; i++){
            for(int j = 0; j < P; j++){
                int sum = 0;
                for(int k = 0; k < M; k++){
                    sum += A[ i * M + k] * B[k * P + j];
                }
                C[i * P + j] = sum;
            }
        }
    }
}
int main(int argc , char *argv[]){
    freopen("mul.txt","r",stdin);
    cout << "Enter the dimenssion of the matrix, N, M , P: " << endl;
    int N , M , P;
    cin >> N >> M >> P;
    cout << "Enter the number of Matrix: " << endl;
    int number_of_matrix;
    cin >> number_of_matrix;

    // totall size calculation
    int a_element = number_of_matrix * N * M;
    int b_element = number_of_matrix * M * P;
    int c_element = number_of_matrix * N * P;

    int *A=new int[a_element];
    int *B=new int[b_element];
    int *C=new int [c_element];

    for(int i = 0; i < a_element; i++){
        A[i]=1;
        //A[i] = (rand() % 3) + 1;
    }
    for(int i = 0; i < b_element; i++){
        //B[i] = (rand() % 3) + 1;
        B[i]=1;
    }


    int *DA, *DB, *DC;
    cudaMalloc(&DA, a_element * sizeof(int));
    cudaMalloc(&DB, b_element * sizeof(int));
    cudaMalloc(&DC, c_element * sizeof(int)); //need after the multiplication
    //host holo cpu and device holo gpu
    cudaMemcpy(DA, A, a_element * sizeof(int), cudaMemcpyHostToDevice); //cpu theke value gpu te pass
    cudaMemcpy(DB, B, b_element * sizeof(int) , cudaMemcpyHostToDevice);

    cout << "Enter the number of core you want to use." << endl;
    int number_of_core=4;


    cudaEvent_t start, end;
    
    cudaEventCreate(&start);
    cudaEventCreate(&end);

    cudaEventRecord(start);// time calculation of starting time

    matrix_multiplication <<< 1 , number_of_core >>> (DA, DB, DC, N , M , P,number_of_matrix, number_of_core);


    cudaEventRecord(end);
    cudaEventSynchronize(end);
    float time_totall = 0;
    cudaEventElapsedTime(&time_totall, start, end);

    cout << "Time taken to the gpu: with " << number_of_core << " is : " << time_totall << " miliseconds." << endl;

    cudaMemcpy(C, DC, c_element * sizeof(int), cudaMemcpyDeviceToHost);

    for(int i = 0; i < N; i++){
        for(int j = 0; j < P; j++){
            cout << C[i * P + j] << " ";
        }
        cout << endl;
    }

    return 0;

}