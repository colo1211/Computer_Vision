#pragma warning(disable:4996)
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <math.h>

void InverseImage(BYTE* Img, BYTE* Out, int W, int H)
{
	int ImgSize = W * H;
	for (int i = 0; i < ImgSize; i++)
	{
		Out[i] = 255 - Img[i];
	}
}
void BrightnessAdj(BYTE* Img, BYTE* Out, int W, int H, int Val)
{
	int ImgSize = W * H;
	for (int i = 0; i < ImgSize; i++)
	{
		if (Img[i] + Val > 255)
		{
			Out[i] = 255;
		}
		else if (Img[i] + Val < 0)
		{
			Out[i] = 0;
		}
		else    Out[i] = Img[i] + Val;
	}
}
void ContrastAdj(BYTE* Img, BYTE* Out, int W, int H, double Val)
{
	int ImgSize = W * H;
	for (int i = 0; i < ImgSize; i++)
	{
		if (Img[i] * Val > 255.0)
		{
			Out[i] = 255;
		}
		else    Out[i] = (BYTE)(Img[i] * Val);
	}
}

void ObtainHistogram(BYTE* Img, int* Histo, int W, int H)
{
	int ImgSize = W * H;
	for (int i = 0; i < ImgSize; i++) {
		Histo[Img[i]]++;
	}
}

void ObtainAHistogram(int* Histo, int* AHisto)
{
	for (int i = 0; i < 256; i++) {
		for (int j = 0; j <= i; j++) {
			AHisto[i] += Histo[j];
		}
	}
}

void HistogramStretching(BYTE* Img, BYTE* Out, int* Histo, int W, int H)
{
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
void HistogramEqualization(BYTE* Img, BYTE* Out, int* AHisto, int W, int H)
{
	int ImgSize = W * H;
	int Nt = W * H, Gmax = 255;
	double Ratio = Gmax / (double)Nt;
	BYTE NormSum[256];
	for (int i = 0; i < 256; i++) {
		NormSum[i] = (BYTE)(Ratio * AHisto[i]);
	}
	for (int i = 0; i < ImgSize; i++)
	{
		Out[i] = NormSum[Img[i]];
	}
}

void Binarization(BYTE* Img, BYTE* Out, int W, int H, BYTE Threshold)
{
	int ImgSize = W * H;
	for (int i = 0; i < ImgSize; i++) {
		if (Img[i] < Threshold) Out[i] = 0;
		else Out[i] = 255;
	}
}

//int GozalezBinThresh()
//{
//
//}

void AverageConv(BYTE* Img, BYTE* Out, int W, int H) // 박스평활화
{
	double Kernel[3][3] = { 0.11111, 0.11111, 0.11111,
							   0.11111, 0.11111, 0.11111,
							   0.11111, 0.11111, 0.11111 };
	double SumProduct = 0.0;
	for (int i = 1; i < H - 1; i++) { // Y좌표 (행)
		for (int j = 1; j < W - 1; j++) { // X좌표 (열)
			for (int m = -1; m <= 1; m++) { // Kernel 행
				for (int n = -1; n <= 1; n++) { // Kernel 열
					SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1];
				}
			}
			Out[i * W + j] = (BYTE)SumProduct;
			SumProduct = 0.0;
		}
	}
}

void GaussAvrConv(BYTE* Img, BYTE* Out, int W, int H) // 가우시안평활화
{
	double Kernel[3][3] = { 0.0625, 0.125, 0.0625,
							   0.125, 0.25, 0.125,
							   0.0625, 0.125, 0.0625 };
	double SumProduct = 0.0;
	for (int i = 1; i < H - 1; i++) { // Y좌표 (행)
		for (int j = 1; j < W - 1; j++) { // X좌표 (열)
			for (int m = -1; m <= 1; m++) { // Kernel 행
				for (int n = -1; n <= 1; n++) { // Kernel 열
					SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1];
				}
			}
			Out[i * W + j] = (BYTE)SumProduct;
			SumProduct = 0.0;
		}
	}
}

