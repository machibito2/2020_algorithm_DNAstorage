#include <stdio.h>
#include <stdlib.h>
#include "grpwk20.h"

int enc()
{
	FILE *ofp;
	if((ofp = fopen(ORGDATA, "r")) ==NULL)
	{
		fprintf(stderr, "cannot open %s\n", ORGDATA);
		exit(1);
	}

	FILE *efp;
	if((efp = fopen(ENCDATA, "w")) ==NULL)
	{
		fprintf(stderr, "cannot open %s\n", ENCDATA);
		exit(1);
	}

	unsigned char c1, c2, res;
	int now, check;
	int prevDNA;
	char next[6];
	char nextDNA[4][3] =
	{
		{'C','G','T'},
		{'G','T','A'},
		{'T','A','C'},
		{'A','C','G'},
	};
	prevDNA = 0;
	for(int i=0; i<ORGDATA_LEN; i+=8)
	{
		now = 0;
		for (int j = 7; j >= 0; --j)	//2進数から10進数に
		{
			c1 = getc(ofp);
			// printf("%c", c1);
			now += (1<<j) * (c1 == '1');
		}
		check = now;
		// printf("%d\n", check);
		// prevDNA = 0;
		int san[6];
		for (int j = 5; j >= 0; --j)	//10進数から3進数に
		{
			int nowMod = now % 3;
			san[j] = nowMod;
			// printf("%d", nowMod);
			now /= 3;
		}
		// printf("\n");
		for (int j = 0; j < 6; ++j)	//3進数をDNAに
		{
			next[j] = nextDNA[prevDNA][san[j]];
			switch (next[j])
			{
				case 'A':
					prevDNA = 0;
					break;
				case 'C':
					prevDNA = 1;
					break;
				case 'G':
					prevDNA = 2;
					break;
				case 'T':
					prevDNA = 3;
					break;
			}
		}
		for (int j = 0; j < 6; ++j)
		{
			fputc(next[j], efp);
		}
		// fputc(nextDNA[prevDNA][0], efp);
		// res = now;
		// printf(" %d %s\n", check, next);
		// fputc(res, efp);
	}
	res = '\n';
	fputc(res, efp);


	fclose(ofp);
	fclose(efp);
	return(0);
}

int main()
{
	enc();
	return(0);
}
