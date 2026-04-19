//friend function 
#include <iostream> 
#include <math.h>
using namespace std; 

//for an equaliteral trianle
class trianlge{
    private: 
    int a; 
    float circumference; 
    float area; 

    public: 
    
    void setA(int length){
        a = length; 
        circumference = 3 * a;
        area =  sqrt(3) * pow(a, 2) /4;  
    }
    friend void PrintResults(trianlge); 
    friend class print_results; 
}; 

void PrintResults(trianlge a){
    cout << "circumference: " << a.circumference <<endl; 
    cout << "area: " << a.area << endl; 
}

class print_results{
    public: 
    void PrintResults2(trianlge a){
    cout << "circumference: " << a.circumference <<endl; 
    cout << "area: " << a.area << endl; 
}
}; 


int main()
{
    trianlge a; 
    a.setA(3); 
    PrintResults(a); 

    print_results t1; 
    t1.PrintResults2(a); 

    cout << "hello world" <<endl; 
    cout << "hello this code is written inside the ipad " <<endl; 
    return 0; 
}