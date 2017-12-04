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
void transposeM32(int M, int N, int A[N][M], int B[M][N]);

/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 */
/* ************************************************
 * PARAMETERS: M row size, N col size, matrix to
               transpose from, matrix to transpose
               to.
 * FUNCTION: Transpose the given matrices while
             trying to minimize accessing and misses
             and try to maximize the hits. This 
             function takes advantage of cache 
             blocking in order to do this.
 * Usage: ./test-trans -M 64 -N 64, can change M,N
 * RETURNS: None.
 ************************************************* */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
   /*
    We need 4 for loops in order to iterate the blocks, and to perform
    the transpose copy. This standard will be used in order to block the 
    matrix and try to minimize misses.
   */
   int blockRow, blockCol, row, col;
   //this implies we can have an 8x8 block to iterate over
   if (M == 32)
   {
       transposeM32(M, N, A, B);
   }
   //Need to be able to handle the odd M = 61 case.
   if (M == 61)
   {
       for (blockRow = 0; blockRow < 8; blockRow++)
       {
           for (blockCol = 0; blockCol < 9; blockCol++)
           {
               for (col = blockCol * 8; col < blockCol * 8 + 8 && col < 67; col++)
               {
                   for (row = blockRow * 8; row < blockRow * 8 + 8 && row < 61; row++)
                   {
                       B[row][col] = A[col][row];
                   }
               }
           }
       }
   }

   if (M == 64)
   {
       int row1, row2, row3, row4;
       for (blockRow = 0; blockRow < 8; blockRow++)
       {
           for (blockCol = 0; blockCol < 8; blockCol++)
           {
               for (col = blockCol * 8; col < blockCol * 8 + 4; col++)
               {
                   for (row = blockRow * 8; row < blockRow * 8 + 4; row++)
                   {
                       //for non diagonal
                       if (row != col)
                       {
                           B[row][col] = A[col][row];
                       }
                   }
                   if (blockRow == blockCol)
                   {
                       B[col][col] = A[col][col];
                   }
               }
               for (col = blockCol * 8 + 4; col < blockCol * 8 + 8; col++)
               {
                   for (row = blockRow * 8; row < blockRow * 8 + 4; row++)
                   {
                       if (row != col - 4)
                       {
                           B[row][col] = A[col - 4][row + 4];
                       }
                   }
                   if (blockCol == blockRow)
                   {
                       B[col - 4][col] = A[col - 4][col];
                   }
               }

               //Store 2 rows of B in order to save accesses
               row = blockRow * 8;
               col = blockCol * 8 + 4;
               row1 = B[row][col];
               row2 = B[row][col + 1];
               row3 = B[row][col + 2];
               row4 = B[row][col + 3];
               for (col = blockCol * 8 + 4; col < blockCol * 8 + 8; col++)
               {
                   B[row][col] = A[col][row];
               }
               row = blockRow * 8 + 4;
               col = blockCol * 8;
               B[row][col] = row1;
               B[row][col + 1] = row2;
               B[row][col + 2] = row3;
               B[row][col + 3] = row4;

               row = blockRow * 8 + 1;
               col = blockCol * 8 + 4;
               row1 = B[row][col];
               row2 = B[row][col + 1];
               row3 = B[row][col + 2];
               row4 = B[row][col + 3];
               for (col = blockCol * 8 + 4; col < blockCol * 8 + 8; col++)
               {
                   B[row][col] = A[col][row];
               }
               row = blockRow * 8 + 5;
               col = blockCol * 8;
               B[row][col] = row1;
               B[row][col + 1] = row2;
               B[row][col + 2] = row3;
               B[row][col + 3] = row4;

               row = blockRow * 8 + 2;
               col = blockCol * 8 + 4;
               row1 = B[row][col];
               row2 = B[row][col + 1];
               row3 = B[row][col + 2];
               row4 = B[row][col + 3];
               for (col = blockCol * 8 + 4; col < blockCol * 8 + 8; col++)
               {
                   B[row][col] = A[col][row];
               }
               row = blockRow * 8 + 6;
               col = blockCol * 8;
               B[row][col] = row1;
               B[row][col + 1] = row2;
               B[row][col + 2] = row3;
               B[row][col + 3] = row4;

               row = blockRow * 8 + 3;
               col = blockCol * 8 + 4;
               row1 = B[row][col];
               row2 = B[row][col + 1];
               row3 = B[row][col + 2];
               row4 = B[row][col + 3];
               for (col = blockCol * 8 + 4; col < blockCol * 8 + 8; col++)
               {
                   B[row][col] = A[col][row];
               }
               row = blockRow * 8 + 7;
               col = blockCol * 8;
               B[row][col] = row1;
               B[row][col + 1] = row2;
               B[row][col + 2] = row3;
               B[row][col + 3] = row4;

               for (row = blockRow * 8 + 4; row < blockRow * 8 + 8; row++)
               {
                   if (blockCol == blockRow)
                   {
                       B[row][row] = A[row][row];
                   }
                   for (col = blockCol * 8 + 4; col < blockCol * 8 + 8; col++)
                   {
                       if (row != col)
                       {
                           B[row][col] = A[col][row];
                       }
                   }
               }
           }
       }
   }
}


void transposeM32(int M, int N, int A[N][M], int B[M][N])
{
    int blockRow, blockCol, row, col;

    for (blockRow = 0; blockRow < 4; blockRow++)
    {
        for (blockCol = 0; blockCol < 4; blockCol++)
        {
               //to iterate over the blocks
           for (row = blockRow * 8; row < blockRow * 8 + 8; row++)
           {
                   //Handle diagonal
               if (blockRow == blockCol)
               {
                   B[row][row] = A[row][row];
               }
               for (col = blockCol * 8; col < blockCol * 8 + 8; col++)
               {
                    //transpose non diagonal
                   if (row != col)
                   {
                       B[row][col] = A[col][row];
                   }
               }
           }
       }
   }


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

