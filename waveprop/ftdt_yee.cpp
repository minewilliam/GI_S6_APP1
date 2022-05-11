#include <iostream>
#include <vector>
#include <mutex>
#include <thread>
#include <math.h>
#include <future>

const int n = 100;
const double courant = 0.1;

namespace {
    std::mutex mutex;

}

//source strenght through time
double power_source(int index)
{
    return 0.1*sin(0.1*index);
}

//wave propagation in all matrice
//H -= courant_number * curl_E(E)
std::vector<std::vector<std::vector<double>>> propagation_H(
    std::vector<std::vector<std::vector<double>>> curl_E, 
    std::vector<std::vector<std::vector<double>>> H)
{
    std::unique_lock<std::mutex> lock(mutex);
    for(int x = 0; x < n; x++)
    {
        for(int y = 0; y < n; y++)
        {
            for(int z = 0; z < n; z++)
            {
                H[x][y][z] -= curl_E[x][y][z] * courant;
            }
        }
    }
    lock.unlock();

    return H;
}

//E += courant_number * curl_H(H) 
std::vector<std::vector<std::vector<double>>> propagation_E(
    std::vector<std::vector<std::vector<double>>> curl_H,
    std::vector<std::vector<std::vector<double>>> E)
{
    std::unique_lock<std::mutex> lock(mutex);
    for(int x = 0; x < n; x++)
    {
        for(int y = 0; y < n; y++)
        {
            for(int z = 0; z < n; z++)
            {
                E[x][y][z] += curl_H[x][y][z] * courant;
            }
        }
    }
    lock.unlock();

    return E;
}

//f1 & f2
std::vector<std::vector<std::vector<double>>> curl_f0( 
    std::vector<std::vector<std::vector<double>>> f1, 
    std::vector<std::vector<std::vector<double>>> f2)
{
    std::vector<std::vector<std::vector<double>>> curl = std::vector<std::vector<std::vector<double>>>(100, std::vector<std::vector<double>>(100, std::vector<double>(100)));
    for(int x = 1; x < n-1; x++)
    {
        for(int y = 1; y < n-1; y++)
        {
            for(int z = 1; z < n-1; z++)
            {
                curl[x][y][z] = (f2[x][y+1][z] - f2[x][y-1][z] - f1[x][y][z+1] + f1[x][y][z-1])/2;
            }
        }
    }

    return curl;
}

//f0 & f2
std::vector<std::vector<std::vector<double>>> curl_f1(
    std::vector<std::vector<std::vector<double>>> f0, 
    std::vector<std::vector<std::vector<double>>> f2)
{
    std::vector<std::vector<std::vector<double>>> curl = std::vector<std::vector<std::vector<double>>>(100, std::vector<std::vector<double>>(100, std::vector<double>(100)));
    for(int x = 1; x < n-1; x++)
    {
        for(int y = 1; y < n-1; y++)
        {
            for(int z = 1; z < n-1; z++)
            {
                curl[x][y][z] = (f0[x][y][z+1] - f0[x][y][z-1] - f2[x+1][y][z] + f2[x-1][y][z])/2;
            }
        }
    }

    return curl;
}