void Prewitt_X_Conv(BYTE* Img, BYTE* Out, int W, int H) // Prewitt 마스크 X
{
	double Kernel[3][3] = { -1.0, 0.0, 1.0,
							   -1.0, 0.0, 1.0,
							   -1.0, 0.0, 1.0 };
	double SumProduct = 0.0;
	for (int i = 1; i < H - 1; i++) { // Y좌표 (행)
		for (int j = 1; j < W - 1; j++) { // X좌표 (열)
			for (int m = -1; m <= 1; m++) { // Kernel 행
				for (int n = -1; n <= 1; n++) { // Kernel 열
					SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1];
				}
			}
			// 0 ~ 765  =====> 0 ~ 255
			Out[i * W + j] = abs((long)SumProduct) / 3;
			SumProduct = 0.0;
		}
	}
}

void Prewitt_Y_Conv(BYTE* Img, BYTE* Out, int W, int H) // Prewitt 마스크 X
{
	double Kernel[3][3] = { -1.0, -1.0, -1.0,
							   0.0, 0.0, 0.0,
							   1.0, 1.0, 1.0 };
	double SumProduct = 0.0;
	for (int i = 1; i < H - 1; i++) { // Y좌표 (행)
		for (int j = 1; j < W - 1; j++) { // X좌표 (열)
			for (int m = -1; m <= 1; m++) { // Kernel 행
				for (int n = -1; n <= 1; n++) { // Kernel 열
					SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1];
				}
			}
			// 0 ~ 765  =====> 0 ~ 255
			Out[i * W + j] = abs((long)SumProduct) / 3;
			SumProduct = 0.0;
		}
	}
}

void Sobel_X_Conv(BYTE* Img, BYTE* Out, int W, int H) // Prewitt 마스크 X
{
	double Kernel[3][3] = { -1.0, 0.0, 1.0,
							   -2.0, 0.0, 2.0,
							   -1.0, 0.0, 1.0 };
	double SumProduct = 0.0;
	for (int i = 1; i < H - 1; i++) { // Y좌표 (행)
		for (int j = 1; j < W - 1; j++) { // X좌표 (열)
			for (int m = -1; m <= 1; m++) { // Kernel 행
				for (int n = -1; n <= 1; n++) { // Kernel 열
					SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1];
				}
			}
			// 0 ~ 1020  =====> 0 ~ 255
			Out[i * W + j] = abs((long)SumProduct) / 4;
			SumProduct = 0.0;
		}
	}
}

void Sobel_Y_Conv(BYTE* Img, BYTE* Out, int W, int H) // Prewitt 마스크 X
{
	double Kernel[3][3] = { -1.0, -2.0, -1.0,
							   0.0, 0.0, 0.0,
							   1.0, 2.0, 1.0 };
	double SumProduct = 0.0;
	for (int i = 1; i < H - 1; i++) { // Y좌표 (행)
		for (int j = 1; j < W - 1; j++) { // X좌표 (열)
			for (int m = -1; m <= 1; m++) { // Kernel 행
				for (int n = -1; n <= 1; n++) { // Kernel 열
					SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1];
				}
			}
			// 0 ~ 765  =====> 0 ~ 255
			Out[i * W + j] = abs((long)SumProduct) / 4;
			SumProduct = 0.0;
		}
	}
}

void Laplace_Conv(BYTE* Img, BYTE* Out, int W, int H) // Prewitt 마스크 X
{
	double Kernel[3][3] = { -1.0, -1.0, -1.0,
							   -1.0, 8.0, -1.0,
							   -1.0, -1.0, -1.0 };
	double SumProduct = 0.0;
	for (int i = 1; i < H - 1; i++) { // Y좌표 (행)
		for (int j = 1; j < W - 1; j++) { // X좌표 (열)
			for (int m = -1; m <= 1; m++) { // Kernel 행
				for (int n = -1; n <= 1; n++) { // Kernel 열
					SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1];
				}
			}
			// 0 ~ 2040  =====> 0 ~ 255
			Out[i * W + j] = abs((long)SumProduct) / 8;
			SumProduct = 0.0;
		}
	}
}

