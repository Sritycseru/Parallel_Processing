
#include <bits/stdc++.h>
using namespace std;
__global__ void kmp(char *P, char *T, int *index, int p_len,int t_len,int number_of_core, int number_of_contact)
{
    int idx = threadIdx.x;
    int start_index = idx * (number_of_contact / number_of_core);
    int end_index = (idx + 1) * (number_of_contact / number_of_core);
    int star_count = start_index;
    if (idx == number_of_core - 1)
    {
        end_index = t_len;
    }else{
    	end_index = index[end_index];
    }
    int fnd=0;
    for (int i = index[start_index]; i < end_index; i++)
    {
        if(T[i]=='*'){fnd=0; star_count++;}
        if(T[i]==P[fnd]) fnd++;
        
        else fnd=0;
        
        if(fnd==p_len){
            index[star_count]= -1;
            fnd=0;
        }
    }
}

int main(int argc ,char *argv[]){

    string name="Jodey";
    vector<string>contact_list;
    vector<string> file_names(argv + 1, argv + argc);
        for (auto file_name: file_names)
        {
            ifstream file(file_name);
            string text1;
            while (getline(file,text1))
            {
                contact_list.push_back(text1);
            }
            file.close();
        }


    int number_of_core = 4;
    

    string text = "";
   int number_of_contact=contact_list.size();
   // cout<<number_of_contact<<endl;

    vector <int> contact_list_index(number_of_contact);
   for(int i = 0 ; i < number_of_contact; i++){
        contact_list_index[i]=text.size();
        text += contact_list[i] + "*";
    }
    char *DT, *PT;
    int m = text.size() + 1;
    cudaMalloc(&DT, m);
    cudaMemcpy(DT, &text[0], m, cudaMemcpyHostToDevice);

    int p_len = name.size();
    cudaMalloc(&PT, p_len);
    cudaMemcpy(PT, &name[0], p_len, cudaMemcpyHostToDevice);

    int *index;

    cudaMalloc(&index, number_of_contact * sizeof(int));
    cudaMemcpy(index, &contact_list_index[0], number_of_contact * sizeof(int), cudaMemcpyHostToDevice);

    cudaEvent_t start, end;
    cudaEventCreate(&start);
    cudaEventCreate(&end);
    cudaEventRecord(start); //start time
    kmp<<<1, number_of_core>>>(PT,DT, index, p_len,m, number_of_core,number_of_contact);

    cudaEventRecord(end);
    cudaEventSynchronize(end);
    float milliseconds = 0;
    cudaEventElapsedTime(&milliseconds, start, end);
    cout << "Time taken to the gpu: with " << number_of_core << " is : " << milliseconds << " miliseconds." << endl;
     vector<int> res(number_of_contact);
    cudaMemcpy(&res[0], index, number_of_contact * sizeof(int), cudaMemcpyDeviceToHost);
    set<string>st;

    for (int i = 1; i < number_of_contact; i++)
    {
        if (res[i] == -1)
            st.insert(contact_list[i]);

    }
    for(auto x:st){
      cout<<x<<endl;
    }
}