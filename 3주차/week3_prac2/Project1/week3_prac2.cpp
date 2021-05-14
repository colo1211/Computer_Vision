#pragma warning(disable:4996)
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>

void InverseImage(BYTE* Img, BYTE* Out, int W, int H) { // 이미지 화소정보, 영상의 가로, 세로 사이즈 
	int ImgSize = W * H;
	for (int i = 0; i < ImgSize; i++)
		Out[i] = 255 - Img[i]; // 명암 반전 
}

void BrightnessAdj(BYTE* Img, BYTE* Out, int W, int H, int val) { // 이미지 화소정보, 영상의 가로, 세로 사이즈 
	int ImgSize = W * H;
	for (int i = 0; i < ImgSize; i++) {
		if (Img[i] + val > 255) { // 클리핑 오버플로우 처리 (처리X-> 검게 출력) 
			Out[i] = 255;
		}
		else if (Img[i] + val < 0) { // 클리핑 언더플로우 처리 (처리X-> 밝게 출력) 
			Out[i] = 0;
		}
		Out[i] = val + Img[i]; // 명암 반전 
	}
}

// 대비 처리 함수
void ContrastAdj(BYTE* Img, BYTE* Out, int W, int H, double val) {
	int ImgSize = W * H;
	for (int i = 0; i < ImgSize; i++) {
		if (Img[i] * val > 255.0) { // 클리핑 오버플로우 처리 (처리X-> 검게 출력) 
			Out[i] = 255;
		}
		else Out[i] = (BYTE)(val * Img[i]); // 강제 형변환(Double * int => BYTE)  
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

void ObtainAHistogram(int* Histo, int* AHisto) { //누적합을 구하는 함수 (AHisto) 
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

int GonzalezBinThresh(BYTE* Img, int* Histo, int W, int H) { // 임계치 자동결정 (By. Gonzales & Woods)  
	double m1 = 0, m2 = 0; // 평균 값
	int ImgSize = W * H;
	BYTE current_T = 0; // 현재 임계 값 
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
	printf("설정 이전 임계값:%d\n", current_T); // 초기 임계값:93
	
	while (1) {
		int count_m1 = 0; int sum_m1 = 0;
		int count_m2 = 0; int sum_m2 = 0;
		for (int i = 0; i < ImgSize; i++) { // G1에는 임계치보다 밝은 화소  
			if (Img[i] > current_T) { 
				sum_m1 += Img[i];
				count_m1++;
			}
			else if (Img[i] < current_T) { // G2에는 임계치보다 어두운 화소 
				sum_m2 += Img[i];
				count_m2++;
			}
		}
		m1 = sum_m1 / (double)count_m1;// G1 평균 -> m1
		m2 = sum_m2 / (double)count_m2;// G2 평균 -> m2

		printf("m1:%f, m2:%f\n", m1, m2);
		
		BYTE new_T = (m1 + m2) / 2;
		double e = new_T - current_T;

		if (e <= 3) {
			current_T = new_T;
			break;
		}
		current_T = new_T;
	}
	printf("설정 이후 임계값:%d\n", current_T); //설정 이후 임계값:100
	return current_T;
}

//박스 평활화 
void AverageConv(BYTE* Img, BYTE* Out, int W, int H) {
	double Kernel[3][3] = { // 1/9 들어가야 하므로 double 타입으로 배열 선언 
		0.1111,0.1111,0.1111,
		0.1111,0.1111,0.1111,
		0.1111,0.1111,0.1111
	};
	double SumProduct = 0.0;

	for (int i = 1; i < H-1; i++) { // 위,아래 마진을 고려, 영상의 세로 (Y좌표) 
		for (int j = 1; j < W-1; j++) { // 영상의 가로 (X 좌표) 
			for (int m = -1; m <= 1; m++) { // 커널 센터중심 위(-1), 자신(0), 아래(1) 
				for (int n = -1; n <= 1; n++) {  //커널 센터중심 왼쪽(-1), 자신(0), 오른쪽(1) 
					SumProduct += Img[(i+m)*W + (j+n)] * Kernel[m+1][n+1]; // 2차원 배열을 1차원 배열에 Mapping 하는 관용적 Syntax: Y(영상의 Y좌표) * W(영상의 가로사이즈) + X(영상의 X 좌표) 
				}
			}
			Out[i*W+j] = (BYTE)SumProduct;
			SumProduct = 0.0;
		}
	}
}

// 가우시안 평활화, 센터에 가중치, 센터에서 멀어질수록 Weight 낮음 => Sum:1 (따라서, 1/16) 
void GaussAvrConv(BYTE* Img, BYTE* Out, int W, int H) {
	double Kernel[3][3] = { // 박스평활화와 로직은 동일, But Kernel Value만 차이. 
		0.0625,0.125,0.0625,
		0.125,0.25,0.125,
		0.0625, 0.125, 0.0625
	};
	double SumProduct = 0.0;

	for (int i = 1; i < H - 1; i++) { // 위,아래 마진을 고려, 영상의 세로 (Y좌표) 
		for (int j = 1; j < W - 1; j++) { // 영상의 가로 (X 좌표) 
			for (int m = -1; m <= 1; m++) { // 커널 센터중심 위(-1), 자신(0), 아래(1) 
				for (int n = -1; n <= 1; n++) {  //커널 센터중심 왼쪽(-1), 자신(0), 오른쪽(1) 
					SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1]; // 2차원 배열을 1차원 배열에 Mapping 하는 관용적 Syntax: Y(영상의 Y좌표) * W(영상의 가로사이즈) + X(영상의 X 좌표) 
				}
			}
			Out[i * W + j] = (BYTE)SumProduct;
			SumProduct = 0.0;
		}
	}
}

// Prewitt 마스크 X
void Prewitt_X_Conv(BYTE* Img, BYTE* Out, int W, int H) {
	double Kernel[3][3] = {  
		-1.0, 0.0, 1.0,
		-1.0, 0.0, 1.0,
		-1.0, 0.0, 1.0
	};
	double SumProduct = 0.0;
	for (int i = 1; i < H - 1; i++) { // 위,아래 마진을 고려, 영상의 세로 (Y좌표) 
		for (int j = 1; j < W - 1; j++) { // 영상의 가로 (X 좌표) 
			for (int m = -1; m <= 1; m++) { // 커널 센터중심 위(-1), 자신(0), 아래(1) 
				for (int n = -1; n <= 1; n++) {  //커널 센터중심 왼쪽(-1), 자신(0), 오른쪽(1) 
					SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1]; // 2차원 배열을 1차원 배열에 Mapping 하는 관용적 Syntax: Y(영상의 Y좌표) * W(영상의 가로사이즈) + X(영상의 X 좌표) 
				}
			}
			// 0~765 => 0~255
			Out[i * W + j] = abs((long)SumProduct) / 3;
			SumProduct = 0.0;
		}
	}
}

// Prewitt 마스크 Y
void Prewitt_Y_Conv(BYTE* Img, BYTE* Out, int W, int H) {
	double Kernel[3][3] = {
		-1.0, -1.0, -1.0,
		0.0, 0.0, 0.0,
		1.0, 1.0, 1.0
	};
	double SumProduct = 0.0;
	for (int i = 1; i < H - 1; i++) { // 위,아래 마진을 고려, 영상의 세로 (Y좌표) 
		for (int j = 1; j < W - 1; j++) { // 영상의 가로 (X 좌표) 
			for (int m = -1; m <= 1; m++) { // 커널 센터중심 위(-1), 자신(0), 아래(1) 
				for (int n = -1; n <= 1; n++) {  //커널 센터중심 왼쪽(-1), 자신(0), 오른쪽(1) 
					SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1]; // 2차원 배열을 1차원 배열에 Mapping 하는 관용적 Syntax: Y(영상의 Y좌표) * W(영상의 가로사이즈) + X(영상의 X 좌표) 
				}
			}
			// 0~765 => 0~255
			Out[i * W + j] = abs((long)SumProduct) / 3;
			SumProduct = 0.0;
		}
	}
}
// Prewitt 마스크 X
void Sobel_X_Conv(BYTE* Img, BYTE* Out, int W, int H) {
	double Kernel[3][3] = {
		-1.0, 0.0, 1.0,
		-2.0, 0.0, 2.0,
		-1.0, 0.0, 1.0
	};
	double SumProduct = 0.0;
	for (int i = 1; i < H - 1; i++) { // 위,아래 마진을 고려, 영상의 세로 (Y좌표) 
		for (int j = 1; j < W - 1; j++) { // 영상의 가로 (X 좌표) 
			for (int m = -1; m <= 1; m++) { // 커널 센터중심 위(-1), 자신(0), 아래(1) 
				for (int n = -1; n <= 1; n++) {  //커널 센터중심 왼쪽(-1), 자신(0), 오른쪽(1) 
					SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1]; // 2차원 배열을 1차원 배열에 Mapping 하는 관용적 Syntax: Y(영상의 Y좌표) * W(영상의 가로사이즈) + X(영상의 X 좌표) 
				}
			}
			// 0~765 => 0~255
			Out[i * W + j] = abs((long)SumProduct) / 4;
			SumProduct = 0.0;
		}
	}
}

