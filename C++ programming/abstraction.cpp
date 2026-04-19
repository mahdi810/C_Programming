//abstraction in OOPS 

#include <iostream>
using namespace std; 

class SmartPhone {
public: 
    virtual void TakeASelfie() = 0;  //to make it a pure virtual function 
    virtual void MakeACall() = 0; 
}; 

class Android : public SmartPhone {
public: 
    void TakeASelfie(){
        cout << "this is an Android taking a selfie  \n"; 
    }
    void MakeACall(){
        cout << "android making a call \n"; 
    }
}; 

class Iphone : public SmartPhone {
public: 
    void TakeASelfie(){
        cout << "this is an Iphone taking a selfie \n"; 
    }

    void MakeACall(){
        cout << "Iphone is calling \n"; 
    }
}; 


int main(){
    SmartPhone* s1 = new Android(); 
    s1->TakeASelfie(); 
    s1->MakeACall(); 

    SmartPhone* s2 = new Iphone(); 
    s2->TakeASelfie();
    s2->MakeACall();  
    return 0; 
}