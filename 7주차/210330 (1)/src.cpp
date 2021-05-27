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

void AverageConv(BYTE* Img, BYTE* Out, int W, int H) // �ڽ���Ȱȭ
{
	double Kernel[3][3] = { 0.11111, 0.11111, 0.11111,
							   0.11111, 0.11111, 0.11111,
							   0.11111, 0.11111, 0.11111 };
	double SumProduct = 0.0;
	for (int i = 1; i < H - 1; i++) { // Y��ǥ (��)
		for (int j = 1; j < W - 1; j++) { // X��ǥ (��)
			for (int m = -1; m <= 1; m++) { // Kernel ��
				for (int n = -1; n <= 1; n++) { // Kernel ��
					SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1];
				}
			}
			Out[i * W + j] = (BYTE)SumProduct;
			SumProduct = 0.0;
		}
	}
}

void GaussAvrConv(BYTE* Img, BYTE* Out, int W, int H) // ����þ���Ȱȭ
{
	double Kernel[3][3] = { 0.0625, 0.125, 0.0625,
							   0.125, 0.25, 0.125,
							   0.0625, 0.125, 0.0625 };
	double SumProduct = 0.0;
	for (int i = 1; i < H - 1; i++) { // Y��ǥ (��)
		for (int j = 1; j < W - 1; j++) { // X��ǥ (��)
			for (int m = -1; m <= 1; m++) { // Kernel ��
				for (int n = -1; n <= 1; n++) { // Kernel ��
					SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1];
				}
			}
			Out[i * W + j] = (BYTE)SumProduct;
			SumProduct = 0.0;
		}
	}
}

void Prewitt_X_Conv(BYTE* Img, BYTE* Out, int W, int H) // Prewitt ����ũ X
{
	double Kernel[3][3] = { -1.0, 0.0, 1.0,
							   -1.0, 0.0, 1.0,
							   -1.0, 0.0, 1.0 };
	double SumProduct = 0.0;
	for (int i = 1; i < H - 1; i++) { // Y��ǥ (��)
		for (int j = 1; j < W - 1; j++) { // X��ǥ (��)
			for (int m = -1; m <= 1; m++) { // Kernel ��
				for (int n = -1; n <= 1; n++) { // Kernel ��
					SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1];
				}
			}
			// 0 ~ 765  =====> 0 ~ 255
			Out[i * W + j] = abs((long)SumProduct) / 3;
			SumProduct = 0.0;
		}
	}
}

void Prewitt_Y_Conv(BYTE* Img, BYTE* Out, int W, int H) // Prewitt ����ũ X
{
	double Kernel[3][3] = { -1.0, -1.0, -1.0,
							   0.0, 0.0, 0.0,
							   1.0, 1.0, 1.0 };
	double SumProduct = 0.0;
	for (int i = 1; i < H - 1; i++) { // Y��ǥ (��)
		for (int j = 1; j < W - 1; j++) { // X��ǥ (��)
			for (int m = -1; m <= 1; m++) { // Kernel ��
				for (int n = -1; n <= 1; n++) { // Kernel ��
					SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1];
				}
			}
			// 0 ~ 765  =====> 0 ~ 255
			Out[i * W + j] = abs((long)SumProduct) / 3;
			SumProduct = 0.0;
		}
	}
}

void Sobel_X_Conv(BYTE* Img, BYTE* Out, int W, int H) // Prewitt ����ũ X
{
	double Kernel[3][3] = { -1.0, 0.0, 1.0,
							   -2.0, 0.0, 2.0,
							   -1.0, 0.0, 1.0 };
	double SumProduct = 0.0;
	for (int i = 1; i < H - 1; i++) { // Y��ǥ (��)
		for (int j = 1; j < W - 1; j++) { // X��ǥ (��)
			for (int m = -1; m <= 1; m++) { // Kernel ��
				for (int n = -1; n <= 1; n++) { // Kernel ��
					SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1];
				}
			}
			// 0 ~ 1020  =====> 0 ~ 255
			Out[i * W + j] = abs((long)SumProduct) / 4;
			SumProduct = 0.0;
		}
	}
}

void Sobel_Y_Conv(BYTE* Img, BYTE* Out, int W, int H) // Prewitt ����ũ X
{
	double Kernel[3][3] = { -1.0, -2.0, -1.0,
							   0.0, 0.0, 0.0,
							   1.0, 2.0, 1.0 };
	double SumProduct = 0.0;
	for (int i = 1; i < H - 1; i++) { // Y��ǥ (��)
		for (int j = 1; j < W - 1; j++) { // X��ǥ (��)
			for (int m = -1; m <= 1; m++) { // Kernel ��
				for (int n = -1; n <= 1; n++) { // Kernel ��
					SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1];
				}
			}
			// 0 ~ 765  =====> 0 ~ 255
			Out[i * W + j] = abs((long)SumProduct) / 4;
			SumProduct = 0.0;
		}
	}
}

