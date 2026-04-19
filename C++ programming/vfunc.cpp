//virtual function 
#include <iostream>
using namespace std; 

class Instrument {
public: 
    virtual void MakeSound() = 0; 
private: 
}; 

class Accordion : public Instrument{
public: 
    void MakeSound()
    {
        cout <<"Accordion is playing "<<endl; 
    }
}; 

class Piano : public Instrument {
public: 
    void MakeSound(){
        cout <<"piano is playing \n"; 
    }
}; 

int main()
{
    Instrument* i1 = new Accordion(); 
    i1->MakeSound();  

    Instrument* i2 = new Piano(); 
    i2->MakeSound(); 
    return 0; 
}