void Laplace_Conv_DC(BYTE* Img, BYTE* Out, int W, int H) // Prewitt 마스크 X
{
	double Kernel[3][3] = { -1.0, -1.0, -1.0,
							   -1.0, 9.0, -1.0,
							   -1.0, -1.0, -1.0 };
	double SumProduct = 0.0;
	for (int i = 1; i < H - 1; i++) { // Y좌표 (행)
		for (int j = 1; j < W - 1; j++) { // X좌표 (열)
			for (int m = -1; m <= 1; m++) { // Kernel 행
				for (int n = -1; n <= 1; n++) { // Kernel 열
					SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1];
				}
			}
			//Out[i * W + j] = abs((long)SumProduct) / 8;
			if (SumProduct > 255.0) Out[i * W + j] = 255;
			else if (SumProduct < 0.0) Out[i * W + j] = 0;
			else Out[i * W + j] = (BYTE)SumProduct;
			SumProduct = 0.0;
		}
	}
}

void SaveBMPFile(BITMAPFILEHEADER hf, BITMAPINFOHEADER hInfo,
	RGBQUAD* hRGB, BYTE* Output, int W, int H, const char* FileName)
{
	FILE* fp = fopen(FileName, "wb");
	fwrite(&hf, sizeof(BYTE), sizeof(BITMAPFILEHEADER), fp);
	fwrite(&hInfo, sizeof(BYTE), sizeof(BITMAPINFOHEADER), fp);
	fwrite(hRGB, sizeof(RGBQUAD), 256, fp);
	fwrite(Output, sizeof(BYTE), W * H, fp);
	fclose(fp);
}

void swap(BYTE* a, BYTE* b)
{
	BYTE temp = *a;
	*a = *b;
	*b = temp;
}

BYTE Median(BYTE* arr, int size)
{
	// 오름차순 정렬
	const int S = size;
	for (int i = 0; i < size - 1; i++) // pivot index
	{
		for (int j = i + 1; j < size; j++) // 비교대상 index
		{
			if (arr[i] > arr[j])    swap(&arr[i], &arr[j]);
		}
	}
	return arr[S / 2];
}

BYTE MaxPooling(BYTE* arr, int size)
{
	// 오름차순 정렬
	const int S = size;
	for (int i = 0; i < size - 1; i++) // pivot index
	{
		for (int j = i + 1; j < size; j++) // 비교대상 index
		{
			if (arr[i] > arr[j])    swap(&arr[i], &arr[j]);
		}
	}
	return arr[S - 1];
}

BYTE MinPooling(BYTE* arr, int size)
{
	// 오름차순 정렬
	const int S = size;
	for (int i = 0; i < size - 1; i++) // pivot index
	{
		for (int j = i + 1; j < size; j++) // 비교대상 index
		{
			if (arr[i] > arr[j])    swap(&arr[i], &arr[j]);
		}
	}
	return arr[0];
}

int push(short* stackx, short* stacky, int arr_size, short vx, short vy, int* top)
{
	if (*top >= arr_size) return(-1);
	(*top)++;
	stackx[*top] = vx;
	stacky[*top] = vy;
	return(1);
}

int pop(short* stackx, short* stacky, short* vx, short* vy, int* top)
{
	if (*top == 0) return(-1);
	*vx = stackx[*top];
	*vy = stacky[*top];
	(*top)--;
	return(1);
}

