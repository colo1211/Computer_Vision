#pragma warning(disable:4996)
#include <stdio.h>
#include <stdlib.h>
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
	double m1 = 0, m2 = 0; // ��� ��
	int ImgSize = W * H;
	BYTE current_T = 0; // ���� �Ӱ� �� 
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
	current_T = (High + Low) / 2;
	printf("���� ���� �Ӱ谪:%d\n", current_T); // �ʱ� �Ӱ谪:93
	
	while (1) {
		int count_m1 = 0; int sum_m1 = 0;
		int count_m2 = 0; int sum_m2 = 0;
		for (int i = 0; i < ImgSize; i++) { // G1���� �Ӱ�ġ���� ���� ȭ��  
			if (Img[i] > current_T) { 
				sum_m1 += Img[i];
				count_m1++;
			}
			else if (Img[i] < current_T) { // G2���� �Ӱ�ġ���� ��ο� ȭ�� 
				sum_m2 += Img[i];
				count_m2++;
			}
		}
		m1 = sum_m1 / (double)count_m1;// G1 ��� -> m1
		m2 = sum_m2 / (double)count_m2;// G2 ��� -> m2

		printf("m1:%f, m2:%f\n", m1, m2);
		
		BYTE new_T = (m1 + m2) / 2;
		double e = new_T - current_T;

		if (e <= 3) {
			current_T = new_T;
			break;
		}
		current_T = new_T;
	}
	printf("���� ���� �Ӱ谪:%d\n", current_T); //���� ���� �Ӱ谪:100
	return current_T;
}

//�ڽ� ��Ȱȭ 
void AverageConv(BYTE* Img, BYTE* Out, int W, int H) {
	double Kernel[3][3] = { // 1/9 ���� �ϹǷ� double Ÿ������ �迭 ���� 
		0.1111,0.1111,0.1111,
		0.1111,0.1111,0.1111,
		0.1111,0.1111,0.1111
	};
	double SumProduct = 0.0;

	for (int i = 1; i < H-1; i++) { // ��,�Ʒ� ������ ���, ������ ���� (Y��ǥ) 
		for (int j = 1; j < W-1; j++) { // ������ ���� (X ��ǥ) 
			for (int m = -1; m <= 1; m++) { // Ŀ�� �����߽� ��(-1), �ڽ�(0), �Ʒ�(1) 
				for (int n = -1; n <= 1; n++) {  //Ŀ�� �����߽� ����(-1), �ڽ�(0), ������(1) 
					SumProduct += Img[(i+m)*W + (j+n)] * Kernel[m+1][n+1]; // 2���� �迭�� 1���� �迭�� Mapping �ϴ� ������ Syntax: Y(������ Y��ǥ) * W(������ ���λ�����) + X(������ X ��ǥ) 
				}
			}
			Out[i*W+j] = (BYTE)SumProduct;
			SumProduct = 0.0;
		}
	}
}

// ����þ� ��Ȱȭ, ���Ϳ� ����ġ, ���Ϳ��� �־������� Weight ���� => Sum:1 (����, 1/16) 
void GaussAvrConv(BYTE* Img, BYTE* Out, int W, int H) {
	double Kernel[3][3] = { // �ڽ���Ȱȭ�� ������ ����, But Kernel Value�� ����. 
		0.0625,0.125,0.0625,
		0.125,0.25,0.125,
		0.0625, 0.125, 0.0625
	};
	double SumProduct = 0.0;

	for (int i = 1; i < H - 1; i++) { // ��,�Ʒ� ������ ���, ������ ���� (Y��ǥ) 
		for (int j = 1; j < W - 1; j++) { // ������ ���� (X ��ǥ) 
			for (int m = -1; m <= 1; m++) { // Ŀ�� �����߽� ��(-1), �ڽ�(0), �Ʒ�(1) 
				for (int n = -1; n <= 1; n++) {  //Ŀ�� �����߽� ����(-1), �ڽ�(0), ������(1) 
					SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1]; // 2���� �迭�� 1���� �迭�� Mapping �ϴ� ������ Syntax: Y(������ Y��ǥ) * W(������ ���λ�����) + X(������ X ��ǥ) 
				}
			}
			Out[i * W + j] = (BYTE)SumProduct;
			SumProduct = 0.0;
		}
	}
}

