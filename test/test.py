#!/usr/bin/env python

from pprint import pprint

def swap(v1, v2):
    return v2, v1

def scramble(data, nn):
    '''
    n = nn<<1;
    j=1;
    for (i=1; i<n; i+=2) {
        if (j>i) {
            swap(data[j-1], data[i-1]);
            swap(data[j], data[i]);
        }
        m = nn;
        while (m>=2 && j>m) {
            j -= m;
            m >>= 1;
        }
        j += m;
    };
    '''
    n = nn<<1
    j = 1
    for i in range(0, n, 2):
        if j > i:
            data[j-1], data[i-1] = swap(data[j-1], data[i-1])
            data[j], data[i] = swap(data[j], data[i])

        m = nn
        while m >= 2 and j > m:
            j -= m
            m >>= 1

        j += m

    return data

if __name__ == '__main__':
    data = []
    n = 16

    for i in range(n):
        data.append(i)
        data.append(0)

    print(data)

    data = scramble(data, n)

    print(data)