// 라벨링을 진행할 때 호출 할 함수  -> 내부에서 push, pop 함수를 호출한다. 
// GlassFire 알고리즘을 이용한 라벨링 함수
void m_BlobColoring(BYTE* CutImage, int height, int width)
{
	int i, j, m, n, top, area, Out_Area, index, BlobArea[1000];
	long k;
	short curColor = 0, r, c;
	//   BYTE** CutImage2;
	Out_Area = 1;


	// 스택으로 사용할 메모리 할당
	short* stackx = new short[height * width];
	short* stacky = new short[height * width];
	short* coloring = new short[height * width];

	int arr_size = height * width;

	// 라벨링된 픽셀을 저장하기 위해 메모리 할당

	for (k = 0; k < height * width; k++) coloring[k] = 0;  // 메모리 초기화

	for (i = 0; i < height; i++)
	{
		index = i * width;
		for (j = 0; j < width; j++)
		{
			// 이미 방문한 점이거나 픽셀값이 255가 아니라면 처리 안함
			if (coloring[index + j] != 0 || CutImage[index + j] != 255) continue;
			r = i; c = j; top = 0; area = 1;
			curColor++;

			while (1)
			{
			GRASSFIRE:
				for (m = r - 1; m <= r + 1; m++)
				{
					index = m * width;
					for (n = c - 1; n <= c + 1; n++)
					{
						//관심 픽셀이 영상경계를 벗어나면 처리 안함
						if (m < 0 || m >= height || n < 0 || n >= width) continue;

						if ((int)CutImage[index + n] == 255 && coloring[index + n] == 0)
						{
							coloring[index + n] = curColor; // 현재 라벨로 마크
							if (push(stackx, stacky, arr_size, (short)m, (short)n, &top) == -1) continue;
							r = m; c = n; area++;
							goto GRASSFIRE;
						}
					}
				}
				if (pop(stackx, stacky, &r, &c, &top) == -1) break;
			}
			if (curColor < 1000) BlobArea[curColor] = area;
		}
	}

	float grayGap = 255.0f / (float)curColor; // curColor : 총 Component 개수(약 25개) 255.0f/25 = 10.xxxx 
	//=> 10.xx px 간격으로 Component의 밝기값을 Filling 해준다. 


	// 라벨링 하는 것 뿐만 아니라 Component 중 가장 큰 것을 찾아서 가장 큰 것만 출력을 해준다. 

	// 가장 면적이 넓은 영역을 찾아내기 위함 , BlobArea 는 각 컴퍼넌트들의 면적정보가 저장되어 있다. 
	for (i = 1; i <= curColor; i++)
	{
		if (BlobArea[i] >= BlobArea[Out_Area]) Out_Area = i;
	}
	// CutImage 배열 클리어~
	for (k = 0; k < width * height; k++) CutImage[k] = 255; // 메인함수에서 Output을 CutImage로 전달받음. 전부다 255(하얗게)로 채운다. 

	// coloring에 저장된 라벨링 결과중 (Out_Area에 저장된) 영역이 가장 큰 것만 CutImage에 저장
	for (k = 0; k < width * height; k++)
	{
		if (coloring[k] == Out_Area) CutImage[k] = 0;  // Size Filtering : 가장 큰 Component만 0(검게)으로 채운다. 
		//if (BlobArea[coloring[k]] > 500) CutImage[k] = 0;  // 특정 면적이상 되는 영역만 출력
		//CutImage[k] = (unsigned char)(coloring[k] * grayGap); // 라벨링된 결과를 그대로 모두 출력, 
		// coloring 배열은 Index정보가 들어가있음. 컴포넌트 숫자가 들어가있음(동일한 영역인지 판별), 1번영역인지, 2번영역인지
		// BlobArea 배열은 각 컴퍼넌트들의 면적정보가 저장되어 있다
		// 배경이 255인 상태에서 각각의 Component들을 밝기값으로 채워나가는 과정 
	}

	delete[] coloring;
	delete[] stackx;
	delete[] stacky;
}
// 라벨링 후 가장 넓은 영역에 대해서만 뽑아내는 코드 포함

void BinImageEdgeDetection(BYTE* Bin, BYTE* Out, int W, int H) {
	for (int i = 0; i < H; i++) { // 경계만 출력
		for (int j = 0; j < W; j++) {
			if (Bin[i * W + j] == 0) { // 전경화소라면, 
				if (!(Bin[(i - 1) * W + j] == 0 && Bin[(i + 1) * W + j] == 0 // 4방향 검사
					&& Bin[i * W + j - 1] == 0 && Bin[i * W + j + 1] == 0)) {
					Out[i * W + j] = 255;
				}
			}
		}
	}
}



