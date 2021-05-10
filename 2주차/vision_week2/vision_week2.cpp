// 201710745 김경원
/*
-3개의 bmp 파일 -
1) output1.bmp: lenna.bmp 파일과 동일한 영상
2) output2.bmp: 원본 영상의 밝기값을 50만큼 증가시킨 영상(별도 overflow로 인한 클리핑처리는 필요없음)
3) output3.bmp: 원본 영상의 반전 결과 영상(negative image)
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
	FILE* fp_1; // lenna.bmp 파일과 동일한 영상
	FILE* fp_2; // 원본 영상의 밝기값을 50만큼 증가시킨 영상
	FILE* fp_3; // 원본 영상의 반전 결과 영상

	fp = fopen("LENNA.bmp", "rb");
	if (fp == NULL) {
		printf("Not Found\n");
		return -1;
	}
	fread(&hf, sizeof(BITMAPFILEHEADER), 1, fp);
	fread(&hInfo, sizeof(BITMAPINFOHEADER), 1, fp);
	fread(hRGB, sizeof(RGBQUAD), 256, fp);

	int imgSize=hInfo.biWidth* hInfo.biHeight;
	BYTE* Image = (BYTE*)malloc(imgSize); // 강제 형변환 
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
	free(Output_1); // 동적할당 해제, memory leak 방지
	free(Output_2); // 동적할당 해제, memory leak 방지
	free(Output_3); // 동적할당 해제, memory leak 방지

	return 0; 
}