#include <iostream>
#include <vector>
#include <mutex>
#include <thread>
#include <math.h>
#include <future>

const int n = 100;
const float courant = 0.2;

namespace {
    std::mutex mutex;

}

//source strenght through time
float power_source(int index)
{
    return 0.1*sin(0.1*index);
}

//wave propagation in all matrice
//H -= courant_number * curl_E(E)
std::vector<std::vector<std::vector<float>>> propagation_H(
    std::vector<std::vector<std::vector<float>>> curl_E, 
    std::vector<std::vector<std::vector<float>>> H)
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
std::vector<std::vector<std::vector<float>>> propagation_E(
    std::vector<std::vector<std::vector<float>>> curl_H,
    std::vector<std::vector<std::vector<float>>> E)
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

//add two matrice
//a - b
std::vector<std::vector<std::vector<float>>> somme_matrice(std::vector<std::vector<std::vector<float>>> somme,
                                                            std::vector<std::vector<std::vector<float>>> a,
                                                            std::vector<std::vector<std::vector<float>>> b)
{
    std::unique_lock<std::mutex> lock(mutex);
    for(int x = 0; x < n; x++)
    {
        for(int y = 0; y < n; y++)
        {
            for(int z = 0; z < n; z++)
            {
                somme[x][y][z] = a[x][y][z] - b[x][y][z];
            }
        }
    }
    lock.unlock();

    return somme;
}

/********************************** curl E **********************************/

/*
1
curl_E[:, :-1, :, 0] += E[:, 1:, :, 2] - E[:, :-1, :, 2]
*/
std::vector<std::vector<std::vector<float>>> line_curl_E_1(
    std::vector<std::vector<std::vector<float>>> f,
    std::vector<std::vector<std::vector<float>>> retour)
{
    std::unique_lock<std::mutex> lock(mutex);
    for(int x = 0; x < n; x++)
    {
        for(int y = 0; y < n-1; y++)
        {
            for(int z = 0; z < n; z++)
            {
                retour[x][y][z] += f[x][y+1][z] - f[x][y][z];
            }
        }
    }
    lock.unlock();

    return retour;
}

/*
2
curl_E[:, :, :-1, 0] -= E[:, :, 1:, 1] - E[:, :, :-1, 1]
*/
std::vector<std::vector<std::vector<float>>> line_curl_E_2(
    std::vector<std::vector<std::vector<float>>> f,
    std::vector<std::vector<std::vector<float>>> retour)
{
    std::unique_lock<std::mutex> lock(mutex);
    for(int x = 0; x < n; x++)
    {
        for(int y = 0; y < n; y++)
        {
            for(int z = 0; z < n-1; z++)
            {
                retour[x][y][z] -= f[x][y][z+1] - f[x][y][z];
            }
        }
    }
    lock.unlock();

    return retour;
}

/*
3
curl_E[:, :, :-1, 1] += E[:, :, 1:, 0] - E[:, :, :-1, 0]
*/
std::vector<std::vector<std::vector<float>>> line_curl_E_3(
    std::vector<std::vector<std::vector<float>>> f,
    std::vector<std::vector<std::vector<float>>> retour)
{
    std::unique_lock<std::mutex> lock(mutex);
    for(int x = 0; x < n; x++)
    {
        for(int y = 0; y < n; y++)
        {
            for(int z = 0; z < n-1; z++)
            {
                retour[x][y][z] += f[x][y][z+1] - f[x][y][z];
            }
        }
    }
    lock.unlock();

    return retour;
}

/*
4
curl_E[:-1, :, :, 1] -= E[1:, :, :, 2] - E[:-1, :, :, 2]
*/
std::vector<std::vector<std::vector<float>>> line_curl_E_4(
    std::vector<std::vector<std::vector<float>>> f,
    std::vector<std::vector<std::vector<float>>> retour)
{
    std::unique_lock<std::mutex> lock(mutex);
    for(int x = 0; x < n-1; x++)
    {
        for(int y = 0; y < n; y++)
        {
            for(int z = 0; z < n; z++)
            {
                retour[x][y][z] -= f[x+1][y][z] - f[x][y][z];
            }
        }
    }
    lock.unlock();

    return retour;
}

