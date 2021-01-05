#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#define SIZE 100
#include "grpwk20.h"

const int ADDRESS = 19;
const int CONTENT = 25 - ADDRESS;

int result[1000000][4] = {};	// 復号結果の保存先
double minWeight[1000000];			// アドレスがかぶったときの判断用


// aとbのハミング距離を求める関数
double hamming_d(int a, int b)
{

	int tmp = a ^ b;

	//ユークリッド距離みたいなやつ
	int retVa = ((tmp&0b10)>>1) + (tmp&0b01);
	return sqrt(retVa);

	// ハミング距離
	int i = 0;

	while (tmp)
	{
		// 立っている最下位ビットを消す
		tmp &= tmp - 1;
		i++;
	}

	return i;
}

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

// トレリス線図のノード構造体
typedef struct
{
	// 状態0b00からこのノードにたどり着くまでの入力系列
	int flow[SIZE];

	// ノードの重み(ハミング距離の計算などで使用)
	double weight;

	// このノードが有効がどうか
	// 初期値はfalse
	int valid;
} Node;

// ビタビ複号を行う関数
// int *received_s : ビタビ複号により複号したい受信系列
// int input_l     : 受信系列の長さ
// int *est_in     : ビタビ複号により推測した送信系列
double viterbi(int *received_s, int input_l, int *est_in)
{

	// 現在のノード(c_node)と次のノード(n_node)
	// node[0]:状態0b00のノード
	// node[1]:状態0b01のノード
	// node[2]:状態0b10のノード
	// node[3]:状態0b11のノード
	Node c_node[4] = {};
	Node n_node[4] = {};

	// ノード配列の長さ(=4)
	int node_l = sizeof(c_node)/sizeof(Node);

	// 更新する前に一時的にノードの重みを保持する変数
	double tmp_weight;

	// ノードが判定可能かチェックする
	int is_updatable;

	// 0b00ノードの初期化
	c_node[0].flow[0] = 0;
	c_node[0].valid = 1;

	// 入力0,1の時の畳み込み符号化器関数の出力(ループ内で使用)
	int result_in0[2], output_in0, n_state_in0;
	int result_in1[2], output_in1, n_state_in1;

	//　時刻tにおける処理
	for(int t = 0; t < input_l; t++)
	{
		// node[i]における処理(iは状態を表す 0b00 ~ 0b11)
		for(int i = 0; i < node_l; i++)
		{
			// 有効なノードでなければスキップ
			if(c_node[i].valid == 0) continue;

			// 最後の2回は入力が0のみ
			// そうでなければ入力は0と1
			if(t < input_l - 2)
			{
				// 入力0について状態遷移関数の出力を格納
				convolution_state_machine(0, i, result_in0);
				output_in0  = result_in0[0];
				n_state_in0 = result_in0[1];

				// ハミング距離を計算し、ノードの重みを計算
				tmp_weight = c_node[i].weight + hamming_d(received_s[t], output_in0);

				// 「次状態ノードがまだ入っていない」　もしくは　「次状態ノードが入っているが、今のノードの重みが、すでに入っている次状態ノードの重みに比べて小さい」　場合のみ更新
				is_updatable = (n_node[n_state_in0].valid == 1) && (n_node[n_state_in0].weight > tmp_weight);

				if(n_node[n_state_in0].valid == 0 || is_updatable)
				{
					// 重みの更新(ハミング距離)
					n_node[n_state_in0].weight = tmp_weight;

					// フローの更新
					for (int j = 0; j < t; j++) n_node[n_state_in0].flow[j] = c_node[i].flow[j]; 
					n_node[n_state_in0].flow[t] = 0;
				}

				// ---入力1についても同様---

				// 入力1について状態遷移関数の出力を格納
				convolution_state_machine(1, i, result_in1);
				output_in1  = result_in1[0];
				n_state_in1 = result_in1[1];

				tmp_weight = c_node[i].weight + hamming_d(received_s[t], output_in1);

				is_updatable = (n_node[n_state_in1].valid == 1) && (n_node[n_state_in1].weight > tmp_weight);

				if(n_node[n_state_in1].valid == 0 || is_updatable)
				{
					// 重みの更新(ハミング距離)
					n_node[n_state_in1].weight = tmp_weight;

					// フローの更新
					for (int j = 0; j < t; j++) n_node[n_state_in1].flow[j] = c_node[i].flow[j]; 
					n_node[n_state_in1].flow[t] = 1;
				}

				// validの有効化
				n_node[n_state_in0].valid = 1;
				n_node[n_state_in1].valid = 1;

			}
			else	// テールビットの処理
			{
				// 最後の2回の入力は0のみ
				convolution_state_machine(0, i, result_in0);
				output_in0  = result_in0[0];
				n_state_in0 = result_in0[1];

				// ハミング距離を計算し、ノードの重みを計算
				tmp_weight = c_node[i].weight + hamming_d(received_s[t], output_in0);

				// 「次状態ノードがまだ入っていない」　もしくは　「次状態ノードが入っているが、今のノードの重みが、すでに入っている次状態ノードの重みに比べて小さい」　場合のみ更新
				is_updatable = (n_node[n_state_in0].valid == 1) && (n_node[n_state_in0].weight > tmp_weight);
				if(n_node[n_state_in0].valid == 0 || is_updatable)
				{
					// 重みの更新(ハミング距離)
					n_node[n_state_in0].weight = tmp_weight;

					// フローの更新
					for (int j = 0; j < t; j++) n_node[n_state_in0].flow[j] = c_node[i].flow[j];
					n_node[n_state_in0].flow[t] = 0;
				}

				// validの有効化
				n_node[n_state_in0].valid = 1;
			}
		} // END for i

		// ノードの更新と次状態ノードの無効化
		for(int i = 0; i < node_l; i++)
		{
			//memcpy(c_node[i].flow, n_node[i].flow, sizeof(int) * SIZE);
			for(int j = 0; j < 100; j++) c_node[i].flow[j] = n_node[i].flow[j];
			c_node[i].weight = n_node[i].weight;
			c_node[i].valid = n_node[i].valid;

			n_node[i].valid = 0;
		}

	} // END for t

	for(int i = 0; i < input_l; i++){
		est_in[i] = c_node[0].flow[i];
	}
	return c_node[0].weight;
}

