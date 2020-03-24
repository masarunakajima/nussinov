
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#define MIN_HAIRPIN_SIZE 3




typedef enum {false,true} bool;


typedef struct {
  int first;
  int second;
} pair_int;

struct nu_output{
  pair_int * E;
  int size;
} ;


static inline int b2n(const char b){
  if (b == 'A') return 0;
  else if (b == 'G') return 1;
  else if (b == 'C') return 2;
  else if (b == 'T') return 3;
  else if (b == 'U') return 3;
  else if (b == 'D') return 6;
  else{
    printf("Invalid base \"%c\".\n", b);
    return -1;
  }
  return -1;
}



static inline bool bp_check(const int a, const int b){
  if (a+b == 3) return true; //W-C pair
  /* else if (a==2 && b ==2) return false; */
  /* else if (a+b == 4) return true; // wobble pair G-U */
  else return false;
}

static inline bool steric_check(const int left, const int right){
  return right - left > MIN_HAIRPIN_SIZE;
}

static inline bool can_form_pair(const int left, const int right,\
                                 const int *nseq){
  return right - left > MIN_HAIRPIN_SIZE && bp_check(nseq[left], nseq[right]);
}


bool zero_size_hairpin_allowed = MIN_HAIRPIN_SIZE < 1;

size_t nu(const int * nseq, const size_t len, pair_int *base_pairs){
  int *ptrN, **N, *ptrL, **L;
  int matrix_size = len + 2;
  int size = sizeof(int *) * matrix_size + sizeof(int) * matrix_size * matrix_size;
  // Initialize 2D matrices N and L
  N = (int **)malloc(size);
  ptrN = (int *)(N + matrix_size);
  for(int i = 0; i < matrix_size; i++)
    N[i] = (ptrN + matrix_size * i);

  L = (int **)malloc(size);
  ptrL = (int *)(L + matrix_size);
  for(int i = 0; i < matrix_size; i++)
    L[i] = (ptrL + matrix_size * i);

  for (int i = 1; i < matrix_size - 1; i++){
    N[i][i-1] = 0;
    N[i][i] = 0;
    L[i][i] = -1;
  }
  N[matrix_size-1][matrix_size-2] = 0;
  int new_N = 0;
  int max_N = 0;

  for (int j = 2; j < matrix_size - 1; j++){
    for (int i = j-1; i >= 1; i--){
      max_N = N[i][j-1];  // base j is not paired
      L[i][j] = -1;
      for (int l = i; l < j - MIN_HAIRPIN_SIZE; l++){
        if (bp_check(nseq[l-1], nseq[j-1])){
          new_N = 1 + N[i][l-1] + N[l+1][j-1];
          if (max_N < new_N){
            max_N = new_N;
            L[i][j] = l;
          }
        }
      }

      N[i][j] = max_N;
    }
  }

  pair_int *pairs = (pair_int *)malloc(len/2 * sizeof(pair_int));
  pair_int *pair = pairs;
  *pair = (pair_int){1, len};
  int i,l,j;
  size_t pos_E = 0;
  while (pair - pairs != -1){
    i = pair->first;
    j = pair->second;
    pair--;
    l = L[i][j];
    if (l == -1) {
      if (i < j) *(++pair) = (pair_int){i, j - 1};
    }
    else{
      base_pairs[pos_E++] = (pair_int){l - 1, j - 1};
      if (i < l) *(++pair) = (pair_int){i, l - 1};
      if (j - l > 2) *(++pair) = (pair_int){l + 1, j - 1};
    }
  }
  free(pairs);
  free(N);
  free(L);

  return pos_E;

}

int main(int argc, char *argv[]) {
  if( argc == 3 ) {
    ;
  }
  else if( argc > 2 ) {
    printf("Please provide input and output files.\n");
  }
  else {
    printf("Two argument expected.\n");
  }
  char *path = argv[1];
  char *outpath = argv[2];
  FILE *fp;
  char *seq;
  size_t len = 0;
  ssize_t seq_len;
  fp = fopen(path, "r");
  if (fp == NULL)
    {
      perror("Error while opening the file.\n");
      exit(EXIT_FAILURE);
    }

  seq_len = getline (&seq, &len, fp);
  if (seq[seq_len - 1] == '\n') seq_len--;
  fclose(fp);
  int *nseq = (int *)malloc(sizeof(int) * (seq_len));
  for (size_t i = 0; i < seq_len; i++){
    nseq[i] = b2n(toupper(seq[i]));
  }
  pair_int *base_pairs = (pair_int *)malloc(sizeof(pair_int) * (seq_len) / 2);
  size_t num_pairs = nu(nseq, seq_len, base_pairs);  // Run Nussinov's algorithm.
  free(nseq);

  fp = fopen(outpath, "w");
  fprintf(fp, "%d\n",num_pairs);
  for (int i = 0; i < num_pairs; i++){
    fprintf (fp, "%d %d\n", base_pairs[i].first,base_pairs[i].second);
  }
  fclose(fp);
  free(base_pairs);

  return 0;
}
