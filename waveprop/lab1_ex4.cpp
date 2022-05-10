#include <iostream>
#include <fstream>
#include <cstring>

void equalArray(int arr1[5][5], int arr2[5][5])
{
    for(int i = 0; i < 5; i++)
    {
        for(int j = 0; j < 5; j++) 
        {
            arr1[i][j] = arr2[i][j];
        }
    }
}

void test(int *arr1, int size1, int *arr2, int size2)
{
    int size_minimum = 0;

    if(size1 <= size2)
    {
        size_minimum = size1;
    }
    else
    {
        size_minimum = size2;
    }

    for(int i = 0; i < size_minimum; i++)
    {
        if(arr1[i] != arr2[i])
        {
            std::cout << "TEST FAILED\n";
            break;
        }
    }
}


int main(int argc, char** argv)
{
    int data1D [5][5]= 
    {
        {1,2,3,4,5},
        {6,7,8,9,10},
        {11,12,13,14,15},
        {16,17,18,19,20},
        {21,22,23,24,25}
    };
    

    int q1[5][5];

    int data1[5] = {1, 2, 3, 4, 5};
    int data2[5] = {1, 2, 3, 4, 5};

    std::cout << "Code ex4\n";
    
    test(data1, sizeof(data1)/sizeof(data1[0]), data2, sizeof(data2)/sizeof(data2[0]));
}