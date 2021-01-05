#include <stdio.h>
#include <stdlib.h>
#include "grpwk20.h"

const int ADDRESS = 19;
const int CONTENT = 25 - ADDRESS;

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



char retDNA(int stateOutput)
{
	char retValue;
	switch (stateOutput)
	{
		case 0b00:
			retValue = 'A';
			break;
		case 0b01:
			retValue = 'C';
			break;
		case 0b10:
			retValue = 'G';
			break;
		case 0b11:
			retValue = 'T';
			break;
	}
	return retValue;
}

// 0 1 2 3
// A C G T

void viterbiEnc(FILE *ofp, FILE *efp)
{
	// 本番は上
	int packetValue = 200000/(CONTENT - 2);
	// int packetValue = 10;
	for (int packetNum = 0; packetNum < packetValue; ++packetNum)
	{
		int binary[32] = {};
		int decimal = packetNum;
		int binI;
		for (binI=0;decimal>0;binI++)
		{
			binary[binI] = decimal % 2;
			decimal = decimal / 2;
		}
		int binNowState = 0b00;
		int result_in[2];

		//アドレスの生成
		for (int binI = ADDRESS-2-1; binI >= 0; --binI)
		{
			convolution_state_machine(binary[binI], binNowState, result_in);
			binNowState = result_in[1];
			// printf("%d", result_in[0]>>1);
			// printf("%d\n", result_in[0]&0b01);

			// ファイルへの書き込み
			fputc(retDNA(result_in[0]), efp);
		}
		//アドレスのtail bit
		for (int BinI = 0; BinI < 2; ++BinI)
		{
			convolution_state_machine(0, binNowState, result_in);
			binNowState = result_in[1];
			// printf("%d", result_in[0]>>1);
			// printf("%d\n", result_in[0]&0b01);

			// ファイルへの書き込み
			fputc(retDNA(result_in[0]), efp);
		}
		// printf("\n");

		int nowState = 0b00;
		char inputDNA;
		int inputData;
		// データの生成
		for(int i=0; i < CONTENT-2; i++)
		{
			inputDNA = getc(ofp);
			inputData = (int)(inputDNA - '0');
			// printf("%d\n", inputData);

			convolution_state_machine(inputData, nowState, result_in);
			nowState = result_in[1];
			// printf("%d", result_in[0]>>1);
			// printf("%d\n", result_in[0]&0b01);

			// ファイルへの書き込み
			fputc(retDNA(result_in[0]), efp);
		}
		// データのtail bit
		for (int BinI = 0; BinI < 2; ++BinI)
		{
			convolution_state_machine(0, nowState, result_in);
			nowState = result_in[1];
			// printf("%d", result_in[0]>>1);
			// printf("%d\n", result_in[0]&0b01);

			// ファイルへの書き込み
			fputc(retDNA(result_in[0]), efp);
		}
	}
	fputc('\n', efp);
}

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

	viterbiEnc(ofp, efp);

	fclose(ofp);
	fclose(efp);
	return(0);
}

int main()
{
	enc();
	return(0);
}
