#include <iostream>
#include <vector>
#include <future>
#include <math.h>

std::vector<float> modification(std::vector<float> h)
{
    h[2] = 2;

    return h;
}
std::vector<float> modification2(std::vector<float> h)
{
    h[2] = 4;

    return h;
}

float source(int index)
{
    return 0.1*sin(0.1*index);
}

int main()
{
    float puissance = 0;
    int index = 0;

    while(true)
    {
        puissance += source(index);
        puissance = 0.1 * puissance;
        index++;
        std::cout << puissance << std::endl;
    }
    
    return 0;
}