void Laplace_Conv(BYTE* Img, BYTE* Out, int W, int H) // Prewitt ����ũ X
{
	double Kernel[3][3] = { -1.0, -1.0, -1.0,
							   -1.0, 8.0, -1.0,
							   -1.0, -1.0, -1.0 };
	double SumProduct = 0.0;
	for (int i = 1; i < H - 1; i++) { // Y��ǥ (��)
		for (int j = 1; j < W - 1; j++) { // X��ǥ (��)
			for (int m = -1; m <= 1; m++) { // Kernel ��
				for (int n = -1; n <= 1; n++) { // Kernel ��
					SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1];
				}
			}
			// 0 ~ 2040  =====> 0 ~ 255
			Out[i * W + j] = abs((long)SumProduct) / 8;
			SumProduct = 0.0;
		}
	}
}

void Laplace_Conv_DC(BYTE* Img, BYTE* Out, int W, int H) // Prewitt ����ũ X
{
	double Kernel[3][3] = { -1.0, -1.0, -1.0,
							   -1.0, 9.0, -1.0,
							   -1.0, -1.0, -1.0 };
	double SumProduct = 0.0;
	for (int i = 1; i < H - 1; i++) { // Y��ǥ (��)
		for (int j = 1; j < W - 1; j++) { // X��ǥ (��)
			for (int m = -1; m <= 1; m++) { // Kernel ��
				for (int n = -1; n <= 1; n++) { // Kernel ��
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
	// �������� ����
	const int S = size;
	for (int i = 0; i < size - 1; i++) // pivot index
	{
		for (int j = i + 1; j < size; j++) // �񱳴�� index
		{
			if (arr[i] > arr[j])    swap(&arr[i], &arr[j]);
		}
	}
	return arr[S / 2];
}

BYTE MaxPooling(BYTE* arr, int size)
{
	// �������� ����
	const int S = size;
	for (int i = 0; i < size - 1; i++) // pivot index
	{
		for (int j = i + 1; j < size; j++) // �񱳴�� index
		{
			if (arr[i] > arr[j])    swap(&arr[i], &arr[j]);
		}
	}
	return arr[S - 1];
}

BYTE MinPooling(BYTE* arr, int size)
{
	// �������� ����
	const int S = size;
	for (int i = 0; i < size - 1; i++) // pivot index
	{
		for (int j = i + 1; j < size; j++) // �񱳴�� index
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

// �󺧸��� ������ �� ȣ�� �� �Լ�  -> ���ο��� push, pop �Լ��� ȣ���Ѵ�. 
// GlassFire �˰����� �̿��� �󺧸� �Լ�
void m_BlobColoring(BYTE* CutImage, int height, int width)
{
	int i, j, m, n, top, area, Out_Area, index, BlobArea[1000];
	long k;
	short curColor = 0, r, c;
	//   BYTE** CutImage2;
	Out_Area = 1;


	// �������� ����� �޸� �Ҵ�
	short* stackx = new short[height * width];
	short* stacky = new short[height * width];
	short* coloring = new short[height * width];

	int arr_size = height * width;

	// �󺧸��� �ȼ��� �����ϱ� ���� �޸� �Ҵ�

	for (k = 0; k < height * width; k++) coloring[k] = 0;  // �޸� �ʱ�ȭ

	for (i = 0; i < height; i++)
	{
		index = i * width;
		for (j = 0; j < width; j++)
		{
			// �̹� �湮�� ���̰ų� �ȼ����� 255�� �ƴ϶�� ó�� ����
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
						//���� �ȼ��� �����踦 ����� ó�� ����
						if (m < 0 || m >= height || n < 0 || n >= width) continue;

						if ((int)CutImage[index + n] == 255 && coloring[index + n] == 0)
						{
							coloring[index + n] = curColor; // ���� �󺧷� ��ũ
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

	float grayGap = 255.0f / (float)curColor; // curColor : �� Component ����(�� 25��) 255.0f/25 = 10.xxxx 
	//=> 10.xx px �������� Component�� ��Ⱚ�� Filling ���ش�. 


	// �󺧸� �ϴ� �� �Ӹ� �ƴ϶� Component �� ���� ū ���� ã�Ƽ� ���� ū �͸� ����� ���ش�. 

	// ���� ������ ���� ������ ã�Ƴ��� ���� , BlobArea �� �� ���۳�Ʈ���� ���������� ����Ǿ� �ִ�. 
	for (i = 1; i <= curColor; i++)
	{
		if (BlobArea[i] >= BlobArea[Out_Area]) Out_Area = i;
	}
	// CutImage �迭 Ŭ����~
	for (k = 0; k < width * height; k++) CutImage[k] = 255; // �����Լ����� Output�� CutImage�� ���޹���. ���δ� 255(�Ͼ��)�� ä���. 

	// coloring�� ����� �󺧸� ����� (Out_Area�� �����) ������ ���� ū �͸� CutImage�� ����
	for (k = 0; k < width * height; k++)
	{
		if (coloring[k] == Out_Area) CutImage[k] = 0;  // Size Filtering : ���� ū Component�� 0(�˰�)���� ä���. 
		//if (BlobArea[coloring[k]] > 500) CutImage[k] = 0;  // Ư�� �����̻� �Ǵ� ������ ���
		//CutImage[k] = (unsigned char)(coloring[k] * grayGap); // �󺧸��� ����� �״�� ��� ���, 
		// coloring �迭�� Index������ ������. ������Ʈ ���ڰ� ������(������ �������� �Ǻ�), 1����������, 2����������
		// BlobArea �迭�� �� ���۳�Ʈ���� ���������� ����Ǿ� �ִ�
		// ����� 255�� ���¿��� ������ Component���� ��Ⱚ���� ä�������� ���� 
	}

	delete[] coloring;
	delete[] stackx;
	delete[] stacky;
}
// �󺧸� �� ���� ���� ������ ���ؼ��� �̾Ƴ��� �ڵ� ����

void BinImageEdgeDetection(BYTE* Bin, BYTE* Out, int W, int H) {
	for (int i = 0; i < H; i++) { // ��踸 ���
		for (int j = 0; j < W; j++) {
			if (Bin[i * W + j] == 0) { // ����ȭ�Ҷ��, 
				if (!(Bin[(i - 1) * W + j] == 0 && Bin[(i + 1) * W + j] == 0 // 4���� �˻�
					&& Bin[i * W + j - 1] == 0 && Bin[i * W + j + 1] == 0)) {
					Out[i * W + j] = 255;
				}
			}
		}
	}
}



// ������ x1, x2, y1, y2�� ã���ִ� �Լ� 
void FindXandY(BYTE* Img, int W, int H, int* x1, int* x2, int* y1, int* y2) {
	for (int i = 0; i < H; i++) { // y��
		for (int j = 0; j < W; j++) { // x�� 
			if (Img[i * W + j] == 0) {
				*y2 = i;
			}
		}
	}
	for (int i = H - 1; i >= 0; i--) { // y�� 
		for (int j = W - 1; j >= 0; j--) { // x�� 
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

// ������ ���� �簢���� �׷��ִ� �Լ� 
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

// ������ �����߽��� ���ϴ� �Լ� 
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

// ������ ũ�ν������� �׷��ִ� �Լ� 
void DrawCrossLine(BYTE* Image, int W, int H, int Cx, int Cy) { 
	for (int y = 0; y < H; y++) { // ������ �߽� x��(����)�� �߽����� �ش� y���� ��� Line�� �׸���. 
		Image[y * W + Cx] = 255;
	}
	for (int x = 0; x < W; x++) { // ������ �߽� y��(����)�� �߽����� �ش� x���� ��� Line�� �׸���. 
		Image[Cy * W + x] = 255;
	}
}

// ���Ʒ� Filp 
void VerticalFlip(BYTE * Img, int W, int H) {
	for (int i = 0; i < H/2; i++) { 
		for (int j = 0; j < W; j++) {
			swap(&Img[i*W+j], &Img[(H-1-i)*W+j]);
		}
	}
}
// �¿� Flip 
void HorizontalFlip(BYTE* Img, int W, int H) {
	for (int i = 0; i < W/2 ; i++) {  // x��ǥ 
		for (int j = 0; j < H; j++) { // y��ǥ
			swap(&Img[j * W + i], &Img[(j * W + (W - 1 - i))]);
		}
	}
}

void Translation(BYTE * Image, BYTE* Output, int W, int H, int Tx, int Ty) {
	Ty *= -1; // 0,0�� ����(�»�) �϶�(���� �̹��� ��ǥ��� �Ϸ��� ó��)  
	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			if ((0 <= i + Ty && i + Ty < H) && (0 <= j + Tx && j + Tx < W)) {
				Output[(i + Ty) * W + (j + Tx)] = Image[i * W + j];
			}
		}
	}
}

// �����ϸ� 
void Scaling(BYTE *Image, BYTE * Output, int W, int H, double SF_X, double SF_Y ) {
	int tempX, tempY;
	for (int i = 0; i < H; i++) { // Y
		for (int j = 0; j < W; j++) { // X 
			tempX = (int)(j / SF_X); // ������ ���  
			tempY = (int)(i / SF_Y); // ������ ��� 
			// tempY = (int) (i*SF_Y); // ������ ���
			// tempX = (int) (j*SF_X); // ������ ��� 
			if (tempY < H && tempX < W)
				//Output[tempY * W + tempX] = Image[i * W + j]; // ������ ���, Hole�� �����. 
				Output[i * W + j] = Image[tempY * W + tempX]; // ������ ���, Hole �ذ�, Output�� �ش��ϴ� ���� Input�������� ������ �����´�. 
		}
	}
}

// ȸ�� 
void Rotation(BYTE * Image, BYTE * Output, int W, int H, int angle) {
	double radian = (3.141592 / 180.0) * angle; // ������ �������� ���� 
	int tempX, tempY;
	for (int i = 0; i < H; i++) { // Y
		for (int j = 0; j < W; j++) { // X 
			//tempX = (int)(cos(radian) * j - sin(radian) * i); // �ð� �ݴ���� ȸ�� ����, ������ ���  
			//tempY = (int)(sin(radian) * j + cos(radian) * i); // �ð� �ݴ���� ȸ�� ����, ������ ���
			tempX = (int)(cos(radian) * j + sin(radian) * i); // �ð� ���� ȸ�� ����, ������ ���  
			tempY = (int)(-sin(radian) * j + cos(radian) * i); // �ð� ���� ȸ�� ����, ������ ���
			if ((tempY < H && tempY >= 0) && (tempX < W && tempX >= 0))
				//Output[tempY * W + tempX] = Image[i * W + j]; // ������ ���
				Output[i * W + j] = Image[tempY * W + tempX]; // ������ ��� 
		}
	}
}
 

int main()
{
	BITMAPFILEHEADER hf; // 14����Ʈ
	BITMAPINFOHEADER hInfo; // 40����Ʈ
	RGBQUAD hRGB[256]; // 1024����Ʈ
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
	BYTE* Temp = (BYTE*)malloc(ImgSize); // �ӽù迭
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


	//Binarization(Image, Temp, W, H, 50); // ����ȭ, ���: �콽�� �ִ� ������ �����ϰ� ����ȭ�� ���� ���� -> ���� �������ִ� Object�� �ν� 
	//InverseImage(Temp, Temp, W, H); // Coin�� ��� (��ο�) ���� (����) , Pupil�� ��� (����) ���� (��ο�) => ����, Inverse ����
	//m_BlobColoring(Temp, H, W); // Component�� ���� �ٸ� ȭ�� ��Ⱚ�� ������ �ȴ�. �̼��� ������Ʈ ���� �� 25���� �����ȴ�. 
	//for (int i = 0; i < ImgSize; i++) Output[i] = Image[i]; // ���� ���� ����, Output[i] = 255; �ϸ� �� ȭ�鿡 ��찪�� ���´�. 
	//BinImageEdgeDetection(Temp, Output, W, H);

	//int x1 = 0, x2 = 0, y1 = 0, y2 = 0;

	//// �����簢���� ���� ��ǥ�� ã������ �Լ� ȣ�� 
	//FindXandY(Temp, W, H, &x1, &x2, &y1, &y2); // Call By Reference 
	//printf("x1:%d, x2:%d, y1:%d, y2:%d\n", x1, x2, y1, y2); 

	//// �����簢�� �׸��� �Լ� ȣ�� 
	//DrawRectOutline(Output, W, H, x1, x2, y1, y2);

	//// ������ �����߽��� ã�� ���� ��ǥ �ʱ�ȭ    
	//int cen_x = 0; int cen_y = 0;


	//// ������ �����߽� ��ǥ ���ϴ� ��� 1 (���� ����) 
	//CenterOfPupil(Temp, &cen_x, &cen_y, W, H); // Call By Reference
 //   printf("pupil_x:%d,pupil_y:%d", cen_x, cen_y);
	//
	//// ������ �����߽� ��ǥ ���ϴ� ��� 2 (���� ver) 
	//// FindXandY�� ���ؼ� ���� x1,x2,y1,y2�� Ȱ��, 
	//// cen_x= (x1+x2)/2, cen_y= (y1+y2)/2 �� DrawCrossLine�� ����(Cx, Cy)�� �־��ָ� �ȴ�. 

	//// ������ �����߽� ���� ũ�ν� ���� �׸��� 
	//DrawCrossLine(Output, W, H, cen_x, cen_y);
	
	VerticalFlip(Image, W, H);
	//HorizontalFlip(Image, W, H);
	//Translation(Image, Output, W, H, 50, 30);

	
	//Scaling(Image, Output, W,H, 0.7,0.7); // Uniform Scaling: Ȯ����� ����

	//Rotation(Image, Output, W,H,-45);
	SaveBMPFile(hf, hInfo, hRGB, Output, W, H, "rotation_vertical.bmp");

	free(Image);
	free(Output);
	free(Temp);
	return 0;
}