#include "StdAfx.h"
#include "RasterWYF.h"
typedef unsigned char DT_8U;//定义8bit数据
using namespace std;

CRasterWYF::CRasterWYF()
{	
}

CRasterWYF::~CRasterWYF()
{
}

//打开图像并得到基本信息
bool CRasterWYF::OpenRasterFile(CString strFilePath)
{
	GDALAllRegister();
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");  //支持中文路径
	MyDataset = (GDALDataset *)GDALOpen(strFilePath, GA_ReadOnly);
	if(NULL == MyDataset)	
		return FALSE;

	nXSize = MyDataset -> GetRasterXSize();//获取列数
	nYSize = MyDataset -> GetRasterYSize();//获取行数
	
	iBandNum  = MyDataset -> GetRasterCount();//获取波段数
	
	MyDataset -> GetGeoTransform(Transform);//获取仿射变换系数
	
	return TRUE;
}

//bool ReadRowData(int nXOff, int nYOff, int nXSize, int nYSize, int nBandCount)
//{
//}

//读取点图像像素信息
BYTE CRasterWYF::ReadNodeData(int iBand, int iX, int iY)
{
	GDALRasterBand *pBand = MyDataset -> GetRasterBand(iBand);//获取波段
	GDALDataType pDT = MyDataset -> GetRasterBand(iBand) -> GetRasterDataType();//获取像素数据类型
	dByte = 0.0;
	pData = (BYTE *)malloc(nXSize * nYSize * 1 * sizeof(BYTE));
	pData = &dByte;
	/*pData = NULL;
	pData = (double *)malloc(nXSize * nYSize * 1 * sizeof(double));*/
	CPLErr Ret = pBand -> RasterIO(GF_Read, iX, iY, 1, 1, pData, 1, 1, pDT, 0, 0);//获取像素点的值
	/*dByte = *pData;*/
	return dByte;//返回像素点的值
}

//计算地理坐标转换为图像坐标的系数
bool  CRasterWYF::ComputeTran()
{
	double C1 = Transform[1] * Transform[5] - Transform[2] * Transform[4]; 
	double C2 = Transform[0] * Transform[5] - Transform[2] * Transform[3];
	double C3 = Transform[0] * Transform[4] - Transform[1] * Transform[3];
	if(C1 == 0)
		return FALSE;
	AfterTransform[0] =  Transform[5] / C1;
	AfterTransform[1] = -Transform[2] / C1;
	AfterTransform[2] = -C2 / C1;
	AfterTransform[3] = -Transform[4] / C1;
	AfterTransform[4] =  Transform[1] / C1;
	AfterTransform[5] =  C3 / C1;
	return TRUE;
}

//关闭图像及句柄
bool CRasterWYF::CloseRasterFile()
{
	GDALClose(MyDataset);
	return TRUE;
}



