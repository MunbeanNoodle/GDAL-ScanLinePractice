#pragma once
#include "gdal/include/gdal_priv.h"
#include "gdal/include/ogrsf_frmts.h"
#pragma comment(lib,"gdal/gdal_i.lib")


class CRasterWYF
{
public:
	CRasterWYF();
	~CRasterWYF();

	GDALDataset *MyDataset;//句柄
	
	int nXSize;//列数
	int nYSize;//行数
	
	int iBandNum;//波段个数

	double Transform[6];//仿射变换系数
	double AfterTransform[6];//转换后的系数
	//CString strProjection;
	BYTE *pData;
	BYTE dByte;

	bool OpenRasterFile(CString strFilePath);//打开图像并得到基本信息

	bool CloseRasterFile();//关闭图像及句柄

	//bool ReadRowData(int nXOff, int nYOff, int nXSize, int nYSize, int nBandCount);//读取一行图像像素信息

	BYTE ReadNodeData(int iBand, int iX, int iY);//读取点图像像素信息

	bool ComputeTran();//计算仿射变换系数

};