// 동공의 x1, x2, y1, y2를 찾아주는 함수 
void FindXandY(BYTE* Img, int W, int H, int* x1, int* x2, int* y1, int* y2) {
	for (int i = 0; i < H; i++) { // y축
		for (int j = 0; j < W; j++) { // x축 
			if (Img[i * W + j] == 0) {
				*y2 = i;
			}
		}
	}
	for (int i = H - 1; i >= 0; i--) { // y축 
		for (int j = W - 1; j >= 0; j--) { // x축 
			if (Img[i * W + j] == 0) {
				*y1 = i;
			}
		}
	}
	for (int j = 0; j < W; j++) {
		for (int i = 0; i < H; i++) {
			if (Img[i * W + j] == 0) {
				*x2 = j;
			}
		}
	}
	for (int j = W - 1; j >= 0; j--) {
		for (int i = H - 1; i >= 0; i--) {
			if (Img[i * W + j] == 0) {
				*x1 = j;
			}
		}
	}
	printf("x1:%d, x2:%d, y1:%d, y2:%d\n", *x1, *x2, *y1, *y2);
}

// 동공의 외접 사각형을 그려주는 함수 
void DrawRectOutline(BYTE* Img, int W, int H, int x1, int x2, int y1, int y2) {
	for (int x = x1; x <= x2; x++) {
		Img[y1 * W + x] = 255;
	}
	for (int y = y1; y <= y2; y++) {
		Img[y * W + x2] = 255;
	}
	for (int x = x1; x <= x2; x++) {
		Img[y2 * W + x] = 255;
	}
	for (int y = y1; y <= y2; y++) {
		Img[y * W + x1] = 255;
	}
}

// 동공의 무게중심을 구하는 함수 
void CenterOfPupil(BYTE* Image, int* cen_x, int* cen_y, int W, int H) {
	int sum_x = 0; int sum_y = 0;
	int count = 0;
	for (int y = 0; y < H; y++) {
		for (int x = 0; x < W; x++) {
			if (Image[y * W + x] == 0) {
				sum_x += x;
				sum_y += y;
				count++;
			}
		}
	}
	*cen_x = (int)sum_x / count;
	*cen_y = (int)sum_y / count;
}

// 동공의 크로스라인을 그려주는 함수 
void DrawCrossLine(BYTE* Image, int W, int H, int Cx, int Cy) { 
	for (int y = 0; y < H; y++) { // 동공의 중심 x축(고정)을 중심으로 해당 y축을 모두 Line을 그린다. 
		Image[y * W + Cx] = 255;
	}
	for (int x = 0; x < W; x++) { // 동공의 중심 y축(고정)을 중심으로 해당 x축을 모두 Line을 그린다. 
		Image[Cy * W + x] = 255;
	}
}

// 위아래 Filp 
void VerticalFlip(BYTE * Img, int W, int H) {
	for (int i = 0; i < H/2; i++) { 
		for (int j = 0; j < W; j++) {
			swap(&Img[i*W+j], &Img[(H-1-i)*W+j]);
		}
	}
}
// 좌우 Flip 
void HorizontalFlip(BYTE* Img, int W, int H) {
	for (int i = 0; i < W/2 ; i++) {  // x좌표 
		for (int j = 0; j < H; j++) { // y좌표
			swap(&Img[j * W + i], &Img[(j * W + (W - 1 - i))]);
		}
	}
}

void Translation(BYTE * Image, BYTE* Output, int W, int H, int Tx, int Ty) {
	Ty *= -1; // 0,0이 원점(좌상) 일때(원래 이미지 좌표대로 하려면 처리)  
	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			if ((0 <= i + Ty && i + Ty < H) && (0 <= j + Tx && j + Tx < W)) {
				Output[(i + Ty) * W + (j + Tx)] = Image[i * W + j];
			}
		}
	}
}