/*
5
curl_E[:-1, :, :, 2] += E[1:, :, :, 1] - E[:-1, :, :, 1]
*/
std::vector<std::vector<std::vector<float>>> line_curl_E_5(
    std::vector<std::vector<std::vector<float>>> f,
    std::vector<std::vector<std::vector<float>>> retour)
{
    std::unique_lock<std::mutex> lock(mutex);
    for(int x = 0; x < n-1; x++)
    {
        for(int y = 0; y < n; y++)
        {
            for(int z = 0; z < n; z++)
            {
                retour[x][y][z] += f[x+1][y][z] - f[x][y][z];
            }
        }
    }
    lock.unlock();

    return retour;
}

/*
6
curl_E[:, :-1, :, 2] -= E[:, 1:, :, 0] - E[:, :-1, :, 0]
*/
std::vector<std::vector<std::vector<float>>> line_curl_E_6(
    std::vector<std::vector<std::vector<float>>> f,
    std::vector<std::vector<std::vector<float>>> retour)
{
    std::unique_lock<std::mutex> lock(mutex);
    for(int x = 0; x < n; x++)
    {
        for(int y = 0; y < n-1; y++)
        {
            for(int z = 0; z < n; z++)
            {
                retour[x][y][z] -= f[x][y+1][z] - f[x][y][z];
            }
        }
    }
    lock.unlock();

    return retour;
}

/********************************** curl H **********************************/

/*
1
curl_H[:,1:,:,0] += H[:,1:,:,2] - H[:,:-1,:,2]
*/
std::vector<std::vector<std::vector<float>>> line_curl_H_1(
    std::vector<std::vector<std::vector<float>>> f,
    std::vector<std::vector<std::vector<float>>> retour)
{
    std::unique_lock<std::mutex> lock(mutex);
    for(int x = 0; x < n; x++)
    {
        for(int y = 1; y < n; y++)
        {
            for(int z = 0; z < n; z++)
            {
                retour[x][y][z] += f[x][y][z] - f[x][y-1][z];
            }
        }
    }
    lock.unlock();

    return retour;
}

/*
2
curl_H[:,:,1:,0] -= H[:,:,1:,1] - H[:,:,:-1,1]
*/
std::vector<std::vector<std::vector<float>>> line_curl_H_2(
    std::vector<std::vector<std::vector<float>>> f,
    std::vector<std::vector<std::vector<float>>> retour)
{
    std::unique_lock<std::mutex> lock(mutex);
    for(int x = 0; x < n; x++)
    {
        for(int y = 0; y < n; y++)
        {
            for(int z = 1; z < n; z++)
            {
                retour[x][y][z] -= f[x][y][z] - f[x][y][z-1];
            }
        }
    }
    lock.unlock();

    return retour;
}

/*
3
curl_H[:,:,1:,1] += H[:,:,1:,0] - H[:,:,:-1,0]
*/
std::vector<std::vector<std::vector<float>>> line_curl_H_3(
    std::vector<std::vector<std::vector<float>>> f,
    std::vector<std::vector<std::vector<float>>> retour)
{
    std::unique_lock<std::mutex> lock(mutex);
    for(int x = 0; x < n; x++)
    {
        for(int y = 0; y < n; y++)
        {
            for(int z = 1; z < n; z++)
            {
                retour[x][y][z] += f[x][y][z] - f[x][y][z-1];
            }
        }
    }
    lock.unlock();

    return retour;
}

