//operator overloading in cpp in OOPS
#include <iostream>
#include <string> 
using namespace std; 

struct yt_channel{
    string name; 
    int sub_num; 
    yt_channel(string name1, int number){
        name = name1; 
        sub_num = number; 
    }
}; 

void operator<<(ostream& COUT, yt_channel& yt1){
    cout << "name: "<<yt1.name<<endl; 
    cout << "sub_number: "<<yt1.sub_num <<endl;  
}

int main(){
    yt_channel yt1 = yt_channel("coding", 20000); 
    cout <<yt1.name<<endl; 
    cout <<yt1; 
    return 0; 
}