#include <stdio.h>
#include <stdlib.h>
#include "grpwk20.h"

// 畳み込み符号化器の状態遷移関数(拘束長 = 3)
// int input   : 入力信号
// int c_state : 現状態
// int *result : 結果を格納する配列 
	// result[0] : 状態遷移関数の出力
	// result[1] : 状態遷移関数の次状態
void* convolution_state_machine(int input, int c_state, int *result)
{

	switch(c_state)
	{
		// 出力(result[0])と次状態(result[1])の格納
		// 三項演算子使い入力が0or1で結果を格納
		case 0b00:
			result[0] = (input == 0) ? 0b00: 0b11;
			result[1] = (input == 0) ? 0b00: 0b01;
		break;

		case 0b01:
			result[0] = (input == 0) ? 0b01: 0b10;
			result[1] = (input == 0) ? 0b10: 0b11;
		break;

		case 0b10:
			result[0] = (input == 0) ? 0b11: 0b00;
			result[1] = (input == 0) ? 0b00: 0b01;
		break;

		case 0b11:
			result[0] = (input == 0) ? 0b10: 0b01;
			result[1] = (input == 0) ? 0b10: 0b11;
		break;

		default:
			printf("%d, %d , Error!\n", input, c_state);
		break;
	}
}

// 0 1 2 3
// A C G T

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

	int result_in[2];
	int nowState = 0b00;
	char inputDNA;
	int inputData;
	// for(int i=0; i<ORGDATA_LEN; i++)
	for(int i=0; i<10; i++)
	{
		inputDNA = getc(ofp);
		inputData = inputDNA - '0';
		// printf("%d\n", inputData);

		convolution_state_machine(inputData, nowState, result_in);
		nowState = result_in[1];
		printf("%d", (result_in[0]&0b10)>>1);
		printf("%d\n", result_in[0]&0b01);

		// ファイルへの書き込み
		// fputc([charの変数], efp);
	}
	inputDNA = '\n';
	fputc(inputDNA, efp);


	fclose(ofp);
	fclose(efp);
	return(0);
}

int main()
{
	enc();
	return(0);
}
