#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>

using namespace std;

int main()
{
    int x = 22;
    int* a = &x;

    cout << x << endl;
    cout << a << endl;
    cout << a+0x10 << endl;
    cout << *(a+0x10) << endl;

    return 0;
}
