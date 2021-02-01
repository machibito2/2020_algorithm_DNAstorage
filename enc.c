#include <stdio.h>
#include <stdlib.h>
#include "grpwk20.h"
#define Rep 5

unsigned char bef = 'A';

void transdna(int n);

int enc(){
  FILE *ofp;
  if((ofp = fopen(ORGDATA, "r")) ==NULL){
    fprintf(stderr, "cannot open %s\n", ORGDATA);
    exit(1);
  }

  FILE *efp;
  if((efp = fopen(ENCDATA, "w")) ==NULL){
    fprintf(stderr, "cannot open %s\n", ENCDATA);
    exit(1);
  }

  unsigned char c1, c2, res;
  int i, n;
  for(i=0; i<ORGDATA_LEN; i++){
    c1 = getc(ofp);
    switch( ( (c1 & 0x1) << 7) >> 7){
    case 0:
      c2 = getc(ofp);
      switch(c2 & 0x1){
        case 0:
        n = 0;
        break;
        case 1:
        n = 1;
        break;
        default:
        break;
      }
      i++;
      break;
    case 1:
      n = 2;
      break;
    default:
      break;
    }
    transdna(n);
    for(int count=0; count < Rep; count++){
      fputc(bef, efp);
    }
  }
  res = '\n';
  fputc(res, efp);

  fclose(ofp);
  fclose(efp);
  return(0);
}
/*
 \ | 0 | 1 | 2 |
 A | C | G | T |
 C | G | T | A |
 G | T | A | C |
 T | A | C | G |
*/
void transdna(int n){
  switch(bef){
    case 'A':
      switch(n){
        case 0:
        bef = 'C';
        break;
        case 1:
        bef = 'G';
        break;
        case 2:
        bef = 'T';
        break;
      }
    break;
    case 'C':
      switch(n){
        case 0:
        bef = 'G';
        break;
        case 1:
        bef = 'T';
        break;
        case 2:
        bef = 'A';
        break;
      }
    break;
    case 'G':
      switch(n){
        case 0:
        bef = 'T';
        break;
        case 1:
        bef = 'A';
        break;
        case 2:
        bef = 'C';
        break;
      }
    break;
    case 'T':
      switch(n){
        case 0:
        bef = 'A';
        break;
        case 1:
        bef = 'C';
        break;
        case 2:
        bef = 'G';
        break;
      }
    break;
  }
}

int main(){
  enc();
  return(0);
}
