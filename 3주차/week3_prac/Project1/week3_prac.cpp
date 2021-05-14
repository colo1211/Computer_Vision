#pragma warning(disable:4996)
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <math.h>
#include <Windows.h>

void InverseImage(BYTE* Img, BYTE* Out, int W, int H) { // �̹��� ȭ������, ������ ����, ���� ������ 
	int ImgSize = W * H;
	for (int i = 0; i < ImgSize; i++)
		Out[i] = 255 - Img[i]; // ��� ���� 
}

void BrightnessAdj(BYTE* Img, BYTE* Out, int W, int H, int val) { // �̹��� ȭ������, ������ ����, ���� ������ 
	int ImgSize = W * H;
	for (int i = 0; i < ImgSize; i++) {
		if (Img[i] + val > 255) { // Ŭ���� �����÷ο� ó�� (ó��X-> �˰� ���) 
			Out[i] = 255;
		}
		else if (Img[i] + val < 0) { // Ŭ���� ����÷ο� ó�� (ó��X-> ��� ���) 
			Out[i] = 0;
		}
		Out[i] = val + Img[i]; // ��� ���� 
	}
}

// ��� ó�� �Լ�
void ContrastAdj(BYTE* Img, BYTE* Out, int W, int H, double val) {
	int ImgSize = W * H;
	for (int i = 0; i < ImgSize; i++) {
		if (Img[i] * val > 255.0) { // Ŭ���� �����÷ο� ó�� (ó��X-> �˰� ���) 
			Out[i] = 255;
		}
		else Out[i] = (BYTE)(val * Img[i]); // ���� ����ȯ(Double * int => BYTE)  
	}
}
void ObtainHistogram(BYTE* Img, int* Histo, int W, int H) {
	int ImgSize = W * H;
	for (int i = 0; i < ImgSize; i++) {
		Histo[Img[i]]++;
	}
}

void HistogramStretching(BYTE* Img, BYTE* Out, int* Histo, int W, int H) {
	int ImgSize = W * H;
	BYTE Low, High;
	for (int i = 0; i < 256; i++) {
		if (Histo[i] != 0) {
			Low = i;
			break;
		}
	}
	for (int i = 255; i >= 0; i--) {
		if (Histo[i] != 0) {
			High = i;
			break;
		}
	}
	for (int i = 0; i < ImgSize; i++) {
		Out[i] = (BYTE)((Img[i] - Low) / (double)(High - Low) * 255.0);
	}
}

void ObtainAHistogram(int* Histo, int* AHisto) { //�������� ���ϴ� �Լ� (AHisto) 
	for (int y = 0; y < 256; y++) {
		for (int x = 0; x <= y; x++) {
			AHisto[y] += Histo[x];
		}
	}
}

void HistogramEqualization(BYTE* Img, BYTE* Out, int* AHisto, int W, int H) {
	int ImgSize = W * H;
	int Nt = W * H;
	int Gmax = 255;
	double Ratio = Gmax / (double)Nt;
	BYTE NormSum[256];
	for (int i = 0; i < 256; i++) {
		NormSum[i] = (BYTE)(Ratio * AHisto[i]); // Ratio-Double, AHisto-Int 
	}
	for (int i = 0; i < ImgSize; i++) {
		Out[i] = NormSum[Img[i]];
	}
}