// Prewitt ����ũ X
void Prewitt_X_Conv(BYTE* Img, BYTE* Out, int W, int H) {
	double Kernel[3][3] = {  
		-1.0, 0.0, 1.0,
		-1.0, 0.0, 1.0,
		-1.0, 0.0, 1.0
	};
	double SumProduct = 0.0;
	for (int i = 1; i < H - 1; i++) { // ��,�Ʒ� ������ ���, ������ ���� (Y��ǥ) 
		for (int j = 1; j < W - 1; j++) { // ������ ���� (X ��ǥ) 
			for (int m = -1; m <= 1; m++) { // Ŀ�� �����߽� ��(-1), �ڽ�(0), �Ʒ�(1) 
				for (int n = -1; n <= 1; n++) {  //Ŀ�� �����߽� ����(-1), �ڽ�(0), ������(1) 
					SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1]; // 2���� �迭�� 1���� �迭�� Mapping �ϴ� ������ Syntax: Y(������ Y��ǥ) * W(������ ���λ�����) + X(������ X ��ǥ) 
				}
			}
			// 0~765 => 0~255
			Out[i * W + j] = abs((long)SumProduct) / 3;
			SumProduct = 0.0;
		}
	}
}

// Prewitt ����ũ Y
void Prewitt_Y_Conv(BYTE* Img, BYTE* Out, int W, int H) {
	double Kernel[3][3] = {
		-1.0, -1.0, -1.0,
		0.0, 0.0, 0.0,
		1.0, 1.0, 1.0
	};
	double SumProduct = 0.0;
	for (int i = 1; i < H - 1; i++) { // ��,�Ʒ� ������ ���, ������ ���� (Y��ǥ) 
		for (int j = 1; j < W - 1; j++) { // ������ ���� (X ��ǥ) 
			for (int m = -1; m <= 1; m++) { // Ŀ�� �����߽� ��(-1), �ڽ�(0), �Ʒ�(1) 
				for (int n = -1; n <= 1; n++) {  //Ŀ�� �����߽� ����(-1), �ڽ�(0), ������(1) 
					SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1]; // 2���� �迭�� 1���� �迭�� Mapping �ϴ� ������ Syntax: Y(������ Y��ǥ) * W(������ ���λ�����) + X(������ X ��ǥ) 
				}
			}
			// 0~765 => 0~255
			Out[i * W + j] = abs((long)SumProduct) / 3;
			SumProduct = 0.0;
		}
	}
}
// Prewitt ����ũ X
void Sobel_X_Conv(BYTE* Img, BYTE* Out, int W, int H) {
	double Kernel[3][3] = {
		-1.0, 0.0, 1.0,
		-2.0, 0.0, 2.0,
		-1.0, 0.0, 1.0
	};
	double SumProduct = 0.0;
	for (int i = 1; i < H - 1; i++) { // ��,�Ʒ� ������ ���, ������ ���� (Y��ǥ) 
		for (int j = 1; j < W - 1; j++) { // ������ ���� (X ��ǥ) 
			for (int m = -1; m <= 1; m++) { // Ŀ�� �����߽� ��(-1), �ڽ�(0), �Ʒ�(1) 
				for (int n = -1; n <= 1; n++) {  //Ŀ�� �����߽� ����(-1), �ڽ�(0), ������(1) 
					SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1]; // 2���� �迭�� 1���� �迭�� Mapping �ϴ� ������ Syntax: Y(������ Y��ǥ) * W(������ ���λ�����) + X(������ X ��ǥ) 
				}
			}
			// 0~765 => 0~255
			Out[i * W + j] = abs((long)SumProduct) / 4;
			SumProduct = 0.0;
		}
	}
}

// Prewitt ����ũ Y
void Sobel_Y_Conv(BYTE* Img, BYTE* Out, int W, int H) {
	double Kernel[3][3] = {
		-1.0,-2.0,-1.0,
		0.0,0.0,0.0,
		1.0,2.0,1.0
	};
	double SumProduct = 0.0;
	for (int i = 1; i < H - 1; i++) { // ��,�Ʒ� ������ ���, ������ ���� (Y��ǥ) 
		for (int j = 1; j < W - 1; j++) { // ������ ���� (X ��ǥ) 
			for (int m = -1; m <= 1; m++) { // Ŀ�� �����߽� ��(-1), �ڽ�(0), �Ʒ�(1) 
				for (int n = -1; n <= 1; n++) {  //Ŀ�� �����߽� ����(-1), �ڽ�(0), ������(1) 
					SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1]; // 2���� �迭�� 1���� �迭�� Mapping �ϴ� ������ Syntax: Y(������ Y��ǥ) * W(������ ���λ�����) + X(������ X ��ǥ) 
				}
			}
			// 0~765 => 0~255
			Out[i * W + j] = abs((long)SumProduct) / 4;
			SumProduct = 0.0;
		}
	}
}
void Laplace_Conv(BYTE* Img, BYTE* Out, int W, int H) {
	double Kernel[3][3] = {
		-1.0, -1.0, -1.0,
		-1.0, 8.0, -1.0,
		-1.0, -1.0, -1.0
	};
	double SumProduct = 0.0;
	for (int i = 1; i < H - 1; i++) { // ��,�Ʒ� ������ ���, ������ ���� (Y��ǥ) 
		for (int j = 1; j < W - 1; j++) { // ������ ���� (X ��ǥ) 
			for (int m = -1; m <= 1; m++) { // Ŀ�� �����߽� ��(-1), �ڽ�(0), �Ʒ�(1) 
				for (int n = -1; n <= 1; n++) {  //Ŀ�� �����߽� ����(-1), �ڽ�(0), ������(1) 
					SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1]; // 2���� �迭�� 1���� �迭�� Mapping �ϴ� ������ Syntax: Y(������ Y��ǥ) * W(������ ���λ�����) + X(������ X ��ǥ) 
				}
			}
			// 0~2040 => 0~255
			Out[i * W + j] = abs((long)SumProduct) / 8;
			SumProduct = 0.0;
		}
	}
}

