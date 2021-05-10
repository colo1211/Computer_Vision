// 201710745 ����
/*
-3���� bmp ���� -
1) output1.bmp: lenna.bmp ���ϰ� ������ ����
2) output2.bmp: ���� ������ ��Ⱚ�� 50��ŭ ������Ų ����(���� overflow�� ���� Ŭ����ó���� �ʿ����)
3) output3.bmp: ���� ������ ���� ��� ����(negative image)
*/
#pragma warning(disable:4996) // safety issue
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h> // Bitmap, RGB 


int main() {
	BITMAPFILEHEADER hf; // 14byte  
	BITMAPINFOHEADER hInfo; // 40byte
	RGBQUAD hRGB[256]; // 1024byte 
	
	FILE* fp; 
	FILE* fp_1; // lenna.bmp ���ϰ� ������ ����
	FILE* fp_2; // ���� ������ ��Ⱚ�� 50��ŭ ������Ų ����
	FILE* fp_3; // ���� ������ ���� ��� ����

	fp = fopen("LENNA.bmp", "rb");
	if (fp == NULL) {
		printf("Not Found\n");
		return -1;
	}
	fread(&hf, sizeof(BITMAPFILEHEADER), 1, fp);
	fread(&hInfo, sizeof(BITMAPINFOHEADER), 1, fp);
	fread(hRGB, sizeof(RGBQUAD), 256, fp);

	int imgSize=hInfo.biWidth* hInfo.biHeight;
	BYTE* Image = (BYTE*)malloc(imgSize); // ���� ����ȯ 
	BYTE* Output_1 = (BYTE*)malloc(imgSize);
	BYTE* Output_2 = (BYTE*)malloc(imgSize);
	BYTE* Output_3 = (BYTE*)malloc(imgSize);
	fread(Image, sizeof(BYTE), imgSize, fp);
	fclose(fp);

	for (int x = 0; x < imgSize; x++) {
		Output_1[x] = Image[x];
		Output_2[x] = Image[x] + 50;
		Output_3[x] = 255 - Image[x]; 
	}


	fp_1 = fopen("output1.bmp", "wb");
	fp_2 = fopen("output2.bmp", "wb");
	fp_3 = fopen("output3.bmp", "wb");
	
	fwrite(&hf, sizeof(BYTE), sizeof(BITMAPFILEHEADER), fp_1);
	fwrite(&hInfo, sizeof(BYTE), sizeof(BITMAPINFOHEADER), fp_1);
	fwrite(hRGB, sizeof(RGBQUAD), 256, fp_1);
	fwrite(Output_1, sizeof(BYTE), imgSize, fp_1);

	fwrite(&hf, sizeof(BYTE), sizeof(BITMAPFILEHEADER), fp_2);
	fwrite(&hInfo, sizeof(BYTE), sizeof(BITMAPINFOHEADER), fp_2);
	fwrite(hRGB, sizeof(RGBQUAD), 256, fp_2);
	fwrite(Output_2, sizeof(BYTE), imgSize, fp_2);

	fwrite(&hf, sizeof(BYTE), sizeof(BITMAPFILEHEADER), fp_3);
	fwrite(&hInfo, sizeof(BYTE), sizeof(BITMAPINFOHEADER), fp_3);
	fwrite(hRGB, sizeof(RGBQUAD), 256, fp_3);
	fwrite(Output_3, sizeof(BYTE), imgSize, fp_3);


	fclose(fp_1);
	fclose(fp_2);
	fclose(fp_3);
	free(Image);
	free(Output_1); // �����Ҵ� ����, memory leak ����
	free(Output_2); // �����Ҵ� ����, memory leak ����
	free(Output_3); // �����Ҵ� ����, memory leak ����

	return 0; 
}