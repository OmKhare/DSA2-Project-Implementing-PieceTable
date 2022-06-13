#include "PieceTable.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

int main()
{
 L = InitLogger();

 printf("offset\tlength\tstart\n");

 struct Tree *profile = NULL;
 clock_t start, end;
 double total = 0, diff;

 unsigned long int target;

 srand(time(NULL));

 unsigned long test1[] = {0, 1, 2, 3, 0, 1, 2, 3, 1, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
 unsigned long current_max = 0;

#define RANGE 1000000
 // double data[RANGE];
 // double x[RANGE];
 FILE *fp = fopen("linear_avg.txt", "w");
 FILE *fp2 = fopen("linear_per_insert.txt", "w");
 for (unsigned long int i = 0; i < RANGE; i++)
 {

  target = (unsigned long)(rand() % (current_max - 0 + 1));

  // printf("insert @ %ld\n", target);

  start = clock();
  profile = Insert(profile, MakePiece("T", 1), target);
  end = clock();

  current_max = target + 1 > current_max ? target + 1 : current_max;
  diff = ((double)(end - start)) / CLOCKS_PER_SEC;
  total += diff;
  if(i%100==0) {
   print_to_file((double)(i), total, fp);
   print_to_file((double)(i), diff, fp2);
  }
  
 }
 // plot_array(x, data, RANGE);
 printf("%f\n", total / RANGE);
 FreeTree(profile);
 profile = NULL;
 fclose(fp);
 return 0;
}