// ���ö�þ� 9, ��� ���� + ��� ������ (������) ���� 
void Laplace_Conv_DC(BYTE* Img, BYTE* Out, int W, int H) {
	double Kernel[3][3] = {
		-1.0, -1.0, -1.0,
		-1.0, 9.0, -1.0,
		-1.0, -1.0, -1.0
	};
	double SumProduct = 0.0;
	for (int i = 1; i < H - 1; i++) { // ��,�Ʒ� ������ ���, ������ ���� (Y��ǥ) 
		for (int j = 1; j < W - 1; j++) { // ������ ���� (X ��ǥ) 
			for (int m = -1; m <= 1; m++) { // Ŀ�� �����߽� ��(-1), �ڽ�(0), �Ʒ�(1) 
				for (int n = -1; n <= 1; n++) {  //Ŀ�� �����߽� ����(-1), �ڽ�(0), ������(1) 
					SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1]; // 2���� �迭�� 1���� �迭�� Mapping �ϴ� ������ Syntax: Y(������ Y��ǥ) * W(������ ���λ�����) + X(������ X ��ǥ) 
				}
			}
			// 0~2040 => 0~255
			if (SumProduct > 255.0) Out[i * W + j] = 255;
			else if (SumProduct < 0.0) Out[i * W + j] = 0;
			else Out[i * W + j] = (BYTE)SumProduct;
			SumProduct = 0.0;
		}
	}
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
	fp = fopen("LENNA.bmp", "rb");
	if (fp == NULL) {
		return -1;
	}
	fread(&hf, sizeof(BITMAPFILEHEADER), 1, fp);
	fread(&hInfo, sizeof(BITMAPINFOHEADER), 1, fp);
	fread(hRGB, sizeof(RGBQUAD), 256, fp);
	int ImgSize = hInfo.biWidth * hInfo.biHeight;
	BYTE* Image = (BYTE*)malloc(ImgSize);
	BYTE* Temp = (BYTE*)malloc(ImgSize);
	BYTE* Output = (BYTE*)malloc(ImgSize);
	fread(Image, sizeof(BYTE), ImgSize, fp);
	fclose(fp);

	int Histo[256] = { 0 }; // ��ü �ȼ��� �ش��ϴ� ȭ�Ұ��� ī��Ʈ�ϴ� Bucket
	int AHisto[256] = { 0 }; // ���� Histo 

	// ����ó��
	//InverseImage(Image, Output, hInfo.biWidth,hInfo.biHeight);
	//BrightnessAdj(Image, Output, hInfo.biWidth,hInfo.biHeight,70);
	//ContrastAdj(Image, Output, hInfo.biWidth, hInfo.biHeight, 0.5);

	//ObtainHistogram(Image, Histo, hInfo.biWidth, hInfo.biHeight);
	//ObtainAHistogram(Histo, AHisto);  // �������� ���ϴ� �Լ� 
	//HistogramEqualization(Image, Output, AHisto, hInfo.biWidth, hInfo.biHeight);

	//HistogramStretching(Image, Output,Histo, hInfo.biWidth, hInfo.biHeight);

	//BYTE T = GonzalezBinThresh(Image, Histo, hInfo.biWidth, hInfo.biHeight);
	//Binarization(Image, Output, hInfo.biWidth, hInfo.biHeight, T);

	//AverageConv(Image, Output, hInfo.biWidth, hInfo.biHeight);
	//GaussAvrConv(Image, Output, hInfo.biWidth, hInfo.biHeight);
	
	/*
	Sobel_X_Conv(Image, Temp, hInfo.biWidth, hInfo.biHeight);
	Sobel_Y_Conv(Image, Output, hInfo.biWidth, hInfo.biHeight);
	for (int i = 0; i < ImgSize; i++) {
		if (Temp[i] > Output[i]) Output[i] = Temp[i];
	}
	Binarization(Output, Output, hInfo.biWidth, hInfo.biHeight, 40);
	*/
	GaussAvrConv(Image, Temp, hInfo.biWidth, hInfo.biHeight); // ������ ����
	Laplace_Conv_DC(Temp, Output, hInfo.biWidth, hInfo.biHeight);

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
	free(Temp);
}