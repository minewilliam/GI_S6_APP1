import numpy as np

def source(index):
    return 0.1 * np.sin(0.1 * index)

if __name__ == "__main__":
    index = 0
    puissance = 0

    while True:
        puissance += source(index)
        puissance = 0.1 * puissance
        print(puissance)
        index+=1