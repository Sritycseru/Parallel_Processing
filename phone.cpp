#include <bits/stdc++.h>

#include <mpi.h>
using namespace std;

set<string>st;
void send_string(string str,  int to)
{
    int length = str.size() + 1;
    MPI_Send(&length, 1, MPI_INT, to, 0, MPI_COMM_WORLD);
    MPI_Send(str.c_str(), length, MPI_CHAR, to, 0, MPI_COMM_WORLD);
}

string get_string(int from)
{
    int length;
    MPI_Recv(&length, 1, MPI_INT, from, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    char *str;
    str = new char[length];
    // char str[] = new char[length]
    // string str(length)
    MPI_Recv(str, length, MPI_CHAR, from, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    return string(str);

}
string create_string(vector<string> &str, int from, int to)
{
    string ans = "";
    for(int i = from ; i < to ; i++)
    {
        ans += str[i] + "*";
    }
    return ans;
}
vector<string> split_string(string str)
{
    vector<string>ans;
    string tmp;
    for(int i=0;i<str.size();i++)
    {
        char ch=str[i];
        if(ch !='*')
            tmp+=ch;
        else
        {
            ans.push_back(tmp);
            tmp="";

        }

    }
    return ans;
}
void check(string text, string pattern)
{
    int n = text.size();
    int m = pattern.size();
    int j = 0;
    for(int i = 0; i < n; i++)
    {
        if(text[i] == pattern[j]) j++;
        else j=0;

        if(j == m)
        {
           j=0;
           st.insert(text);
            break;
        }
    }
}
int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);
    int number_of_task;
    int task_id;
    MPI_Comm_rank(MPI_COMM_WORLD, &task_id);
    MPI_Comm_size(MPI_COMM_WORLD, &number_of_task);


    string name="Leo";
    // tempalte
    if(task_id == 0)
    {


        vector <string> contact_list;

        /// input from two or more input file

        vector<string> file_names(argv + 1, argv + argc);
        for (auto file_name: file_names)
        {
            ifstream file(file_name);
            string text;
            while (getline(file,text))
            {
                contact_list.push_back(text);
            }
            file.close();
        }
        /// end of taking input u can see input using contact_back_to_str printing
//        for(auto x:contact_list)
//        {
//            cout<<x<<endl;
//        }
        int number_of_contact=contact_list.size();
//        cout<<number_of_contact<<endl;

        double start_time,end_time;
        start_time = MPI_Wtime();
        int p_len=name.size();
        for(int i =1 ; i< number_of_task ; i++)
        {
            int start_position = i * (number_of_contact / number_of_task);
            int end_position = (i + 1) * (number_of_contact / number_of_task);
            if( i == number_of_task - 1) end_position = number_of_contact;

            //send text strings
            string for_send_contact = create_string(contact_list, start_position, end_position);
//            send_string(for_send_contact, i);
            int length=for_send_contact.size();
            MPI_Send(&length, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(for_send_contact.c_str(), length, MPI_CHAR, i, 0, MPI_COMM_WORLD);

        }
        // for root process calculation or checking
        contact_list.resize( number_of_contact / number_of_task);

        int n = contact_list.size();
        for(int i = 0; i < n; i++)
        {
//            cout<<contact_list[i]<<endl;
            check(contact_list[i], name);

        }
        end_time=MPI_Wtime();
        cout<<endl;
        cout<<"totall time needed "<<end_time-start_time<<endl<<endl;


    }
    else
    {
        string temp_contact=get_string(0);
        vector<string>contact_list=split_string(temp_contact);

        
        for(int i = 0; i < contact_list.size(); i++)
        {
//            cout<<contact_list[i]<<endl;
            check(contact_list[i], name);
        }

    }
    for(auto str:st){
       cout<<str<<endl;
    }
    MPI_Finalize();
    return 0;
}