// 스케일링 
void Scaling(BYTE *Image, BYTE * Output, int W, int H, double SF_X, double SF_Y ) {
	int tempX, tempY;
	for (int i = 0; i < H; i++) { // Y
		for (int j = 0; j < W; j++) { // X 
			tempX = (int)(j / SF_X); // 역방향 사상  
			tempY = (int)(i / SF_Y); // 역방향 사상 
			// tempY = (int) (i*SF_Y); // 순방향 사상
			// tempX = (int) (j*SF_X); // 순방향 사상 
			if (tempY < H && tempX < W)
				//Output[tempY * W + tempX] = Image[i * W + j]; // 순방향 사상, Hole이 생긴다. 
				Output[i * W + j] = Image[tempY * W + tempX]; // 역방향 사상, Hole 해결, Output에 해당하는 값을 Input에서부터 역으로 가져온다. 
		}
	}
}

// 회전 
void Rotation(BYTE * Image, BYTE * Output, int W, int H, int angle) {
	double radian = (3.141592 / 180.0) * angle; // 각도를 라디안으로 변경 
	int tempX, tempY;
	for (int i = 0; i < H; i++) { // Y
		for (int j = 0; j < W; j++) { // X 
			//tempX = (int)(cos(radian) * j - sin(radian) * i); // 시계 반대방향 회전 공식, 순방향 사상  
			//tempY = (int)(sin(radian) * j + cos(radian) * i); // 시계 반대방향 회전 공식, 순방향 사상
			tempX = (int)(cos(radian) * j + sin(radian) * i); // 시계 방향 회전 공식, 역방향 사상  
			tempY = (int)(-sin(radian) * j + cos(radian) * i); // 시계 방향 회전 공식, 역방향 사상
			if ((tempY < H && tempY >= 0) && (tempX < W && tempX >= 0))
				//Output[tempY * W + tempX] = Image[i * W + j]; // 순방향 사상
				Output[i * W + j] = Image[tempY * W + tempX]; // 역방향 사상 
		}
	}
}
 