//f0 & f1
std::vector<std::vector<std::vector<double>>> curl_f2(
    std::vector<std::vector<std::vector<double>>> f0, 
    std::vector<std::vector<std::vector<double>>> f1)
{
    std::vector<std::vector<std::vector<double>>> curl = std::vector<std::vector<std::vector<double>>>(100, std::vector<std::vector<double>>(100, std::vector<double>(100)));
    for(int x = 1; x < n-1; x++)
    {
        for(int y = 1; y < n-1; y++)
        {
            for(int z = 1; z < n-1; z++)
            {
                curl[x][y][z] = (f1[x+1][y][z] - f1[x-1][y][z] - f0[x][y+1][z] + f0[x][y-1][z])/2;
            }
        }
    }

    return curl;
}
int main()
{
    //F
    std::vector<std::vector<std::vector<double>>> f0_E;
    f0_E = std::vector<std::vector<std::vector<double>>>(100, std::vector<std::vector<double>>(100, std::vector<double>(100)));
    std::vector<std::vector<std::vector<double>>> f1_E;
    f1_E = std::vector<std::vector<std::vector<double>>>(100, std::vector<std::vector<double>>(100, std::vector<double>(100)));
    std::vector<std::vector<std::vector<double>>> f2_E;
    f2_E = std::vector<std::vector<std::vector<double>>>(100, std::vector<std::vector<double>>(100, std::vector<double>(100)));

    std::vector<std::vector<std::vector<double>>> f0_H;
    f0_H = std::vector<std::vector<std::vector<double>>>(100, std::vector<std::vector<double>>(100, std::vector<double>(100)));
    std::vector<std::vector<std::vector<double>>> f1_H;
    f1_H = std::vector<std::vector<std::vector<double>>>(100, std::vector<std::vector<double>>(100, std::vector<double>(100)));
    std::vector<std::vector<std::vector<double>>> f2_H;
    f2_H = std::vector<std::vector<std::vector<double>>>(100, std::vector<std::vector<double>>(100, std::vector<double>(100)));

    //curl
    std::vector<std::vector<std::vector<double>>> f0_E_curl;
    f0_E_curl = std::vector<std::vector<std::vector<double>>>(100, std::vector<std::vector<double>>(100, std::vector<double>(100)));
    std::vector<std::vector<std::vector<double>>> f1_E_curl;
    f1_E_curl = std::vector<std::vector<std::vector<double>>>(100, std::vector<std::vector<double>>(100, std::vector<double>(100)));
    std::vector<std::vector<std::vector<double>>> f2_E_curl;
    f2_E_curl = std::vector<std::vector<std::vector<double>>>(100, std::vector<std::vector<double>>(100, std::vector<double>(100)));

    std::vector<std::vector<std::vector<double>>> f0_H_curl;
    f0_H_curl = std::vector<std::vector<std::vector<double>>>(100, std::vector<std::vector<double>>(100, std::vector<double>(100)));
    std::vector<std::vector<std::vector<double>>> f1_H_curl;
    f1_H_curl = std::vector<std::vector<std::vector<double>>>(100, std::vector<std::vector<double>>(100, std::vector<double>(100)));
    std::vector<std::vector<std::vector<double>>> f2_H_curl;
    f2_H_curl = std::vector<std::vector<std::vector<double>>>(100, std::vector<std::vector<double>>(100, std::vector<double>(100)));

    int source_position_x = floor(n / 3);
    int source_position_y = floor(n / 3);
    int source_position_z = floor(n / 2);
    int index = 0;

    std::cout.precision(4);
    
    while(true)
    {
        auto future_curl_f0_E = std::async (curl_f0, f1_E, f2_E);
        auto future_curl_f1_E = std::async (curl_f1, f0_E, f2_E);
        auto future_curl_f2_E = std::async (curl_f2, f0_E, f1_E);
        
        /************************* Propagation *************************/

        auto future_f0_E = std::async(propagation_E, f0_H_curl, f0_E);
        auto future_f1_E = std::async(propagation_E, f1_H_curl, f1_E);
        auto future_f2_E = std::async(propagation_E, f2_H_curl, f2_E);

        f0_E = future_f0_E.get();
        f1_E = future_f1_E.get();
        f2_E = future_f2_E.get();

        f0_E_curl = future_curl_f0_E.get();
        f1_E_curl = future_curl_f1_E.get();
        f2_E_curl = future_curl_f2_E.get();

        //std::cout << index << " " << f0_E[source_position_x][source_position_y][source_position_z] << std::endl;

        f0_E[source_position_x][source_position_y][source_position_z] += power_source(index);
        //std::cout << index << " " << f0_E[source_position_x][source_position_y][source_position_z] << std::endl;

        auto future_curl_f0_H = std::async(curl_f0, f1_H, f2_H);
        auto future_curl_f1_H = std::async(curl_f1, f0_H, f2_H);
        auto future_curl_f2_H = std::async(curl_f2, f0_H, f1_H);

        /************************* Propagation *************************/

        auto future_f0_H = std::async(propagation_H, f0_E_curl, f0_H);
        auto future_f1_H = std::async(propagation_H, f1_E_curl, f1_H);
        auto future_f2_H = std::async(propagation_H, f2_E_curl, f2_H);

        f0_H = future_f0_H.get();
        f1_H = future_f1_H.get();
        f2_H = future_f2_H.get();
        
        f0_H_curl = future_curl_f0_H.get();
        f1_H_curl = future_curl_f1_H.get();
        f2_H_curl = future_curl_f2_H.get();

        //increase in time
        index++;
        
        f0_E_curl = std::vector<std::vector<std::vector<double>>>(100, std::vector<std::vector<double>>(100, std::vector<double>(100)));
        f1_E_curl = std::vector<std::vector<std::vector<double>>>(100, std::vector<std::vector<double>>(100, std::vector<double>(100)));
        f2_E_curl = std::vector<std::vector<std::vector<double>>>(100, std::vector<std::vector<double>>(100, std::vector<double>(100)));
        
        f0_H_curl = std::vector<std::vector<std::vector<double>>>(100, std::vector<std::vector<double>>(100, std::vector<double>(100)));
        f1_H_curl = std::vector<std::vector<std::vector<double>>>(100, std::vector<std::vector<double>>(100, std::vector<double>(100)));
        f2_H_curl = std::vector<std::vector<std::vector<double>>>(100, std::vector<std::vector<double>>(100, std::vector<double>(100)));
        std::cout << index << " > " << f0_E[source_position_x][source_position_y][source_position_z] << std::endl;
    }
    
    return 0;
}