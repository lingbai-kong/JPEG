/*
若要在集成环境钟测试请将TEST置为1。
若要在控制台运行请将TEST置为0
*/
#define TEST 0
#include "PicReader.h"
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <cstring>
#include <cmath>
#define PI 3.1415926
#define MEM_ERR -1
#define FILE_ERR -2
#define GETBIT_ERR -3
using namespace std;
BYTE Luminance_Quantization_Table[64] = {
	16,  11,  10,  16,  24,  40,  51,  61,
	12,  12,  14,  19,  26,  58,  60,  55,
	14,  13,  16,  24,  40,  57,  69,  56,
	14,  17,  22,  29,  51,  87,  80,  62,
	18,  22,  37,  56,  68, 109, 103,  77,
	24,  35,  55,  64,  81, 104, 113,  92,
	49,  64,  78,  87, 103, 121, 120, 101,
	72,  92,  95,  98, 112, 100, 103,  99
};
BYTE Chrominance_Quantization_Table[64] = {
	17,  18,  24,  47,  99,  99,  99,  99,
	18,  21,  26,  66,  99,  99,  99,  99,
	24,  26,  56,  99,  99,  99,  99,  99,
	47,  66,  99,  99,  99,  99,  99,  99,
	99,  99,  99,  99,  99,  99,  99,  99,
	99,  99,  99,  99,  99,  99,  99,  99,
	99,  99,  99,  99,  99,  99,  99,  99,
	99,  99,  99,  99,  99,  99,  99,  99
};
UINT ZigzagTable[64] = {
	0, 1, 5, 6,14,15,27,28,
	2, 4, 7,13,16,26,29,42,
	3, 8,12,17,25,30,41,43,
	9,11,18,24,31,40,44,53,
	10,19,23,32,39,45,52,54,
	20,22,33,38,46,51,55,60,
	21,34,37,47,50,56,59,61,
	35,36,48,49,57,58,62,63
};
BYTE Standard_DC_Luminance_NRCodes[] = { 0, 0, 7, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0 };
BYTE Standard_DC_Luminance_Values[] = { 4, 5, 3, 2, 6, 1, 0, 7, 8, 9, 10, 11 };

BYTE Standard_DC_Chrominance_NRCodes[] = { 0, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0 };
BYTE Standard_DC_Chrominance_Values[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };

BYTE Standard_AC_Luminance_NRCodes[] = { 0, 2, 1, 3, 3, 2, 4, 3, 5, 5, 4, 4, 0, 0, 1, 0x7d };
BYTE Standard_AC_Luminance_Values[] =
{
	0x01, 0x02, 0x03, 0x00, 0x04, 0x11, 0x05, 0x12,
	0x21, 0x31, 0x41, 0x06, 0x13, 0x51, 0x61, 0x07,
	0x22, 0x71, 0x14, 0x32, 0x81, 0x91, 0xa1, 0x08,
	0x23, 0x42, 0xb1, 0xc1, 0x15, 0x52, 0xd1, 0xf0,
	0x24, 0x33, 0x62, 0x72, 0x82, 0x09, 0x0a, 0x16,
	0x17, 0x18, 0x19, 0x1a, 0x25, 0x26, 0x27, 0x28,
	0x29, 0x2a, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,
	0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49,
	0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59,
	0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69,
	0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79,
	0x7a, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89,
	0x8a, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98,
	0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7,
	0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6,
	0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3, 0xc4, 0xc5,
	0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2, 0xd3, 0xd4,
	0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xe1, 0xe2,
	0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea,
	0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8,
	0xf9, 0xfa
};

