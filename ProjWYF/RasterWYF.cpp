#include "StdAfx.h"
#include "RasterWYF.h"
typedef unsigned char DT_8U;//����8bit����
using namespace std;

CRasterWYF::CRasterWYF()
{	
}

CRasterWYF::~CRasterWYF()
{
}

//��ͼ�񲢵õ�������Ϣ
bool CRasterWYF::OpenRasterFile(CString strFilePath)
{
	GDALAllRegister();
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");  //֧������·��
	MyDataset = (GDALDataset *)GDALOpen(strFilePath, GA_ReadOnly);
	if(NULL == MyDataset)	
		return FALSE;

	nXSize = MyDataset -> GetRasterXSize();//��ȡ����
	nYSize = MyDataset -> GetRasterYSize();//��ȡ����
	
	iBandNum  = MyDataset -> GetRasterCount();//��ȡ������
	
	MyDataset -> GetGeoTransform(Transform);//��ȡ����任ϵ��
	
	return TRUE;
}

//bool ReadRowData(int nXOff, int nYOff, int nXSize, int nYSize, int nBandCount)
//{
//}

//��ȡ��ͼ��������Ϣ
BYTE CRasterWYF::ReadNodeData(int iBand, int iX, int iY)
{
	GDALRasterBand *pBand = MyDataset -> GetRasterBand(iBand);//��ȡ����
	GDALDataType pDT = MyDataset -> GetRasterBand(iBand) -> GetRasterDataType();//��ȡ������������
	dByte = 0.0;
	pData = (BYTE *)malloc(nXSize * nYSize * 1 * sizeof(BYTE));
	pData = &dByte;
	/*pData = NULL;
	pData = (double *)malloc(nXSize * nYSize * 1 * sizeof(double));*/
	CPLErr Ret = pBand -> RasterIO(GF_Read, iX, iY, 1, 1, pData, 1, 1, pDT, 0, 0);//��ȡ���ص��ֵ
	/*dByte = *pData;*/
	return dByte;//�������ص��ֵ
}

//�����������ת��Ϊͼ�������ϵ��
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

//�ر�ͼ�񼰾��
bool CRasterWYF::CloseRasterFile()
{
	GDALClose(MyDataset);
	return TRUE;
}



