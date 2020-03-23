
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
const int min_bases = 3;




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

static inline bool can_form_pair(const int left, const int right,\
                                 const int *nseq){
  return right - left > min_bases && bp_check(nseq[left], nseq[right]);
}



size_t nu(const int * nseq, const size_t len, pair_int *base_pairs){
  /* printf("%d",len); */
  // make the matrix
  int size = 0;
  int *ptrN, **N, *ptrL, **L;

  size = sizeof(int *) * len + sizeof(int) * len * len;
  N = (int **)malloc(size);
  ptrN = (int *)(N + len);
  for(int i = 0; i < len; i++)
    N[i] = (ptrN + len * i);

  L = (int **)malloc(size);
  ptrL = (int *)(L + len);
  for(int i = 0; i < len; i++)
    L[i] = (ptrL + len * i);
  /* int N[len][len]; */
  /* int L[len][len]; */
  int new_N = 0;
  int max_N = 0;

  for (int i = 0; i < len; i++){
    N[i][i] = 0;
    L[i][i] = -1;
  }

  for (int j = 1; j < len; j++){
    for (int i = j-1; i >= 0; i--){
      max_N = N[i][j-1];
      L[i][j] = -1;
      for (int l = i; l < j - 1; l++){
        if (j - l > min_bases && bp_check(nseq[l],nseq[j])){
          new_N = 1;
          if (l > i) new_N += N[i][l-1];
          if (j > l + 1) new_N += N[l+1][j-1];
          if (max_N < new_N){
            max_N = new_N;
            L[i][j] = l;
          }
        }
      }
      N[i][j] = max_N;
    }
  }
  /* printf ("%d\n",N[0][len-1]); */
  // backtrack

  pair_int pairs[len*len];
  pair_int p0;
  p0.first = 0;
  p0.second = len-1;
  pairs[0] = p0;
  int pos = 0;
  int i,l,j;
  pair_int p,p_E;
  pair_int E[len/2];
  size_t pos_E = 0;
  while (pos != -1){
    i = pairs[pos].first;
    j = pairs[pos].second;
    l = L[i][j];
    /* printf("L[%d][%d] = %d\n",i,j,l); */
    /* return; */
    pos -= 1;
    if (l == -1) {
      if (i < j){
        p.first = i;
        p.second = j-1;
        pairs[++pos] = p;
      }
    }
    else{
      p_E.first = l;
      p_E.second = j;
      base_pairs[pos_E++] = p_E;
      if (i < l) {
        p.first = i;
        p.second = l-1;
        pairs[++pos] = p;
      }
      if (j-1 > l) {
        p.first = l+1;
        p.second = j-1;
        pairs[++pos] = p;
      }
    }

  }
  free(N);
  free(L);
  /* for (int i = 0; i < pos_E; i++){ */
  /*   printf ("(%d, %d)\n", E[i].first,E[i].second); */
  /* } */

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
  char * seq = NULL;
  size_t len = 0;
  ssize_t read;
  fp = fopen(path, "r");
  if (fp == NULL)
    {
      perror("Error while opening the file.\n");
      exit(EXIT_FAILURE);
    }

  read = getline (&seq, &len, fp);
  fclose(fp);
  /* printf(seq); */


  /* printf("%d\n", read); */

  int nseq[read-1]; // assumes the ends with \n
  for (size_t i = 0; i < read-1; i++){
    /* printf("%c\n",seq[i]); */
    nseq[i] = b2n(toupper(seq[i]));     /*  */
  }
  pair_int base_pairs[read/2];
  size_t num_pairs = nu(nseq, read-1, base_pairs);


  fp = fopen(outpath, "w");
  fprintf(fp, "%d\n",num_pairs);
  for (int i = 0; i < num_pairs; i++){
    fprintf (fp, "%d %d\n", base_pairs[i].first,base_pairs[i].second);
  }
  fclose(fp);


  return 0;
}