int main()
{
	BITMAPFILEHEADER hf; // 14바이트
	BITMAPINFOHEADER hInfo; // 40바이트
	RGBQUAD hRGB[256]; // 1024바이트
	FILE* fp;
	fp = fopen("LENNA.bmp", "rb");
	if (fp == NULL) {
		printf("File not found!\n");
		return -1;
	}
	fread(&hf, sizeof(BITMAPFILEHEADER), 1, fp);
	fread(&hInfo, sizeof(BITMAPINFOHEADER), 1, fp);
	fread(hRGB, sizeof(RGBQUAD), 256, fp);
	int ImgSize = hInfo.biWidth * hInfo.biHeight;
	BYTE* Image = (BYTE*)malloc(ImgSize);
	BYTE* Temp = (BYTE*)malloc(ImgSize); // 임시배열
	BYTE* Output = (BYTE*)malloc(ImgSize);
	fread(Image, sizeof(BYTE), ImgSize, fp);
	fclose(fp);

	int H = hInfo.biHeight;
	int W = hInfo.biWidth;

	int Histo[256] = { 0 };
	int AHisto[256] = { 0 };

	/*ObtainHistogram(Image, Histo, hInfo.biWidth, hInfo.biHeight);
	ObtainAHistogram(Histo, AHisto);
	HistogramEqualization(Image, Output, AHisto, hInfo.biWidth, hInfo.biHeight);*/
	/*int Thres = GozalezBinThresh();
	Binarization(Image, Output, hInfo.biWidth, hInfo.biHeight, Thres);*/

	//GaussAvrConv(Image, Output, hInfo.biWidth, hInfo.biHeight);

	/*Sobel_X_Conv(Image, Temp, hInfo.biWidth, hInfo.biHeight);
	Sobel_Y_Conv(Image, Output, hInfo.biWidth, hInfo.biHeight);
	for (int i = 0; i < ImgSize; i++) {
	   if (Temp[i] > Output[i])    Output[i] = Temp[i];
	}
	Binarization(Output, Output, hInfo.biWidth, hInfo.biHeight, 40);*/
	/*GaussAvrConv(Image, Temp, hInfo.biWidth, hInfo.biHeight);
	Laplace_Conv_DC(Temp, Output, hInfo.biWidth, hInfo.biHeight);*/

	//HistogramStretching(Image, Output, Histo, hInfo.biWidth, hInfo.biHeight);
	//InverseImage(Image, Output, hInfo.biWidth, hInfo.biHeight);
	//BrightnessAdj(Image, Output, hInfo.biWidth, hInfo.biHeight, 70);
	//ContrastAdj(Image, Output, hInfo.biWidth, hInfo.biHeight, 0.5);

	/* Median filtering */
	//BYTE temp[9];
	//int W = hInfo.biWidth, H = hInfo.biHeight;
	//int i, j;
	//for (i = 1; i < H - 1; i++) {
	//   for (j = 1; j < W - 1; j++) {
	//      temp[0] = Image[(i - 1) * W + j-1];
	//      temp[1] = Image[(i - 1) * W + j];
	//      temp[2] = Image[(i - 1) * W + j+1];
	//      temp[3] = Image[i * W + j-1];
	//      temp[4] = Image[i * W + j];
	//      temp[5] = Image[i * W + j+1];
	//      temp[6] = Image[(i + 1) * W + j-1];
	//      temp[7] = Image[(i + 1) * W + j];
	//      temp[8] = Image[(i + 1) * W + j+1];
	//      Output[i * W + j] = Median(temp, 9);
	//      //Output[i * W + j] = MaxPooling(temp, 9);
	//      //Output[i * W + j] = MinPooling(temp, 9);
	//   }
	//}
	/* Median filtering */
	//AverageConv(Image, Output, hInfo.biWidth, hInfo.biHeight);


	//Binarization(Image, Temp, W, H, 50); // 이진화, 결과: 녹슬어 있는 동전은 완전하게 이진화가 되지 않음 -> 서로 떨어져있는 Object로 인식 
	//InverseImage(Temp, Temp, W, H); // Coin은 배경 (어두움) 전경 (밝음) , Pupil은 배경 (밝음) 전경 (어두움) => 따라서, Inverse 진행
	//m_BlobColoring(Temp, H, W); // Component가 서로 다른 화소 밝기값을 가지게 된다. 미세한 컴포넌트 포함 약 25개가 생성된다. 
	//for (int i = 0; i < ImgSize; i++) Output[i] = Image[i]; // 원본 영상 복사, Output[i] = 255; 하면 흰 화면에 경곗값만 나온다. 
	//BinImageEdgeDetection(Temp, Output, W, H);

	//int x1 = 0, x2 = 0, y1 = 0, y2 = 0;

	//// 외접사각형을 위한 좌표를 찾기위한 함수 호출 
	//FindXandY(Temp, W, H, &x1, &x2, &y1, &y2); // Call By Reference 
	//printf("x1:%d, x2:%d, y1:%d, y2:%d\n", x1, x2, y1, y2); 

	//// 외접사각형 그리기 함수 호출 
	//DrawRectOutline(Output, W, H, x1, x2, y1, y2);

	//// 동공의 무게중심을 찾기 위한 좌표 초기화    
	//int cen_x = 0; int cen_y = 0;


	//// 동공의 무게중심 좌표 구하는 방법 1 (과제 제시) 
	//CenterOfPupil(Temp, &cen_x, &cen_y, W, H); // Call By Reference
 //   printf("pupil_x:%d,pupil_y:%d", cen_x, cen_y);
	//
	//// 동공의 무게중심 좌표 구하는 방법 2 (간단 ver) 
	//// FindXandY를 통해서 구한 x1,x2,y1,y2를 활용, 
	//// cen_x= (x1+x2)/2, cen_y= (y1+y2)/2 를 DrawCrossLine의 인자(Cx, Cy)로 넣어주면 된다. 

	//// 동공의 무게중심 기준 크로스 라인 그리기 
	//DrawCrossLine(Output, W, H, cen_x, cen_y);
	
	VerticalFlip(Image, W, H);
	//HorizontalFlip(Image, W, H);
	//Translation(Image, Output, W, H, 50, 30);

	
	//Scaling(Image, Output, W,H, 0.7,0.7); // Uniform Scaling: 확대비율 동일

	//Rotation(Image, Output, W,H,-45);
	SaveBMPFile(hf, hInfo, hRGB, Output, W, H, "rotation_vertical.bmp");

	free(Image);
	free(Output);
	free(Temp);
	return 0;
}