/*
4
curl_H[1:,:,:,1] -= H[1:,:,:,2] - H[:-1,:,:,2]
*/
std::vector<std::vector<std::vector<float>>> line_curl_H_4(
    std::vector<std::vector<std::vector<float>>> f,
    std::vector<std::vector<std::vector<float>>> retour)
{
    std::unique_lock<std::mutex> lock(mutex);
    for(int x = 1; x < n; x++)
    {
        for(int y = 0; y < n; y++)
        {
            for(int z = 0; z < n; z++)
            {
                retour[x][y][z] -= f[x][y][z] - f[x-1][y][z];
            }
        }
    }
    lock.unlock();

    return retour;
}

/*
5
curl_H[1:,:,:,2] += H[1:,:,:,1] - H[:-1,:,:,1]
*/
std::vector<std::vector<std::vector<float>>> line_curl_H_5(
    std::vector<std::vector<std::vector<float>>> f,
    std::vector<std::vector<std::vector<float>>> retour)
{
    std::unique_lock<std::mutex> lock(mutex);
    for(int x = 1; x < n; x++)
    {
        for(int y = 0; y < n; y++)
        {
            for(int z = 0; z < n; z++)
            {
                retour[x][y][z] += f[x][y][z] - f[x-1][y][z];
            }
        }
    }
    lock.unlock();

    return retour;
}

/*
6
curl_H[:,1:,:,2] -= H[:,1:,:,0] - H[:,:-1,:,0]
*/
std::vector<std::vector<std::vector<float>>> line_curl_H_6(
    std::vector<std::vector<std::vector<float>>> f,
    std::vector<std::vector<std::vector<float>>> retour)
{
    std::unique_lock<std::mutex> lock(mutex);
    for(int x = 0; x < n; x++)
    {
        for(int y = 1; y < n; y++)
        {
            for(int z = 0; z < n; z++)
            {
                retour[x][y][z] -= f[x][y][z] - f[x][y-1][z];
            }
        }
    }
    lock.unlock();

    return retour;
}

std::vector<std::vector<std::vector<float>>> curl_f0(
    std::vector<std::vector<std::vector<float>>> f0, 
    std::vector<std::vector<std::vector<float>>> f1, 
    std::vector<std::vector<std::vector<float>>> f2)
{
    std::vector<std::vector<std::vector<float>>> curl = std::vector<std::vector<std::vector<float>>>(100, std::vector<std::vector<float>>(100, std::vector<float>(100)));
    for(int x = 1; x < n-1; x++)
    {
        for(int y = 1; y < n-1; y++)
        {
            for(int z = 1; z < n-1; z++)
            {
                curl[x][y][z] = (f2[x][y-1][z] - f2[x][y+1][z] - f1[x][y][z-1] + f1[x][y][z+1])/2;
            }
        }
    }

    return curl;
}

