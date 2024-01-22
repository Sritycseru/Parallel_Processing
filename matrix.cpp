#include <bits/stdc++.h>
using namespace std;
#include <mpi.h>

int cnt = 0;
void print_matrix(const vector<int> &matrix,int N, int P)
{
    for (int i = 0; i < matrix.size(); i++)
    {
        if(i%(N*P) == 0){
            cout << endl;
            cnt++;
            printf("Matrix: %d\n",cnt);
        }
        cout << matrix[i] << " ";
        if ((i + 1) % P == 0)
            cout << endl;
    }
}

/// you can modify this function for only one matrix print removing top loop
vector<int> matrix_multiplication(int *A, int *B, int N, int M, int P, int K)
{
    vector<int> ans(N * P * K);

    for (int i = 0; i < K; i++)
    {
        int add_A = i * N * M;
        int add_B = i * M * P;
        int add_C = i * N * P;

        for (int j = 0; j < N; j++)
        {
            for (int k = 0; k < P; k++)
            {
                int sum = 0;

                for (int row = 0; row < M; row++)
                {
                    sum += A[add_A + j * N + row] * B[add_B + P * row + k];
                }
                ans[add_C + j * P + k] = sum;
            }
        }
    }
    return ans;
}
int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);

    int size;
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // cout << task_id << " out of " << no_of_processor << endl;

    if (rank == 0)
    {

        int N, M, P;
        N = atoi(argv[1]);
        M = atoi(argv[2]);
        P = atoi(argv[3]);

        //cout << "Enter the dimenssion of the matrix: ";
        //cin >> N >> M >> P;
        //cout << "Enter the number of matrix: ";
        int K = atoi(argv[4]);
        //cin >> K;
        // if(int(argc)>K){
        //     cout <<"Number of process must be less than or equal to the number of matrix"<<endl;
        //     MPI_Abort(MPI_COMM_WORLD, 1);
        // }
        int *A = new int[N * M * K];
        int *B = new int[M * P * K]; /// array declare is same as pointer then you got right output

        /// input iccamoto nibo
        for (int i = 0; i < N * M * K; i++)
              A[i]=1;
            //A[i] = rand()%5+1;
        for (int i = 0; i < M * P * K; i++)
            //B[i] = rand()%5+1;
              B[i]=1;

        double start_time, end_time;
        start_time = MPI_Wtime();

        for (int i = 1; i < size; i++)
        {
            int start_index = i * (K / size);
            int end_index = (i + 1) * (K / size);
            if (i == size- 1)
                end_index = K;
            // send number of matrix

            int number_per_process = end_index - start_index;
            MPI_Send(&number_per_process, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            // send N M P
            MPI_Send(&N, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(&M, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(&P, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            // send A array
            //
            int start_A = start_index * N * M; /// for detecting next array starting index A
            int end_A = end_index * N * M;
            int arr_elemnt = end_A - start_A;

            /// send array_A fixed value to nProcess
            MPI_Send(&arr_elemnt, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(&(A[start_A]), arr_elemnt, MPI_INT, i, 0, MPI_COMM_WORLD);
            ////send B array

            int start_B = start_index * M * P;
            int end_B = end_index * M * P;
            arr_elemnt = end_B - start_B;

            /// send array_B fixed value to nProcess
            MPI_Send(&arr_elemnt, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(&(B[start_B]), arr_elemnt, MPI_INT, i, 0, MPI_COMM_WORLD);
        }

        vector<int> ans = matrix_multiplication(A, B, N, M, P, K / size);
        cout <<"Printing matrix from only rank 0"<<endl;
        print_matrix(ans,N,P);
        
        end_time = MPI_Wtime();
        cout << "TIME " << end_time - start_time << endl;
    }
    else
    {
        int K, N, M, P;
        MPI_Recv(&K, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&N, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&M, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&P, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        /// for take size of A and the Array
        int size_A, size_B;
        MPI_Recv(&size_A, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        //vector<int> A(size_A);
        int *A = new int[K*N*M];
        MPI_Recv(&A[0], size_A, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        /// for take size of B and the Array
        MPI_Recv(&size_B, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        //vector<int> B(size_B);
        int *B = new int[K*M*P];
        MPI_Recv(&B[0], size_B, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        vector<int> c = matrix_multiplication(&A[0], &B[0], N, M, P, K);
        //print_matrix(c,N,P);

        /// you can print also the value of multiplication
    }

    MPI_Finalize();
    return 0;
}