int retDecimal(char DNA)
{
	int retValue;
	switch (DNA)
	{
		case 'A':
			retValue = 0;
			break;
		case 'C':
			retValue = 1;
			break;
		case 'G':
			retValue = 2;
			break;
		case 'T':
			retValue = 3;
			break;
	}
	return retValue;
}

void viterbiDec(FILE *sfp, FILE *dfp)
{
	//本番は上
	int packetNum = 200000/(CONTENT - 2);
	// int packetNum = 10;
	int maad = 0;
	char nowDNA;
	// for(int i = 0; (c = getc(sfp)) != '\n'; ++i)
	for(int kkk = 0; kkk < packetNum; ++kkk)
	{
		//アドレスの復号
		int address = 0;
		int received_address[ADDRESS];
		for (int binI = 0; binI < ADDRESS; ++binI)
		{
			nowDNA = getc(sfp);
			// printf("%c", nowDNA);
			received_address[binI] = retDecimal(nowDNA);
			// printf("%d", received_address[binI]);
		}
		// printf("\n");
		int estAddress[SIZE];
		double kakunin = viterbi(received_address, ADDRESS, estAddress);
		for (int i = 0; i < ADDRESS - 2; i++)
		{
			// printf("%d", estAddress[i]);
			address += estAddress[i] * (1<<(ADDRESS-2-1-i));
		}
		// printf("\n");
		// printf("%d\n", address);
		// printf("\n");


		//データの復号
		int data;
		int received_data[CONTENT];
		for (int binI = 0; binI < CONTENT; ++binI)
		{
			nowDNA = getc(sfp);
			// printf("%c", nowDNA);
			received_data[binI] = retDecimal(nowDNA);
			// printf("%d", received_data[binI]);
		}
		// printf("\n");
		int estData[SIZE];
		kakunin += viterbi(received_data, CONTENT, estData);
		if (minWeight[address] > kakunin)
		{
			minWeight[address] = kakunin;
			for (int i = 0; i < CONTENT - 2; i++)
			{
				result[address][i] = estData[i];
				// printf("%d", estData[i]);
			}
		}
		// printf("\n\n");
		maad = (address > maad) ? address : maad;
	}
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

	char sute;
	viterbiDec(sfp, dfp);
	sute = getc(sfp);
	viterbiDec(sfp, dfp);
	sute = getc(sfp);
	viterbiDec(sfp, dfp);
	// sute = getc(sfp);
	// viterbiDec(sfp, dfp);
	// sute = getc(sfp);
	// viterbiDec(sfp, dfp);
	// sute = getc(sfp);
	// viterbiDec(sfp, dfp);
	// sute = getc(sfp);
	// viterbiDec(sfp, dfp);
	// sute = getc(sfp);
	// viterbiDec(sfp, dfp);
	// sute = getc(sfp);
	// viterbiDec(sfp, dfp);
	// sute = getc(sfp);
	// viterbiDec(sfp, dfp);

	// printf("%d\n", maad);
	for (int i = 0; i < 200000/(CONTENT-2); ++i)
	{
		for (int j = 0; j < CONTENT-2; ++j)
		{
			fputc((char)('0' + result[i][j]), dfp);
		}
	}
	// printf("\n");
	fputc('\n', dfp);

	fclose(sfp);
	fclose(dfp);
	return(0);
}

int main()
{
	for (int i = 0; i < 1000000; ++i)
	{
		minWeight[i] = (1<<30)-1;
	}
	dec();

	// // 受信系列(ここを推定したい受信系列に変える)
	// int received_s[] =
	// {
	// 	0b00,
	// 	0b11,
	// 	0b10,
	// 	0b10,
	// 	0b11,
	// 	0b00
	// };

	// // received_s配列の長さ
	// int input_l = sizeof(received_s) / sizeof(int);

	// // 推定した入力系列
	// int est_in[SIZE];

	// // Viterbi関数の実行
	// viterbi(received_s, input_l, est_in);

	// // 結果の出力
	// printf("ビタビアルゴリズムによる入力系列の推定結果(拘束長3)\n");

	// // 最後の２つは入力0なので、表示しない
	// for (int i = 0; i < input_l - 2; i++){
	// 	printf("%d\n", est_in[i]);
	// }

	return 0;
}