std::vector<std::vector<std::vector<float>>> curl_f1(
    std::vector<std::vector<std::vector<float>>> f0, 
    std::vector<std::vector<std::vector<float>>> f1, 
    std::vector<std::vector<std::vector<float>>> f2)
{
    std::vector<std::vector<std::vector<float>>> curl = std::vector<std::vector<std::vector<float>>>(100, std::vector<std::vector<float>>(100, std::vector<float>(100)));
    for(int x = 1; x < n-1; x++)
    {
        for(int y = 1; y < n-1; y++)
        {
            for(int z = 1; z < n-1; z++)
            {
                curl[x][y][z] = (f0[x][y][z-1] - f0[x][y+1][z+1] - f2[x-1][y][z] + f2[x+1][y][z])/2;
            }
        }
    }

    return curl;
}
std::vector<std::vector<std::vector<float>>> curl_f2(
    std::vector<std::vector<std::vector<float>>> f0, 
    std::vector<std::vector<std::vector<float>>> f1, 
    std::vector<std::vector<std::vector<float>>> f2)
{
    std::vector<std::vector<std::vector<float>>> curl = std::vector<std::vector<std::vector<float>>>(100, std::vector<std::vector<float>>(100, std::vector<float>(100)));
    for(int x = 1; x < n-1; x++)
    {
        for(int y = 1; y < n-1; y++)
        {
            for(int z = 1; z < n-1; z++)
            {
                curl[x][y][z] = (f1[x-1][y][z] - f1[x+1][y][z] - f0[x][y-1][z] + f0[x][y+1][z])/2;
            }
        }
    }

    return curl;
}
int main()
{
    //F
    std::vector<std::vector<std::vector<float>>> f0_E;
    f0_E = std::vector<std::vector<std::vector<float>>>(100, std::vector<std::vector<float>>(100, std::vector<float>(100)));
    std::vector<std::vector<std::vector<float>>> f1_E;
    f1_E = std::vector<std::vector<std::vector<float>>>(100, std::vector<std::vector<float>>(100, std::vector<float>(100)));
    std::vector<std::vector<std::vector<float>>> f2_E;
    f2_E = std::vector<std::vector<std::vector<float>>>(100, std::vector<std::vector<float>>(100, std::vector<float>(100)));

    std::vector<std::vector<std::vector<float>>> f0_H;
    f0_H = std::vector<std::vector<std::vector<float>>>(100, std::vector<std::vector<float>>(100, std::vector<float>(100)));
    std::vector<std::vector<std::vector<float>>> f1_H;
    f1_H = std::vector<std::vector<std::vector<float>>>(100, std::vector<std::vector<float>>(100, std::vector<float>(100)));
    std::vector<std::vector<std::vector<float>>> f2_H;
    f2_H = std::vector<std::vector<std::vector<float>>>(100, std::vector<std::vector<float>>(100, std::vector<float>(100)));

    //curl
    std::vector<std::vector<std::vector<float>>> f0_E_curl;
    f0_E_curl = std::vector<std::vector<std::vector<float>>>(100, std::vector<std::vector<float>>(100, std::vector<float>(100)));
    std::vector<std::vector<std::vector<float>>> f1_E_curl;
    f1_E_curl = std::vector<std::vector<std::vector<float>>>(100, std::vector<std::vector<float>>(100, std::vector<float>(100)));
    std::vector<std::vector<std::vector<float>>> f2_E_curl;
    f2_E_curl = std::vector<std::vector<std::vector<float>>>(100, std::vector<std::vector<float>>(100, std::vector<float>(100)));

    std::vector<std::vector<std::vector<float>>> f0_H_curl;
    f0_H_curl = std::vector<std::vector<std::vector<float>>>(100, std::vector<std::vector<float>>(100, std::vector<float>(100)));
    std::vector<std::vector<std::vector<float>>> f1_H_curl;
    f1_H_curl = std::vector<std::vector<std::vector<float>>>(100, std::vector<std::vector<float>>(100, std::vector<float>(100)));
    std::vector<std::vector<std::vector<float>>> f2_H_curl;
    f2_H_curl = std::vector<std::vector<std::vector<float>>>(100, std::vector<std::vector<float>>(100, std::vector<float>(100)));


    std::vector<std::vector<std::vector<float>>> k1 = std::vector<std::vector<std::vector<float>>>(100, std::vector<std::vector<float>>(100, std::vector<float>(100)));
    std::vector<std::vector<std::vector<float>>> k2 = std::vector<std::vector<std::vector<float>>>(100, std::vector<std::vector<float>>(100, std::vector<float>(100)));
    std::vector<std::vector<std::vector<float>>> k3 = std::vector<std::vector<std::vector<float>>>(100, std::vector<std::vector<float>>(100, std::vector<float>(100)));
    std::vector<std::vector<std::vector<float>>> k4 = std::vector<std::vector<std::vector<float>>>(100, std::vector<std::vector<float>>(100, std::vector<float>(100)));
    std::vector<std::vector<std::vector<float>>> k5 = std::vector<std::vector<std::vector<float>>>(100, std::vector<std::vector<float>>(100, std::vector<float>(100)));
    std::vector<std::vector<std::vector<float>>> k6 = std::vector<std::vector<std::vector<float>>>(100, std::vector<std::vector<float>>(100, std::vector<float>(100)));
    std::vector<std::vector<std::vector<float>>> k7 = std::vector<std::vector<std::vector<float>>>(100, std::vector<std::vector<float>>(100, std::vector<float>(100)));
    std::vector<std::vector<std::vector<float>>> k8 = std::vector<std::vector<std::vector<float>>>(100, std::vector<std::vector<float>>(100, std::vector<float>(100)));

    int source_position_x = floor(n / 3);
    int source_position_y = floor(n / 3);
    int source_position_z = floor(n / 2);
    int index = 0;

    std::cout.precision(4);
    
    while(true)
    {
        f0_E_curl = curl_f0(f0_E, f1_E, f2_E); 
        f1_E_curl = curl_f1(f0_E, f1_E, f2_E);
        f2_E_curl = curl_f2(f0_E, f1_E, f2_E); 
        
        /************************* Propagation *************************/
        //std::cout << index << " " << f0_E[source_position_x][source_position_y][source_position_z] << std::endl;
        std::cout << index << " > " << f0_E[source_position_x][source_position_y][source_position_z] << std::endl;

        auto future_f0_E = std::async(propagation_E, f0_H_curl, f0_E);
        auto future_f1_E = std::async(propagation_E, f1_H_curl, f1_E);
        auto future_f2_E = std::async(propagation_E, f2_H_curl, f2_E);

        f0_E = future_f0_E.get();
        f1_E = future_f1_E.get();
        f2_E = future_f2_E.get();

        //std::cout << index << " " << f0_E[source_position_x][source_position_y][source_position_z] << std::endl;

        f0_E[source_position_x][source_position_y][source_position_z] += power_source(index);
        //std::cout << index << " " << f0_E[source_position_x][source_position_y][source_position_z] << std::endl;


        f0_H_curl = curl_f0(f0_H, f1_H, f2_H);
        f1_H_curl = curl_f1(f0_H, f1_H, f2_H);
        f2_H_curl = curl_f2(f0_H, f1_H, f2_H);
        
        /************************* Propagation *************************/

        auto future_f0_H = std::async(propagation_H, f0_E_curl, f0_H);
        auto future_f1_H = std::async(propagation_H, f1_E_curl, f1_H);
        auto future_f2_H = std::async(propagation_H, f2_E_curl, f2_H);

        f0_H = future_f0_H.get();
        f1_H = future_f1_H.get();
        f2_H = future_f2_H.get();
        
        //increase in time
        index++;
        
        f0_E_curl = std::vector<std::vector<std::vector<float>>>(100, std::vector<std::vector<float>>(100, std::vector<float>(100)));
        f1_E_curl = std::vector<std::vector<std::vector<float>>>(100, std::vector<std::vector<float>>(100, std::vector<float>(100)));
        f2_E_curl = std::vector<std::vector<std::vector<float>>>(100, std::vector<std::vector<float>>(100, std::vector<float>(100)));
        
        f0_H_curl = std::vector<std::vector<std::vector<float>>>(100, std::vector<std::vector<float>>(100, std::vector<float>(100)));
        f1_H_curl = std::vector<std::vector<std::vector<float>>>(100, std::vector<std::vector<float>>(100, std::vector<float>(100)));
        f2_H_curl = std::vector<std::vector<std::vector<float>>>(100, std::vector<std::vector<float>>(100, std::vector<float>(100)));

        //std::cout << index << " " << f0_E[30][30][30] << std::endl;
        //std::cout << index << " " << f0_E_curl[source_position_x][source_position_y][source_position_z] << std::endl;
    }
    
    return 0;
}