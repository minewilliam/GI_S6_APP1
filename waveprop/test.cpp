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
int main()
{
    std::vector<float> h(10, 0);
    std::vector<float> retour(10, 0);


    std::cout << sin(3.14/2) << std::endl;
    
    return 0;
}