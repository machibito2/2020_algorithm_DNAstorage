#include <stdio.h>
#include <stdlib.h>
#include "grpwk20.h"
unsigned char bef = 'A';

int transbin(char n);

int dec(){
  FILE *sfp;
  if((sfp = fopen(SEQDATA, "r")) ==NULL){
    fprintf(stderr, "cannot open %s\n", SEQDATA);
    exit(1);
  }

  FILE *dfp;
  if((dfp = fopen(DECDATA, "w")) ==NULL){
    fprintf(stderr, "cannot open %s\n", DECDATA);
    exit(1);
  }

  unsigned char b = 'A', c, no, res;
  int n = 0, i;
  unsigned char seqdna[200000];
  while((c = getc(sfp)) != '\n'){
    if(b != c){
      seqdna[n] = b;
      b = c;
      n++;
    }
  }
  seqdna[n] = b;
  n++;

  int mojisu = 0;
  for(i=1; i < n; i++){
    no = seqdna[i];
    switch(transbin(no)){
      case 0:
	      fputc('0', dfp);
	      if (mojisu < 200000-1){
		      fputc('0', dfp);
	      }
	      mojisu += 2;
	      break;
      case 1:
	      fputc('0', dfp);
	      if (mojisu < 200000-1){
		      fputc('1', dfp);
	      }
	      mojisu += 2;
	      break;
      case 2:
	      fputc('1', dfp);
	      mojisu += 1;
	      break;
      default:
	      break;
    }
  }
  res = '\n';
  fputc(res, dfp);
    
  fclose(sfp);
  fclose(dfp);
  return(0);
}

int main(){
  dec();
  return(0);
}
/*
\\\| 0 | 1 | 2 |
 A | C | G | T |
 C | G | T | A |
 G | T | A | C |
 T | A | C | G |
*/
int transbin(char n){
  switch(bef){
    case 'A':
      switch(n){
        case 'C':
        bef = 'C';
        return 0;
        case 'G':
        bef = 'G';
        return 1;
        case 'T':
        bef = 'T';
        return 2;
        default:
        return 0;
      }
    case 'C':
      switch(n){
        case 'G':
        bef = 'G';
        return 0;
        case 'T':
        bef = 'T';
        return 1;
        case 'A':
        bef = 'A';
        return 2;
        default:
        return 0;
      }
    case 'G':
      switch(n){
        case 'T':
        bef = 'T';
        return 0;
        case 'A':
        bef = 'A';
        return 1;
        case 'C':
        bef = 'C';
        return 2;
        default:
        return 0;
      }
    case 'T':
      switch(n){
        case 'A':
        bef = 'A';
        return 0;
        case 'C':
        bef = 'C';
        return 1;
        case 'G':
        bef = 'G';
        return 2;
        default:
        return 0;
      }
  }
  return 0;
}
