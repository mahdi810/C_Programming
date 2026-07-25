#include <stdio.h>
#include <math.h>
#include <stdbool.h>

// implementing the sieve algorithm
#define max_no 1000000

int main()
{
    printf("sieve algorithm\n");
    unsigned int max = max_no + 1;
    bool is_prime[max_no + 1];
    for (int i = 0; i < max_no + 1; i++)
    {
        is_prime[i] = true;
    }

    is_prime[0] = is_prime[1] = false;

    unsigned int sieve_max = round(sqrt(max));

    printf("max: %d, sieve_max: %d\n", max, sieve_max);

    for (int i = 2; i < sieve_max; i++)
    {
        for (int j = 0; j < max; j++)
        {
            if (is_prime[i])
            {
                for (int k = i; k < max; k += i)
                {
                    is_prime[k] = false;
                }
            }
        }
    }

    // printing all the numbers
    printf("%-8s %-20s \n", "SN.", "Prime Number");
    printf("--------------------------------------\n");

    int count = 1;
    for (int i = 0; i < max; i++)
    {
        if (is_prime[i] == true)
        {
            printf("%d \t %d \n", count++, i);
        }
    }

    return 0;
}