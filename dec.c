#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "grpwk20.h"

int retBan(int *prevDNA, char now)
{
	int res = 0;
	if (now == 'A')
	{
		if (*prevDNA == 1)
		{
			res = 2;
		}
		else if (*prevDNA == 2)
		{
			res = 1;
		}
		else if (*prevDNA == 3)
		{
			res = 0;
		}
		*prevDNA = 0;
	}
	if (now == 'C')
	{
		if (*prevDNA == 0)
		{
			res = 0;
		}
		else if (*prevDNA == 2)
		{
			res = 2;
		}
		else if (*prevDNA == 3)
		{
			res = 1;
		}
		*prevDNA = 1;
	}
	if (now == 'G')
	{
		if (*prevDNA == 0)
		{
			res = 1;
		}
		else if (*prevDNA == 1)
		{
			res = 0;
		}
		else if (*prevDNA == 3)
		{
			res = 2;
		}
		*prevDNA = 2;
	}
	if (now == 'T')
	{
		if (*prevDNA == 0)
		{
			res = 2;
		}
		else if (*prevDNA == 1)
		{
			res = 1;
		}
		else if (*prevDNA == 2)
		{
			res = 0;
		}
		*prevDNA = 3;
	}
	// printf("%d\n", res);
	return res;
}

int retPrev(char c)
{
	switch (c)
	{
		case 'A':
			return 0;
		case 'C':
			return 1;
		case 'G':
			return 2;
		case 'T':
			return 3;
	}
	return 0;
}

int dec()
{
	FILE *sfp;
	if((sfp = fopen(SEQDATA, "r")) ==NULL)
	{
		fprintf(stderr, "cannot open %s\n", SEQDATA);
		exit(1);
	}

	FILE *dfp;
	if((dfp = fopen(DECDATA, "w")) ==NULL)
	{
		fprintf(stderr, "cannot open %s\n", DECDATA);
		exit(1);
	}

	unsigned char c, res;
	int prevDNA = 0;
	char nextDNA[4][3] =
	{
		{'C','G','T'},
		{'G','T','A'},
		{'T','A','C'},
		{'A','C','G'},
	};
	int nowPos = 1;
	int now = 0;
	for(int i = 0; (c = getc(sfp)) != '\n'; ++i)
	{
		now += retBan(&prevDNA, c) * pow(3,5-(i%6));	//DNAを10進数に
		if (i % 5 == 0 && i != 0)
		{
			printf("now %d\n", now);
			// printf("%d\n", now);
			int nowMod;
			int ni[8] = {};
			for (int j = 8; j >= 0; --j)	//10進数から2進数に
			{
				nowMod = now % 2;
				ni[j] = nowMod;
				now /= 2;
			}
			for (int j = 0; j < 8; ++j)	//2進数をDNAに
			{
				// printf("%d", ni[j]);
			}
			// printf("\n");
			for (int j = 0; j < 8; j += 2)	//2進数をDNAに
			{
				res = 2 * ni[j] + ni[j+1];
				// printf("%d\n", res);
				fputc((res>>1)+'0', dfp);
				fputc((res&0x1)+'0', dfp);
			}
		}

		// fputc((res>>1)+'0', dfp);
		// fputc((res&0x1)+'0', dfp);
	}
	res = '\n';
	fputc(res, dfp);

	fclose(sfp);
	fclose(dfp);
	return(0);
}

int main()
{
	dec();
	return(0);
}
