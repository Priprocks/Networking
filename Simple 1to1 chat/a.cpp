#include<iostream>
using namespace std;
int main()
{
    char a[100];
    a[0]='x';
    cout<<*a<<"\n"<<*(&a);
    return 0;
}