// Prewitt 마스크 Y
void Sobel_Y_Conv(BYTE* Img, BYTE* Out, int W, int H) {
	double Kernel[3][3] = {
		-1.0,-2.0,-1.0,
		0.0,0.0,0.0,
		1.0,2.0,1.0
	};
	double SumProduct = 0.0;
	for (int i = 1; i < H - 1; i++) { // 위,아래 마진을 고려, 영상의 세로 (Y좌표) 
		for (int j = 1; j < W - 1; j++) { // 영상의 가로 (X 좌표) 
			for (int m = -1; m <= 1; m++) { // 커널 센터중심 위(-1), 자신(0), 아래(1) 
				for (int n = -1; n <= 1; n++) {  //커널 센터중심 왼쪽(-1), 자신(0), 오른쪽(1) 
					SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1]; // 2차원 배열을 1차원 배열에 Mapping 하는 관용적 Syntax: Y(영상의 Y좌표) * W(영상의 가로사이즈) + X(영상의 X 좌표) 
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
	for (int i = 1; i < H - 1; i++) { // 위,아래 마진을 고려, 영상의 세로 (Y좌표) 
		for (int j = 1; j < W - 1; j++) { // 영상의 가로 (X 좌표) 
			for (int m = -1; m <= 1; m++) { // 커널 센터중심 위(-1), 자신(0), 아래(1) 
				for (int n = -1; n <= 1; n++) {  //커널 센터중심 왼쪽(-1), 자신(0), 오른쪽(1) 
					SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1]; // 2차원 배열을 1차원 배열에 Mapping 하는 관용적 Syntax: Y(영상의 Y좌표) * W(영상의 가로사이즈) + X(영상의 X 좌표) 
				}
			}
			// 0~2040 => 0~255
			Out[i * W + j] = abs((long)SumProduct) / 8;
			SumProduct = 0.0;
		}
	}
}

// 라플라시안 9, 밝기 유지 + 경계 노이즈 (고주파) 증폭 
void Laplace_Conv_DC(BYTE* Img, BYTE* Out, int W, int H) {
	double Kernel[3][3] = {
		-1.0, -1.0, -1.0,
		-1.0, 9.0, -1.0,
		-1.0, -1.0, -1.0
	};
	double SumProduct = 0.0;
	for (int i = 1; i < H - 1; i++) { // 위,아래 마진을 고려, 영상의 세로 (Y좌표) 
		for (int j = 1; j < W - 1; j++) { // 영상의 가로 (X 좌표) 
			for (int m = -1; m <= 1; m++) { // 커널 센터중심 위(-1), 자신(0), 아래(1) 
				for (int n = -1; n <= 1; n++) {  //커널 센터중심 왼쪽(-1), 자신(0), 오른쪽(1) 
					SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1]; // 2차원 배열을 1차원 배열에 Mapping 하는 관용적 Syntax: Y(영상의 Y좌표) * W(영상의 가로사이즈) + X(영상의 X 좌표) 
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
	BITMAPFILEHEADER hf; // BMP 파일헤더 14Bytes
	BITMAPINFOHEADER hInfo; // BMP 인포헤더 40Bytes
	RGBQUAD hRGB[256]; // 팔레트 (256 * 4Bytes)
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

	int Histo[256] = { 0 }; // 전체 픽셀에 해당하는 화소값을 카운트하는 Bucket
	int AHisto[256] = { 0 }; // 누적 Histo 

	// 영상처리
	//InverseImage(Image, Output, hInfo.biWidth,hInfo.biHeight);
	//BrightnessAdj(Image, Output, hInfo.biWidth,hInfo.biHeight,70);
	//ContrastAdj(Image, Output, hInfo.biWidth, hInfo.biHeight, 0.5);

	//ObtainHistogram(Image, Histo, hInfo.biWidth, hInfo.biHeight);
	//ObtainAHistogram(Histo, AHisto);  // 누적합을 구하는 함수 
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
	GaussAvrConv(Image, Temp, hInfo.biWidth, hInfo.biHeight); // 노이즈 제거
	Laplace_Conv_DC(Temp, Output, hInfo.biWidth, hInfo.biHeight);

	/* 히스토그램 출력
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