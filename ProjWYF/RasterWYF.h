#pragma once
#include "gdal/include/gdal_priv.h"
#include "gdal/include/ogrsf_frmts.h"
#pragma comment(lib,"gdal/gdal_i.lib")


class CRasterWYF
{
public:
	CRasterWYF();
	~CRasterWYF();

	GDALDataset *MyDataset;//���
	
	int nXSize;//����
	int nYSize;//����
	
	int iBandNum;//���θ���

	double Transform[6];//����任ϵ��
	double AfterTransform[6];//ת�����ϵ��
	//CString strProjection;
	BYTE *pData;
	BYTE dByte;

	bool OpenRasterFile(CString strFilePath);//��ͼ�񲢵õ�������Ϣ

	bool CloseRasterFile();//�ر�ͼ�񼰾��

	//bool ReadRowData(int nXOff, int nYOff, int nXSize, int nYSize, int nBandCount);//��ȡһ��ͼ��������Ϣ

	BYTE ReadNodeData(int iBand, int iX, int iY);//��ȡ��ͼ��������Ϣ

	bool ComputeTran();//�������任ϵ��

};