BYTE Standard_AC_Chrominance_NRCodes[] = { 0, 2, 1, 2, 4, 4, 3, 4, 7, 5, 4, 4, 0, 1, 2, 0x77 };
BYTE Standard_AC_Chrominance_Values[] =
{
	0x00, 0x01, 0x02, 0x03, 0x11, 0x04, 0x05, 0x21,
	0x31, 0x06, 0x12, 0x41, 0x51, 0x07, 0x61, 0x71,
	0x13, 0x22, 0x32, 0x81, 0x08, 0x14, 0x42, 0x91,
	0xa1, 0xb1, 0xc1, 0x09, 0x23, 0x33, 0x52, 0xf0,
	0x15, 0x62, 0x72, 0xd1, 0x0a, 0x16, 0x24, 0x34,
	0xe1, 0x25, 0xf1, 0x17, 0x18, 0x19, 0x1a, 0x26,
	0x27, 0x28, 0x29, 0x2a, 0x35, 0x36, 0x37, 0x38,
	0x39, 0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48,
	0x49, 0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58,
	0x59, 0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
	0x69, 0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78,
	0x79, 0x7a, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
	0x88, 0x89, 0x8a, 0x92, 0x93, 0x94, 0x95, 0x96,
	0x97, 0x98, 0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5,
	0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4,
	0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3,
	0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2,
	0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda,
	0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9,
	0xea, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8,
	0xf9, 0xfa
};
class Jpeg {
protected:
	PicReader img;								//提取图片信息或展示图片
	fstream file;								//对文件进行操作
	BYTE* source = nullptr;						//存放图片原始RGBA数据
	BYTE* div = nullptr;						//存放图片的分块数据
	DOUBLE* YCC = nullptr;						//存放图片的YCbCr颜色空间的分块数据
	DOUBLE* DCT = nullptr;						//存放离散余弦变换后的数据
	WORD* quant = nullptr;						//存放量化后的数据
	DOUBLE* Zlist = nullptr;					//存放Zigzag扫描排序后的数据
	SHORT* RLE_Y = nullptr;						//存放行程编码后的Y分量数据
	SHORT* RLE_Cb = nullptr;					//存放行程编码后的Cb分量数据
	SHORT* RLE_Cr = nullptr;					//存放行程编码后的Cr分量数据
	WORD* Huff_DC_Y_Value = nullptr;			//Y分量直流量值霍夫曼表
	WORD* Huff_DC_Y_Length = nullptr;			//Y分量直流量长度霍夫曼表
	WORD* Huff_DC_C_Value = nullptr;			//C分量直流量值霍夫曼表
	WORD* Huff_DC_C_Length = nullptr;			//C分量直流量长度霍夫曼表
	WORD* Huff_AC_Y_Value = nullptr;			//Y分量交流量值霍夫曼表
	WORD* Huff_AC_Y_Length = nullptr;			//Y分量交流量长度霍夫曼表
	WORD* Huff_AC_C_Value = nullptr;			//C分量交流量值霍夫曼表
	WORD* Huff_AC_C_Length = nullptr;			//C分量交流量长度霍夫曼表
	UINT x, y;									//原始图像宽、高
	UINT divx, divy;							//分块后图像宽、高
	BYTE Qy[64];								//Y分量Zigzag扫描量化表
	BYTE Qc[64];								//C分量Zigzag扫描量化表
	void putBit(BYTE b, BOOL isfinish = false)//向文件流file输出一个比特b,当isfinish为真时处理输出最后一个字节
	{
		static BYTE buf = 0;//字节缓冲区
		static BYTE buf_count = 0;
		BYTE zero = 0x00;
		if (isfinish && buf_count != 0)//最后一个缓冲不满时
		{
			buf <<= 8 - buf_count;//补零对齐
			file.write((char*)&buf, sizeof(BYTE));//写入压缩文件
		}
		else
		{
			if (b == 1)
			{
				buf = buf << 1;
				buf = buf | 1;
				buf_count++;
			}
			else
			{
				buf = buf << 1;
				buf_count++;
			}
			if (buf_count == 8)//缓冲区满时
			{
				file.write((char*)&buf, sizeof(BYTE));//把缓冲字节写入压缩文件
				if (buf == 0xFF)
					file.write((char*)&zero, sizeof(BYTE));
				buf = 0;//缓冲复位
				buf_count = 0;//缓冲区计数复位
			}
		}
	}
	void getBit(BYTE& b)//从文件流file中提取一个比特到b
	{
		static BYTE buf = 0;//字节缓冲区
		static short buf_count = 0;
		static short k = 128;//掩码

		if (buf_count == 0)
		{
			if (file.peek() == EOF)//文件读完却仍然请求读取
			{
				cerr << "error:no more byte to read\n";
				exit(GETBIT_ERR);
			}
			else//缓冲读完时
			{
				if (buf == 0xFF)
				{
					file.read((char*)&buf, sizeof(BYTE));
					if (buf == 0xD9)
					{
						cerr << "error:meet end of jpeg\n";
						exit(FILE_ERR);
					}
				}
				file.read((char*)&buf, sizeof(BYTE));
				buf_count = 8;//缓冲计数复位
				k = 128;//掩码复位
			}
		}
		if (buf & k)
		{
			b = 1;
		}
		else
		{
			b = 0;
		}
		k = k / 2;
		buf_count--;
	}
	void putBYTE(BYTE b)//向文件中写入一个字节b
	{
		file.write((const char*)&b, sizeof(BYTE));
	}
	void getBYTE(BYTE& b)//从文件中读取一个字节b
	{
		file.read((char*)&b, sizeof(BYTE));
	}
	void putWORD(WORD w)//以大端方式向文件中写入字w
	{
		WORD t = ((w >> 8) & 0xFF) | ((w & 0xFF) << 8);
		file.write((const char*)&t, sizeof(WORD));
	}
	void getWORD(WORD& w)//以大端方式向文件中读取字w
	{
		WORD t;
		file.read((char*)&t, sizeof(WORD));
		w = ((t >> 8) & 0xFF) | ((t & 0xFF) << 8);
	}
	void putString(const char* ps, UINT length)//向文件中写入lengthB数据
	{
		file.write(ps, length * sizeof(BYTE));
	}
	void getString(char* gs, UINT length)//从文件中读取lengthB数据
	{
		file.read(gs, length * sizeof(BYTE));
	}
	void _initHuffmanTable(const BYTE* NRCodes, const BYTE* Values, WORD* Bits, WORD* Length)
	{
		UINT table_pos = 0;
		UINT code_value = 0;
		for (UINT i = 1; i <= 16; i++)
		{
			for (UINT j = 1; j <= NRCodes[i - 1]; j++)
			{
				Bits[Values[table_pos]] = code_value;
				Length[Values[table_pos]] = i;
				table_pos++;
				code_value++;
			}
			code_value <<= 1;
		}
	}
	void InitHuffmanTable()
	{
		_initHuffmanTable(Standard_DC_Luminance_NRCodes, Standard_DC_Luminance_Values, Huff_DC_Y_Value, Huff_DC_Y_Length);
		_initHuffmanTable(Standard_DC_Chrominance_NRCodes, Standard_DC_Chrominance_Values, Huff_DC_C_Value, Huff_DC_C_Length);
		_initHuffmanTable(Standard_AC_Luminance_NRCodes, Standard_AC_Luminance_Values, Huff_AC_Y_Value, Huff_AC_Y_Length);
		_initHuffmanTable(Standard_AC_Chrominance_NRCodes, Standard_AC_Chrominance_Values, Huff_AC_C_Value, Huff_AC_C_Length);
	}
	void InitQualityTable(INT scale = 50)
	{
		if (scale <= 0) scale = 1;
		if (scale >= 1000) scale = 999;

		for (UINT i = 0; i < 64; i++)
		{
			INT turn = (Luminance_Quantization_Table[i] * scale + 50) / 100;
			if (turn <= 0) turn = 1;
			if (turn > 0xFF) turn = 0xFF;
			Qy[ZigzagTable[i]] = turn;

			turn = (Chrominance_Quantization_Table[i] * scale + 50) / 100;
			if (turn <= 0) turn = 1;
			if (turn > 0xFF) turn = 0xFF;
			Qc[ZigzagTable[i]] = turn;
		}
	}
	void WriteHead()
	{
		putWORD(0xFFD8);	//SOI,文件开始

		putWORD(0xFFE0);	//APP0，JFIF应用资料块
		putWORD(16);		//APP0资料块大小,16BYTE
		putString("JFIF", 5);//标识符
		putBYTE(1);			// 版本号_hi
		putBYTE(1);			// 版本号_low
		putBYTE(0);			// x和y的密度单位，0为无单位
		putWORD(1);			// x方向像素密度
		putWORD(1);			// y方向像素密度
		putBYTE(0);			// 缩略图水平像素数目
		putBYTE(0);			// 缩略图竖直像素数目

		putWORD(0xFFE1);	//APP1标记，高程作业标记
		putWORD(21);		//APP1资料块大小,16BYTE
		putString("klb's jpeg program", 19);//高程作业信息

		putWORD(0xFFDB);	//DQT，量化表
		putWORD(132);		//量化表长度为132个字节
		putBYTE(0);			//y分量量化表
		putString((const char*)Qy, 64);
		putBYTE(1);			//c分量量化表
		putString((const char*)Qc, 64);

		putWORD(0xFFC0);	//SOFO，帧开始
		putWORD(17);		//帧开始长度
		putBYTE(8);			//精度，每个颜色分量每个像素的位数
		putWORD(y);			//图像高度
		putWORD(x);			//图像宽度
		putBYTE(3);			//颜色分量数

		putBYTE(1);			//第一个颜色分量Y
		putBYTE(0x11);		//垂直方向的样本因子（低四位）和水平方向的样本因子（高四位）
		putBYTE(0);			//量化表符号Luminance_Quantization_Table

		putBYTE(2);			//第二个颜色分量Cb
		putBYTE(0x11);		//垂直方向的样本因子（低四位）和水平方向的样本因子（高四位）
		putBYTE(1);			//量化表符号Chrominance_Quantization_Table

		putBYTE(3);			//第三个颜色分量Cr
		putBYTE(0x11);		//垂直方向的样本因子（低四位）和水平方向的样本因子（高四位）
		putBYTE(1);			//量化表符号Chrominance_Quantization_Table

		putWORD(0xFFC4);	//DHT，霍夫曼表
		putWORD(418);		//霍夫曼表长度为418BYTE
		putBYTE(0x00);		//类型+索引
		putString((const char*)Standard_DC_Luminance_NRCodes, sizeof(Standard_DC_Luminance_NRCodes));
		putString((const char*)Standard_DC_Luminance_Values, sizeof(Standard_DC_Luminance_Values));
		putBYTE(0x10);		//类型+索引
		putString((const char*)Standard_AC_Luminance_NRCodes, sizeof(Standard_AC_Luminance_NRCodes));
		putString((const char*)Standard_AC_Luminance_Values, sizeof(Standard_AC_Luminance_Values));
		putBYTE(0x01);		//类型+索引
		putString((const char*)Standard_DC_Chrominance_NRCodes, sizeof(Standard_DC_Chrominance_NRCodes));
		putString((const char*)Standard_DC_Chrominance_Values, sizeof(Standard_DC_Chrominance_Values));
		putBYTE(0x11);		//类型+索引
		putString((const char*)Standard_AC_Chrominance_NRCodes, sizeof(Standard_AC_Chrominance_NRCodes));
		putString((const char*)Standard_AC_Chrominance_Values, sizeof(Standard_AC_Chrominance_Values));

		putWORD(0xFFDA);	//SOS，扫描线开始
		putWORD(12);		//SOS长度
		putBYTE(3);			//颜色分量数

		putBYTE(1);			//第一个颜色分量ID
		putBYTE(0);			//交流表号（第四位）直流表号（高四位）

		putBYTE(2);			//第二个颜色分量ID
		putBYTE(0x11);		//交流表号（第四位）直流表号（高四位）

		putBYTE(3);			//第三个颜色分量ID
		putBYTE(0x11);		//交流表号（第四位）直流表号（高四位）

							//Jpeg渐进式图像相关数据
		putBYTE(0);			//Ss    Zigzag index of first coefficient included in scan
		putBYTE(0x3F);		//Se    Zigzag index of last coefficient included in scan
		putBYTE(0);			//Ah    Zero for first scan of a coefficient, else Al of prior scan
							//Al    Successive approximation low bit position for scan
	}
	void ReadHead()
	{
		BOOL flag = TRUE;
		BYTE B;
		WORD W;
		WORD L;
		while (flag)
		{
			getBYTE(B);
			if (B != 0xFF)
				exit(FILE_ERR);
			getBYTE(B);
			switch (B)
			{
			case 0xD8:
				break;
			case 0xE0:
			case 0xE1:
			case 0xE2:
			case 0xE3:
			case 0xE4:
			case 0xE5:
			case 0xE6:
			case 0xE7:
			case 0xE8:
			case 0xE9:
			case 0xEA:
			case 0xEB:
			case 0xEC:
			case 0xED:
			case 0xEF:
				getWORD(L);
				L -= 2;
				while (L > 0)
				{
					getBYTE(B);
					L--;
				}
				break;
			case 0xDB:
				getWORD(W);
				getBYTE(B);
				getString((char*)Qy, 64);
				getBYTE(B);
				getString((char*)Qc, 64);
				break;
			case 0xC0:
				getWORD(L);
				L -= 2;
				getBYTE(B);
				L -= 1;
				getWORD(W);
				y = W;
				L -= 2;
				getWORD(W);
				x = W;
				L -= 2;
				divx = ((x + 7) / 8) * 8;
				divy = ((y + 7) / 8) * 8;
				while (L > 0)
				{
					getBYTE(B);
					L--;
				}
				break;
			case 0xC4:
				getWORD(W);
				getBYTE(B);
				getString((char*)Standard_DC_Luminance_NRCodes, sizeof(Standard_DC_Luminance_NRCodes));
				getString((char*)Standard_DC_Luminance_Values, sizeof(Standard_DC_Luminance_Values));
				getBYTE(B);
				getString((char*)Standard_AC_Luminance_NRCodes, sizeof(Standard_AC_Luminance_NRCodes));
				getString((char*)Standard_AC_Luminance_Values, sizeof(Standard_AC_Luminance_Values));
				getBYTE(B);
				getString((char*)Standard_DC_Chrominance_NRCodes, sizeof(Standard_DC_Chrominance_NRCodes));
				getString((char*)Standard_DC_Chrominance_Values, sizeof(Standard_DC_Chrominance_Values));
				getBYTE(B);
				getString((char*)Standard_AC_Chrominance_NRCodes, sizeof(Standard_AC_Chrominance_NRCodes));
				getString((char*)Standard_AC_Chrominance_Values, sizeof(Standard_AC_Chrominance_Values));
				break;
			case 0xDA:
				getWORD(L);
				L -= 2;
				while (L > 0)
				{
					getBYTE(B);
					L--;
				}
				flag = false;
				break;
			default:
				exit(FILE_ERR);
				break;
			}
		}
	}
	void Div()
	{
		for (UINT Y = 0; Y < divy / 8; Y++)
		{
			for (UINT X = 0; X < divx / 8; X++)
			{
				for (UINT j = Y * 8; j < (Y + 1) * 8 && j < y; j++)
				{
					for (UINT i = X * 8; i < (X + 1) * 8 && i < x; i++)
					{
						div[((Y * divx / 8 + X) * 64 + (j - Y * 8) * 8 + i - X * 8) * 3 + 0] = source[(j * x + i) * 4 + 0];
						div[((Y * divx / 8 + X) * 64 + (j - Y * 8) * 8 + i - X * 8) * 3 + 1] = source[(j * x + i) * 4 + 1];
						div[((Y * divx / 8 + X) * 64 + (j - Y * 8) * 8 + i - X * 8) * 3 + 2] = source[(j * x + i) * 4 + 2];
					}
				}
			}
		}
	}
	void unDiv()
	{
		for (UINT Y = 0; Y < divy / 8; Y++)
		{
			for (UINT X = 0; X < divx / 8; X++)
			{
				for (UINT j = Y * 8; j < (Y + 1) * 8 && j < y; j++)
				{
					for (UINT i = X * 8; i < (X + 1) * 8 && i < x; i++)
					{
						source[(j * x + i) * 4 + 0] = div[((Y * divx / 8 + X) * 64 + (j - Y * 8) * 8 + i - X * 8) * 3 + 0];
						source[(j * x + i) * 4 + 1] = div[((Y * divx / 8 + X) * 64 + (j - Y * 8) * 8 + i - X * 8) * 3 + 1];
						source[(j * x + i) * 4 + 2] = div[((Y * divx / 8 + X) * 64 + (j - Y * 8) * 8 + i - X * 8) * 3 + 2];
					}
				}
			}
		}
	}
	void RGB2YCC()
	{
		for (UINT j = 0; j < divy; j++)
		{
			for (UINT i = 0; i < divx; i++)
			{
				BYTE R, G, B;
				R = div[(j * divx + i) * 3 + 0];
				G = div[(j * divx + i) * 3 + 1];
				B = div[(j * divx + i) * 3 + 2];
				YCC[(j * divx + i) * 3 + 0] = 0.299 * R + 0.5870 * G + 0.114 * B - 128; //Y
				YCC[(j * divx + i) * 3 + 1] = -0.1687 * R - 0.3313 * G + 0.5 * B;       //Cb
				YCC[(j * divx + i) * 3 + 2] = 0.5 * R - 0.4187 * G - 0.0813 * B;        //Cr
			}
		}
	}
	void YCC2RGB()
	{
		for (UINT j = 0; j < divy; j++)
		{
			for (UINT i = 0; i < divx; i++)
			{
				DOUBLE Y, Cb, Cr;
				Y = YCC[(j * divx + i) * 3 + 0];
				Cb = YCC[(j * divx + i) * 3 + 1];
				Cr = YCC[(j * divx + i) * 3 + 2];
				DOUBLE R, G, B;
				R = (Y + 128) + 1.402 * Cr;
				G = (Y + 128) - 0.34414 * Cb - 0.71414 * Cr;
				B = (Y + 128) + 1.772 * Cb;

				if (R > 255)
					R = 255;
				if (G > 255)
					G = 255;
				if (B > 255)
					B = 255;

				if (R < 0)
					R = 0;
				if (G < 0)
					G = 0;
				if (B < 0)
					B = 0;

				div[(j * divx + i) * 3 + 0] = BYTE(R);//R
				div[(j * divx + i) * 3 + 1] = BYTE(G);//G
				div[(j * divx + i) * 3 + 2] = BYTE(B);//B
			}
		}
	}
	void YCC2DCT()
	{
		for (UINT Y = 0; Y < divy / 8; Y++)
		{
			for (UINT X = 0; X < divx / 8; X++)
			{
				for (UINT v = 0; v < 8; v++)
				{
					for (UINT u = 0; u < 8; u++)
					{
						DOUBLE alpha_u = (u == 0) ? 1 / sqrt(8) : 0.5;
						DOUBLE alpha_v = (v == 0) ? 1 / sqrt(8) : 0.5;
						DOUBLE sigma_Y = 0;
						DOUBLE sigma_Cb = 0;
						DOUBLE sigma_Cr = 0;
						for (UINT j = 0; j < 8; j++)
						{
							for (UINT i = 0; i < 8; i++)
							{
								DOUBLE t = cos((2 * DOUBLE(i) + 1) * u * PI / 16) * cos((2 * DOUBLE(j) + 1) * v * PI / 16);

								sigma_Y += YCC[((Y * divx / 8 + X) * 64 + j * 8 + i) * 3 + 0] * t;
								sigma_Cb += YCC[((Y * divx / 8 + X) * 64 + j * 8 + i) * 3 + 1] * t;
								sigma_Cr += YCC[((Y * divx / 8 + X) * 64 + j * 8 + i) * 3 + 2] * t;
							}
						}
						DCT[((Y * divx / 8 + X) * 64 + v * 8 + u) * 3 + 0] = alpha_u * alpha_v * sigma_Y;
						DCT[((Y * divx / 8 + X) * 64 + v * 8 + u) * 3 + 1] = alpha_u * alpha_v * sigma_Cb;
						DCT[((Y * divx / 8 + X) * 64 + v * 8 + u) * 3 + 2] = alpha_u * alpha_v * sigma_Cr;
					}
				}
			}
		}
	}
	void DCT2YCC()
	{
		for (UINT Y = 0; Y < divy / 8; Y++)
		{
			for (UINT X = 0; X < divx / 8; X++)
			{
				for (UINT j = 0; j < 8; j++)
				{
					for (UINT i = 0; i < 8; i++)
					{
						DOUBLE sigma_Y = 0;
						DOUBLE sigma_Cb = 0;
						DOUBLE sigma_Cr = 0;
						for (UINT v = 0; v < 8; v++)
						{
							for (UINT u = 0; u < 8; u++)
							{
								DOUBLE alpha_u = (u == 0) ? 1 / sqrt(8) : 0.5;
								DOUBLE alpha_v = (v == 0) ? 1 / sqrt(8) : 0.5;
								DOUBLE t = cos((2 * DOUBLE(i) + 1) * u * PI / 16) * cos((2 * DOUBLE(j) + 1) * v * PI / 16);

								sigma_Y += alpha_u * alpha_v * DCT[((Y * divx / 8 + X) * 64 + v * 8 + u) * 3 + 0] * t;
								sigma_Cb += alpha_u * alpha_v * DCT[((Y * divx / 8 + X) * 64 + v * 8 + u) * 3 + 1] * t;
								sigma_Cr += alpha_u * alpha_v * DCT[((Y * divx / 8 + X) * 64 + v * 8 + u) * 3 + 2] * t;
							}
						}
						YCC[((Y * divx / 8 + X) * 64 + j * 8 + i) * 3 + 0] = sigma_Y;
						YCC[((Y * divx / 8 + X) * 64 + j * 8 + i) * 3 + 1] = sigma_Cb;
						YCC[((Y * divx / 8 + X) * 64 + j * 8 + i) * 3 + 2] = sigma_Cr;
					}
				}
			}
		}
	}
	void Zigzag()
	{
		for (UINT Y = 0; Y < divy / 8; Y++)
		{
			for (UINT X = 0; X < divx / 8; X++)
			{
				for (UINT j = 0; j < 8; j++)
				{
					for (UINT i = 0; i < 8; i++)
					{
						Zlist[((Y * divx / 8 + X) * 64 + ZigzagTable[j * 8 + i]) * 3 + 0] = DCT[((Y * divx / 8 + X) * 64 + j * 8 + i) * 3 + 0];
						Zlist[((Y * divx / 8 + X) * 64 + ZigzagTable[j * 8 + i]) * 3 + 1] = DCT[((Y * divx / 8 + X) * 64 + j * 8 + i) * 3 + 1];
						Zlist[((Y * divx / 8 + X) * 64 + ZigzagTable[j * 8 + i]) * 3 + 2] = DCT[((Y * divx / 8 + X) * 64 + j * 8 + i) * 3 + 2];
					}
				}
			}
		}
	}
	void unZigzag()
	{
		for (UINT Y = 0; Y < divy / 8; Y++)
		{
			for (UINT X = 0; X < divx / 8; X++)
			{
				for (UINT j = 0; j < 8; j++)
				{
					for (UINT i = 0; i < 8; i++)
					{
						DCT[((Y * divx / 8 + X) * 64 + j * 8 + i) * 3 + 0] = Zlist[((Y * divx / 8 + X) * 64 + ZigzagTable[j * 8 + i]) * 3 + 0];
						DCT[((Y * divx / 8 + X) * 64 + j * 8 + i) * 3 + 1] = Zlist[((Y * divx / 8 + X) * 64 + ZigzagTable[j * 8 + i]) * 3 + 1];
						DCT[((Y * divx / 8 + X) * 64 + j * 8 + i) * 3 + 2] = Zlist[((Y * divx / 8 + X) * 64 + ZigzagTable[j * 8 + i]) * 3 + 2];
					}
				}
			}
		}
	}
	void Quantization()
	{
		for (UINT Y = 0; Y < divy / 8; Y++)
		{
			for (UINT X = 0; X < divx / 8; X++)
			{
				for (UINT n = 0; n < 64; n++)
				{
					quant[((Y * divx / 8 + X) * 64 + n) * 3 + 0] = WORD(round(Zlist[((Y * divx / 8 + X) * 64 + n) * 3 + 0] / Qy[n]));
					quant[((Y * divx / 8 + X) * 64 + n) * 3 + 1] = WORD(round(Zlist[((Y * divx / 8 + X) * 64 + n) * 3 + 1] / Qc[n]));
					quant[((Y * divx / 8 + X) * 64 + n) * 3 + 2] = WORD(round(Zlist[((Y * divx / 8 + X) * 64 + n) * 3 + 2] / Qc[n]));
				}
			}
		}
	}
	void unQuantization()
	{
		for (UINT Y = 0; Y < divy / 8; Y++)
		{
			for (UINT X = 0; X < divx / 8; X++)
			{
				for (UINT n = 0; n < 64; n++)
				{
					Zlist[((Y * divx / 8 + X) * 64 + n) * 3 + 0] = DOUBLE(SHORT(quant[((Y * divx / 8 + X) * 64 + n) * 3 + 0])) * DOUBLE(Qy[n]);
					Zlist[((Y * divx / 8 + X) * 64 + n) * 3 + 1] = DOUBLE(SHORT(quant[((Y * divx / 8 + X) * 64 + n) * 3 + 1])) * DOUBLE(Qc[n]);
					Zlist[((Y * divx / 8 + X) * 64 + n) * 3 + 2] = DOUBLE(SHORT(quant[((Y * divx / 8 + X) * 64 + n) * 3 + 2])) * DOUBLE(Qc[n]);
				}
			}
		}
	}
	void _RLE(SHORT* rle, BYTE no)
	{
		for (UINT Y = 0; Y < divy / 8; Y++)
		{
			for (UINT X = 0; X < divx / 8; X++)
			{
				UINT n = (Y * divx / 8 + X) * 128;
				UINT eof_pos;
				UINT zero_count = 0;
				UINT m = 1;
				rle[n++] = 0;
				rle[n++] = quant[((Y * divx / 8 + X) * 64 + 0) * 3 + no];
				for (eof_pos = 63; eof_pos >= 1; eof_pos--)
				{
					if (quant[((Y * divx / 8 + X) * 64 + eof_pos) * 3 + no] != 0)
						break;
				}
				eof_pos++;
				while (m < eof_pos)
				{
					if (quant[((Y * divx / 8 + X) * 64 + m) * 3 + no] == 0)
					{
						zero_count++;
						if (zero_count == 16)
						{
							rle[n++] = 15;
							rle[n++] = 0;
							zero_count = 0;
						}
					}
					else
					{
						rle[n++] = zero_count;
						rle[n++] = quant[((Y * divx / 8 + X) * 64 + m) * 3 + no];
						zero_count = 0;
					}
					m++;
				}
			}
		}
	}
	void RLE()
	{
		_RLE(RLE_Y, 0);
		_RLE(RLE_Cb, 1);
		_RLE(RLE_Cr, 2);
	}
	void _unRLE(SHORT* rle, BYTE no)
	{
		for (UINT Y = 0; Y < divy / 8; Y++)
		{
			for (UINT X = 0; X < divx / 8; X++)
			{
				UINT n = (Y * divx / 8 + X) * 128 + 1;
				UINT zero_count = 0;
				UINT data = 0;
				UINT m = 0;
				quant[((Y * divx / 8 + X) * 64 + m++) * 3 + no] = rle[n++];
				while (m < 64)
				{
					zero_count = rle[n++];
					data = rle[n++];

					if (zero_count == 0 && data == 0)
						break;

					while (zero_count > 0)
					{
						quant[((Y * divx / 8 + X) * 64 + m++) * 3 + no] = 0;
						zero_count--;
					}
					quant[((Y * divx / 8 + X) * 64 + m++) * 3 + no] = data;
				}
			}
		}
	}
	void unRLE()
	{
		_unRLE(RLE_Y, 0);
		_unRLE(RLE_Cb, 1);
		_unRLE(RLE_Cr, 2);
	}
	void DPCM()
	{
		for (INT Y = divy / 8 - 1; Y >= 0; Y--)
		{
			for (INT X = divx / 8 - 1; X >= 0; X--)
			{
				UINT n = (Y * divx / 8 + X) * 128;
				if (n == 0)
					break;
				UINT pre = (Y * divx / 8 + X - 1) * 128;
				RLE_Y[n + 1] = (RLE_Y[n + 1] - RLE_Y[pre + 1]);
				RLE_Cb[n + 1] = (RLE_Cb[n + 1] - RLE_Cb[pre + 1]);
				RLE_Cr[n + 1] = (RLE_Cr[n + 1] - RLE_Cr[pre + 1]);
			}
		}
	}
	void unDPCM()
	{
		for (UINT Y = 0; Y < divy / 8; Y++)
		{
			for (UINT X = 0; X < divx / 8; X++)
			{
				UINT n = (Y * divx / 8 + X) * 128;
				if (n == 0)
					continue;
				UINT pre = (Y * divx / 8 + X - 1) * 128;
				RLE_Y[n + 1] = (RLE_Y[n + 1] + RLE_Y[pre + 1]);
				RLE_Cb[n + 1] = (RLE_Cb[n + 1] + RLE_Cb[pre + 1]);
				RLE_Cr[n + 1] = (RLE_Cr[n + 1] + RLE_Cr[pre + 1]);
			}
		}
	}
	BOOL code(WORD p, SHORT q, WORD* Huff_Value, WORD* Huff_Length)
	{
		if (p == 0 && q == 0)
		{
			WORD mask = 1;
			mask <<= (Huff_Length[0] - 1);
			while (mask > 0)
			{
				if (mask & Huff_Value[0])
				{
					putBit(1);
				}
				else
				{
					putBit(0);
				}
				mask >>= 1;
			}
			return false;
		}
		else {
			if (q > 0)
			{
				BYTE length = 16;
				WORD mask = 0x8000;
				while ((mask & q) == 0)
				{
					length--;
					mask >>= 1;
				}
				p <<= 4;
				p |= length;

				mask = 1;
				mask <<= (Huff_Length[p] - 1);
				while (mask > 0)
				{
					if (mask & Huff_Value[p])
					{
						putBit(1);
					}
					else
					{
						putBit(0);
					}
					mask >>= 1;
				}

				mask = 1;
				mask <<= length - 1;
				while (mask > 0)
				{
					if (mask & q)
					{
						putBit(1);
					}
					else
					{
						putBit(0);
					}
					mask >>= 1;
				}
			}
			else if (q < 0)
			{
				SHORT _q = -q;
				BYTE length = 16;
				WORD mask = 0x8000;
				while ((mask & _q) == 0)
				{
					length--;
					mask >>= 1;
				}
				p <<= 4;
				p |= length;

				mask = 1;
				mask <<= (Huff_Length[p] - 1);
				while (mask > 0)
				{
					if (mask & Huff_Value[p])
					{
						putBit(1);
					}
					else
					{
						putBit(0);
					}
					mask >>= 1;
				}

				mask = 1;
				mask <<= length - 1;
				while (mask > 0)
				{
					if (mask & _q)
					{
						putBit(0);
					}
					else
					{
						putBit(1);
					}
					mask >>= 1;
				}
			}
			else
			{
				p <<= 4;
				WORD mask = 1;
				mask <<= (Huff_Length[p] - 1);
				while (mask > 0)
				{
					if (mask & Huff_Value[p])
					{
						putBit(1);
					}
					else
					{
						putBit(0);
					}
					mask >>= 1;
				}
			}
			return true;
		}
	}
	void _WriteHuffmanBit(UINT X, UINT Y, SHORT* rle, WORD* Huff_DC_Value, WORD* Huff_DC_Length, WORD* Huff_AC_Value, WORD* Huff_AC_Length)
	{
		UINT count = 0;
		UINT n = (Y * divx / 8 + X) * 128;

		code(rle[n + 0], rle[n + 1], Huff_DC_Value, Huff_DC_Length);
		count++;

		for (UINT i = 2; i < 128; i += 2)
		{
			if (!code(rle[n + i], rle[n + i + 1], Huff_AC_Value, Huff_AC_Length))
				break;
			count += rle[n + i];
			count++;
			if (count == 64)
				break;
		}
	}
	void WriteHuffmanBit()
	{
		for (UINT Y = 0; Y < divy / 8; Y++)
		{
			for (UINT X = 0; X < divx / 8; X++)
			{
				_WriteHuffmanBit(X, Y, RLE_Y, Huff_DC_Y_Value, Huff_DC_Y_Length, Huff_AC_Y_Value, Huff_AC_Y_Length);
				_WriteHuffmanBit(X, Y, RLE_Cb, Huff_DC_C_Value, Huff_DC_C_Length, Huff_AC_C_Value, Huff_AC_C_Length);
				_WriteHuffmanBit(X, Y, RLE_Cr, Huff_DC_C_Value, Huff_DC_C_Length, Huff_AC_C_Value, Huff_AC_C_Length);
			}
		}
		putBit(0, 1);
	}
	void decode(SHORT& p, SHORT& q, WORD* Huff_Value, WORD* Huff_Length, INT SIZE)
	{
		p = 0; q = 0;
		UINT length = 0;
		WORD t = 0x00;
		WORD _p = 0x0000;
		BYTE b = 0x00;
		while (length < 16)
		{
			getBit(b);
			t <<= 1;
			t |= b;
			length++;
			for (_p = 0; _p < SIZE; _p++)
			{
				if (Huff_Length[_p] == length && Huff_Value[_p] == t)
				{
					break;
				}
			}
			if (Huff_Length[_p] == length && Huff_Value[_p] == t)
			{
				break;
			}
		}

		p = (_p >> 4) & 0x0F;
		length = (_p & 0x0F);
		if (length > 0)
		{
			getBit(b);
			length--;
			if (b)
			{
				q |= b;
				while (length > 0)
				{
					q <<= 1;
					getBit(b);
					q |= b;
					length--;
				}
			}
			else
			{
				q |= ((~b) & 0x01);
				while (length > 0)
				{
					q <<= 1;
					getBit(b);
					q |= ((~b) & 0x01);
					length--;
				}
				q = -q;
			}
		}
	}
	void _ReadHuffmanBit(UINT X, UINT Y, SHORT* rle, WORD* Huff_DC_Value, WORD* Huff_DC_Length, WORD* Huff_AC_Value, WORD* Huff_AC_Length)
	{
		UINT count = 0;
		UINT n = (Y * divx / 8 + X) * 128;
		decode(rle[n + 0], rle[n + 1], Huff_DC_Value, Huff_DC_Length, 16);
		count++;

		for (UINT i = 2; i < 128; i += 2)
		{
			decode(rle[n + i], rle[n + i + 1], Huff_AC_Value, Huff_AC_Length, 256);
			if (rle[n + i] == 0 && rle[n + i + 1] == 0)
			{
				while (i < 128)
					rle[n + i++] = 0;
				break;
			}
			count += rle[n + i];
			count++;
			if (count == 64)
				break;
		}
	}
	void ReadHuffmanBit()
	{
		for (UINT Y = 0; Y < divy / 8; Y++)
		{
			for (UINT X = 0; X < divx / 8; X++)
			{
				_ReadHuffmanBit(X, Y, RLE_Y, Huff_DC_Y_Value, Huff_DC_Y_Length, Huff_AC_Y_Value, Huff_AC_Y_Length);
				_ReadHuffmanBit(X, Y, RLE_Cb, Huff_DC_C_Value, Huff_DC_C_Length, Huff_AC_C_Value, Huff_AC_C_Length);
				_ReadHuffmanBit(X, Y, RLE_Cr, Huff_DC_C_Value, Huff_DC_C_Length, Huff_AC_C_Value, Huff_AC_C_Length);
			}
		}
	}
public:
	Jpeg(const char* fname, BOOL isCompress)
	{
		if (isCompress)
		{
			char outfname[256] = "\0";
			strcpy_s(outfname, 256, fname);
			INT dot_pos;
			for (dot_pos = 255; dot_pos >= 0; dot_pos--)
			{
				if (outfname[dot_pos] == '.')
					break;
			}
			outfname[dot_pos] = '\0';
			strcat_s(outfname, ".jpeg");
			file.open(outfname, ios::binary | ios::out);
			if (!file)
			{
				cerr << "error:open file failed\n";
				exit(FILE_ERR);
			}
			img.readPic(fname);
			img.getData(source, x, y);
			divx = ((x + 7) / 8) * 8;
			divy = ((y + 7) / 8) * 8;
		}
		else
		{
			file.open(fname, ios::binary | ios::in);
			if (!file)
			{
				cerr << "error:open file failed\n";
				exit(FILE_ERR);
			}
			ReadHead();
			source = new(nothrow)BYTE[x * y * 4];
			if (source == nullptr)
			{
				cerr << "error:apply for memory failed\n";
				exit(MEM_ERR);
			}
			memset(source, 0, x * y * 4 * sizeof(BYTE));
		}
		div = new(nothrow)BYTE[divx * divy * 3];
		if (div == nullptr)
		{
			cerr << "error:apply for memory failed\n";
			exit(MEM_ERR);
		}
		memset(div, 0, divx * divy * 3 * sizeof(BYTE));
		YCC = new(nothrow)DOUBLE[divx * divy * 3];
		if (YCC == nullptr)
		{
			cerr << "error:apply for memory failed\n";
			exit(MEM_ERR);
		}
		memset(YCC, 0, divx * divy * 3 * sizeof(DOUBLE));
		DCT = new(nothrow)DOUBLE[divx * divy * 3];
		if (DCT == nullptr)
		{
			cerr << "error:apply for memory failed\n";
			exit(MEM_ERR);
		}
		memset(DCT, 0, divx * divy * 3 * sizeof(DOUBLE));
		quant = new(nothrow)WORD[divx * divy * 3];
		if (quant == nullptr)
		{
			cerr << "error:apply for memory failed\n";
			exit(MEM_ERR);
		}
		memset(quant, 0, divx * divy * 3 * sizeof(WORD));
		Zlist = new(nothrow)DOUBLE[divx * divy * 3];
		if (Zlist == nullptr)
		{
			cerr << "error:apply for memory failed\n";
			exit(MEM_ERR);
		}
		memset(Zlist, 0, divx * divy * 3 * sizeof(DOUBLE));
		RLE_Y = new(nothrow)SHORT[2 * divx * divy];
		if (RLE_Y == nullptr)
		{
			cerr << "error:apply for memory failed\n";
			exit(MEM_ERR);
		}
		memset(RLE_Y, 0, 2 * divx * divy * sizeof(SHORT));
		RLE_Cb = new(nothrow)SHORT[2 * divx * divy];
		if (RLE_Cb == nullptr)
		{
			cerr << "error:apply for memory failed\n";
			exit(MEM_ERR);
		}
		memset(RLE_Cb, 0, 2 * divx * divy * sizeof(SHORT));
		RLE_Cr = new(nothrow)SHORT[2 * divx * divy];
		if (RLE_Cr == nullptr)
		{
			cerr << "error:apply for memory failed\n";
			exit(MEM_ERR);
		}
		memset(RLE_Cr, 0, 2 * divx * divy * sizeof(SHORT));

		Huff_DC_Y_Value = new(nothrow)WORD[16];
		if (Huff_DC_Y_Value == nullptr)
		{
			cerr << "error:apply for memory failed\n";
			exit(MEM_ERR);
		}
		memset(Huff_DC_Y_Value, 0, 16 * sizeof(WORD));
		Huff_DC_Y_Length = new(nothrow)WORD[16];
		if (Huff_DC_Y_Length == nullptr)
		{
			cerr << "error:apply for memory failed\n";
			exit(MEM_ERR);
		}
		memset(Huff_DC_Y_Length, 0, 16 * sizeof(WORD));

		Huff_DC_C_Value = new(nothrow)WORD[16];
		if (Huff_DC_C_Value == nullptr)
		{
			cerr << "error:apply for memory failed\n";
			exit(MEM_ERR);
		}
		memset(Huff_DC_C_Value, 0, 16 * sizeof(WORD));
		Huff_DC_C_Length = new(nothrow)WORD[16];
		if (Huff_DC_C_Length == nullptr)
		{
			cerr << "error:apply for memory failed\n";
			exit(MEM_ERR);
		}
		memset(Huff_DC_C_Length, 0, 16 * sizeof(WORD));

		Huff_AC_Y_Value = new(nothrow)WORD[256];
		if (Huff_AC_Y_Value == nullptr)
		{
			cerr << "error:apply for memory failed\n";
			exit(MEM_ERR);
		}
		memset(Huff_AC_Y_Value, 0, 256 * sizeof(WORD));
		Huff_AC_Y_Length = new(nothrow)WORD[256];
		if (Huff_AC_Y_Length == nullptr)
		{
			cerr << "error:apply for memory failed\n";
			exit(MEM_ERR);
		}
		memset(Huff_AC_Y_Length, 0, 256 * sizeof(WORD));

		Huff_AC_C_Value = new(nothrow)WORD[256];
		if (Huff_AC_C_Value == nullptr)
		{
			cerr << "error:apply for memory failed\n";
			exit(MEM_ERR);
		}
		memset(Huff_AC_C_Value, 0, 256 * sizeof(WORD));
		Huff_AC_C_Length = new(nothrow)WORD[256];
		if (Huff_AC_C_Length == nullptr)
		{
			cerr << "error:apply for memory failed\n";
			exit(MEM_ERR);
		}
		memset(Huff_AC_C_Length, 0, 256 * sizeof(WORD));
		InitHuffmanTable();
		InitQualityTable();
	}
	~Jpeg()
	{
		file.close();
		delete[] source;
		delete[] div;
		delete[] YCC;
		delete[] DCT;
		delete[] quant;
		delete[] Zlist;
		delete[] RLE_Y;
		delete[] RLE_Cb;
		delete[] RLE_Cr;
		delete[] Huff_DC_Y_Value;
		delete[] Huff_DC_Y_Length;
		delete[] Huff_DC_C_Value;
		delete[] Huff_DC_C_Length;
		delete[] Huff_AC_Y_Value;
		delete[] Huff_AC_Y_Length;
		delete[] Huff_AC_C_Value;
		delete[] Huff_AC_C_Length;
	}
	void compress()
	{
		Div();
		RGB2YCC();
		YCC2DCT();
		Zigzag();
		Quantization();
		RLE();
		DPCM();
		WriteHead();
		WriteHuffmanBit();
		putWORD(0xFFD9);
	}
	void decompress()
	{
		ReadHuffmanBit();
		unDPCM();
		unRLE();
		unQuantization();
		unZigzag();
		DCT2YCC();
		YCC2RGB();
		unDiv();
		img.showPic(source, x, y);
	}
};
#if TEST
int main()
{
	cout << "TESTINE COMPRESS\n";
	{
		Jpeg j("lena.tiff", true);
		j.compress();
	}
	cout << "COMPRESS COMPLETE\n";

	cout << "TESTINE DECOMPRESS\n";
	{
		Jpeg j("lena.jpeg", false);
		j.decompress();
	}
	cout << "DECOMPRESS COMPLETE\n";
	return 0;
}
#else
int main(
	int argc,
	char* argv[]
)
{
	if (argc < 3)
	{
		cerr << "the program don't accept" << argc << "parameter(s), you must input two parameters\n";
		return 1;
	}
	if (!strcmp(argv[1], "-compress"))
	{
		Jpeg j(argv[2], true);
		j.compress();
		cout << "compress success\n";
	}
	else if (!strcmp(argv[1], "-read"))
	{
		Jpeg j(argv[2], false);
		j.decompress();
		cout << "read success\n";
	}
	else
	{
		cerr << "invalid commend, please input \"-compress\" or \"-read\"\n";
		return 2;
	}
	return 0;
}
#endif // TEST