#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

/*
This is a small c program that checks if a randomly generated number is
a palindrome. 
*/

/* Returns an integer in the range [0, n).
 *
 * Uses rand(), and so is affected-by/affects the same seed.
 * source: http://stackoverflow.com/questions/822323/how-to-generate-a-random-number-in-c
 */
int randint(int n) {
  if ((n - 1) == RAND_MAX) {
    return rand();
  } else {
    // Chop off all of the values that would cause skew...
    long end = RAND_MAX / n; // truncate skew
    assert (end > 0L);
    end *= n;

    // ... and ignore results from rand() that fall above that limit.
    // (Worst case the loop condition should succeed 50% of the time,
    // so we can expect to bail out of this loop pretty quickly.)
    int r;
    while ((r = rand()) >= end);

    return r % n;
  }
}

int main()
{
    int num_of_iterations = 100000;
    int i;
    for(i =0 ; i<num_of_iterations; i++){
        int n, reversedInteger = 0, remainder, originalInteger;
        n = randint(1000000);
        originalInteger = n;

        // reversed integer is stored in variable 
        while( n!=0 )
        {
            remainder = n%10;
            reversedInteger = reversedInteger*10 + remainder;
            n /= 10;
        }

        // palindrome if orignalInteger and reversedInteger are equal
        if (originalInteger == reversedInteger)
            printf("%d is a palindrome.\n", originalInteger);
        else
            printf("%d is not a palindrome.\n", originalInteger);
    }   
    
    return 0;
}