int GonzalezBinThresh(BYTE* Img, int* Histo, int W, int H) { // �Ӱ�ġ �ڵ����� (By. Gonzales & Woods)  
	double m1=0, m2=0; // ��� ��
	int current_T=0; // ���� �Ӱ� �� 
	BYTE Low, High;
	for (int i = 0; i < 256; i++) {
		if (Histo[i] != 0) {
			Low = i;
			break;
		}
	}
	for (int i = 255; i >= 0; i--) {
		if (Histo[i] != 0) {
			High = i;
			break;
		}
	}
	current_T= (High + Low) / 2;
	printf("���� ���� �Ӱ谪:%d, �ּҰ�:%d, �ִ밪:%d\n", current_T, Low, High); // �ʱ� �Ӱ谪:93, �ּҰ�:18, �ִ밪:168
	while(1){
		int count_m1 = 0; int sum_m1 = 0; 
		int count_m2 = 0; int sum_m2 = 0;
		for (int i = 0; i < 256; i++) { // G1���� �Ӱ�ġ���� ���� ȭ��  
			if (i > current_T && Histo[i]!=0) { //G1
				sum_m1 = sum_m1 + (i* Histo[i]);
				count_m1 += Histo[i];
			}
			else if (i < current_T && Histo[i] != 0){ //G2
				sum_m2 = sum_m2 + (i * Histo[i]);
				count_m2 += Histo[i];
			}
		}
		// G1 ��� -> m1
		m1 = sum_m1 / count_m1;
		// G2 ��� -> m2
		m2 = sum_m2 / count_m2;
		printf("m1:%d, count_m1:%d, m2:%d, count_m2:%d\n", m1, count_m1, m2 ,count_m2);
		// ���ο� �Ӱ�ġ T
		int new_T = (m1 + m2) / 2;
		printf("new_T:%d\n", new_T);
		int e = abs(new_T-current_T);
		if (e <= 3) {
			printf("Ż���ϴ� ������ ����, new_T:%d, current_T:%d\n", new_T, current_T);
			current_T = new_T;
			break;
		}
		current_T = new_T;
	}
	printf("���� ���� �Ӱ谪:%d, �ּҰ�:%d, �ִ밪:%d\n", current_T, Low, High); //���� ���� �Ӱ谪:100, �ּҰ�:18, �ִ밪:168
	return current_T;
}

void Binarization(BYTE* Img, BYTE* Out, int W, int H, BYTE ThresHold) {
	int ImgSize = W * H;
	for (int i = 0; i < ImgSize; i++) {
		if (Img[i] < ThresHold) Out[i] = 0;
		else Out[i] = 255;
	}
}

int main()
{
	BITMAPFILEHEADER hf; // BMP ������� 14Bytes
	BITMAPINFOHEADER hInfo; // BMP ������� 40Bytes
	RGBQUAD hRGB[256]; // �ȷ�Ʈ (256 * 4Bytes)
	FILE* fp;
	fp = fopen("coin.bmp", "rb");
	if (fp == NULL) {
		return -1;
	}
	fread(&hf, sizeof(BITMAPFILEHEADER), 1, fp);
	fread(&hInfo, sizeof(BITMAPINFOHEADER), 1, fp);
	fread(hRGB, sizeof(RGBQUAD), 256, fp);
	int ImgSize = hInfo.biWidth * hInfo.biHeight;
	BYTE* Image = (BYTE*)malloc(ImgSize);
	BYTE* Output = (BYTE*)malloc(ImgSize);
	fread(Image, sizeof(BYTE), ImgSize, fp);
	fclose(fp);

	int Histo[256] = { 0 }; // ��ü �ȼ��� �ش��ϴ� ȭ�Ұ��� ī��Ʈ�ϴ� Bucket
	int AHisto[256] = { 0 }; // ���� Histo 

	// ����ó��
	//InverseImage(Image, Output, hInfo.biWidth,hInfo.biHeight);
	//BrightnessAdj(Image, Output, hInfo.biWidth,hInfo.biHeight,70);
	//ContrastAdj(Image, Output, hInfo.biWidth, hInfo.biHeight, 0.5);

	ObtainHistogram(Image, Histo, hInfo.biWidth, hInfo.biHeight);
	//ObtainAHistogram(Histo, AHisto);  // �������� ���ϴ� �Լ� 
	//HistogramEqualization(Image, Output, AHisto, hInfo.biWidth, hInfo.biHeight);

	//HistogramStretching(Image, Output,Histo, hInfo.biWidth, hInfo.biHeight);



	BYTE T = GonzalezBinThresh(Image, Histo, hInfo.biWidth, hInfo.biHeight);
	Binarization(Image, Output, hInfo.biWidth, hInfo.biHeight, T);


	/* ������׷� ���
	for (int i = 0; i < 256; i++) {
		printf("%d\n", Histo[i]);
	}
	*/


	fp = fopen("output.bmp", "wb");
	fwrite(&hf, sizeof(BYTE), sizeof(BITMAPFILEHEADER), fp);
	fwrite(&hInfo, sizeof(BYTE), sizeof(BITMAPINFOHEADER), fp);
	fwrite(hRGB, sizeof(RGBQUAD), 256, fp);
	fwrite(Output, sizeof(BYTE), ImgSize, fp);
	fclose(fp);
	free(Image);
	free(Output);
}