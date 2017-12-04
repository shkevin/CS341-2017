/* 
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */ 
#include <stdio.h>
#include "cachelab.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
    char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
   // REQUIRES(M > 0);
   // REQUIRES(N > 0);
   int block_r, block_c, r, c;
   /* Ordinary */
   if (M == 32)
   {
       for (block_r = 0; block_r < 4; block_r++)
           for (block_c = 0; block_c < 4; block_c++)
               for (r = block_r * 8; r < block_r * 8 + 8; r++)
               {
                   /* diagonal references produce conflicts, so solve it first */
                   if (block_r == block_c)
                   {
                       B[r][r] = A[r][r];
                   }
                   for (c = block_c * 8; c < block_c * 8 + 8; c++)
                   {
                       /* A[r] will be corrupted by B[r], so in this case, just don't access A[r]
                          (or B[r] will be corrupted again, causing double evictions) */
                       if (r != c)
                       {
                           B[r][c] = A[c][r];
                       }
                   }
               }
   }
   // ENSURES(is_transpose(M, N, A, B));
   if (M == 61)
   {
       for (block_r = 0; block_r < 8; block_r++)
           for (block_c = 0; block_c < 9; block_c++)
               /* be careful with loop order. 72-67=5, 64-61=3.
                  so we choose to loop column of B first, thus saving more overbound accesses */
               for (c = block_c * 8; c < block_c * 8 + 8 && c < 67; c++)
                   for (r = block_r * 8; r < block_r * 8 + 8 && r < 61; r++)
                       B[r][c] = A[c][r];
   }

   if (M == 64)
   /* we can no longer just block into 8*8 blocks as it will cause conflict even in the same matrix */
   /*  A     B
     |a|b| |a|b|
     |c|d| |c|d|
    */
   {
       int r0, r1, r2, r3;
       for (block_r = 0; block_r < 8; block_r++)
           for (block_c = 0; block_c < 8; block_c++)
           {
               /* Part A: usual way */
               for (c = block_c * 8; c < block_c * 8 + 4; c++)
               {
                   for (r = block_r * 8; r < block_r * 8 + 4; r++)
                   {
                       if (r != c)
                       {
                           B[r][c] = A[c][r];
                       }
                   }
                   if (block_r == block_c)
                   {
                       B[c][c] = A[c][c];
                   }
               }
               /* Part B: used to store what should be in C */
               for (c = block_c * 8 + 4; c < block_c * 8 + 8; c++)
               {
                   for (r = block_r * 8; r < block_r * 8 + 4; r++)
                   {
                       if (r != c - 4)
                       {
                           B[r][c] = A[c - 4][r + 4];
                       }
                   }
                   if (block_c == block_r)
                   {
                       B[c - 4][c] = A[c - 4][c];
                   }
               }
               /* Part B & C: transport B into registers,
                              then fill B using values in original matrix,
                              finally fill C with registers */
               /* put 2 rows of B into register */
               r = block_r * 8;
               c = block_c * 8 + 4;
               r0 = B[r][c];
               r1 = B[r][c + 1];
               r2 = B[r][c + 2];
               r3 = B[r][c + 3];
               for (c = block_c * 8 + 4; c < block_c * 8 + 8; c++)
               {
                   B[r][c] = A[c][r];
               }
               r = block_r * 8 + 4;
               c = block_c * 8;
               B[r][c] = r0;
               B[r][c + 1] = r1;
               B[r][c + 2] = r2;
               B[r][c + 3] = r3;

               r = block_r * 8 + 1;
               c = block_c * 8 + 4;
               r0 = B[r][c];
               r1 = B[r][c + 1];
               r2 = B[r][c + 2];
               r3 = B[r][c + 3];
               for (c = block_c * 8 + 4; c < block_c * 8 + 8; c++)
               {
                   B[r][c] = A[c][r];
               }
               r = block_r * 8 + 5;
               c = block_c * 8;
               B[r][c] = r0;
               B[r][c + 1] = r1;
               B[r][c + 2] = r2;
               B[r][c + 3] = r3;

               r = block_r * 8 + 2;
               c = block_c * 8 + 4;
               r0 = B[r][c];
               r1 = B[r][c + 1];
               r2 = B[r][c + 2];
               r3 = B[r][c + 3];
               for (c = block_c * 8 + 4; c < block_c * 8 + 8; c++)
               {
                   B[r][c] = A[c][r];
               }
               r = block_r * 8 + 6;
               c = block_c * 8;
               B[r][c] = r0;
               B[r][c + 1] = r1;
               B[r][c + 2] = r2;
               B[r][c + 3] = r3;

               r = block_r * 8 + 3;
               c = block_c * 8 + 4;
               r0 = B[r][c];
               r1 = B[r][c + 1];
               r2 = B[r][c + 2];
               r3 = B[r][c + 3];
               for (c = block_c * 8 + 4; c < block_c * 8 + 8; c++)
               {
                   B[r][c] = A[c][r];
               }
               r = block_r * 8 + 7;
               c = block_c * 8;
               B[r][c] = r0;
               B[r][c + 1] = r1;
               B[r][c + 2] = r2;
               B[r][c + 3] = r3;



               /* Part D: usual way */
               for (r = block_r * 8 + 4; r < block_r * 8 + 8; r++)
               {
                   if (block_c == block_r)
                   {
                       B[r][r] = A[r][r];
                   }
                   for (c = block_c * 8 + 4; c < block_c * 8 + 8; c++)
                   {
                       if (r != c)
                       {
                           B[r][c] = A[c][r];
                       }
                   }
               }
           }
   }   
}

/* 
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started. 
 */ 

/* 
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tempStorage;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tempStorage = A[i][j];
            B[j][i] = tempStorage;
        }
    }    

}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc); 

    /* Register any additional transpose functions */
    registerTransFunction(trans, trans_desc); 

}

/* 
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}

