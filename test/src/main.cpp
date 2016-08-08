#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>

using namespace std;

int main()
{
    unsigned long nn = 1;
    unsigned long n = nn<<10;

    cout << nn << endl;
    cout << n << endl;
    cout << "char " << sizeof(char) * 8 << endl;
    cout << "long " << sizeof(long) * 8 << endl;
    cout << "int " << sizeof(int) * 8 << endl;
    cout << "unsigned long " << sizeof(unsigned long long) * 8 << endl;

    return 0;
}
