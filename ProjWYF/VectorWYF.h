#pragma once
#include "gdal/include/gdal_priv.h"
#include "gdal/include/ogrsf_frmts.h"
#pragma comment(lib,"gdal/gdal_i.lib")

#include<vector>
#include"ScanLine.h"
using namespace std;

//---���Ա�ṹ��--------------------------------------------------------------------------------------
typedef struct tag_ATT_TABLE
{
	int iFieldNum;	//�ֶθ���					
	vector<OGRFieldType> enFieldType;	//�ֶ�����
	vector<CString> strFieldName;    //�ֶ�����

}ATT_TABLE;

//---�ڵ�����ṹ��--------------------------------------------------------------------------------------
typedef struct tag_MY_POINT
{
	double dX;
	double dY;
}MY_POINT;

//---�����������Ϣ�ṹ��--------------------------------------------------------------------------------------
typedef struct tag_MY_RING
{
	int iPtNum;     //�ڵ����
	vector<MY_POINT> Pts;  //�洢�ڵ������
}MY_RING;

//--�������Ϣ�ṹ��--------------------------------------------------------------------------------------
typedef struct tag_MY_POLYGON  
{
	vector<MY_RING> Rings;   //���л����⻷���±�Ϊ0��λ��
	int iRingNum;    //���ĸ���
}MY_POLYGON;

class CVectorWYF
{
public:
	CVectorWYF();
	~CVectorWYF();
	
	int iLayerNum;

	OGRSpatialReference *SpatialRef;	 //�ռ�ο�

	ATT_TABLE AttTable;   //���Ա�ṹ

	INT64 i64ObjectNum;  //�������

	MY_POLYGON PolygonGeoData;   //��������ռ���Ϣ

	vector<CString> PolygonAttData;    //�ռ����������Ϣ

	OGRwkbGeometryType rGeoType;	//�򿪵�ʸ���ļ���ͼ��ͼԪ����
	
	bool OpenVectorFile(CString strFilePath);   //��ʸ���ļ�����ȡ�ռ�ο������Ա��������

	bool CreateVectorFile(CString strFilePath);   //�½��ļ�����������ͼ���������Ϣ�Լ��ռ�����������Ϣ

	bool ReadObject(INT64 i64ObjID);  //ͨ��Dataset�������������Ż�ȡ��ռ���Ϣ  0<=i64ObjID<=i64ObjectNum-1  1.get ID 2.

	bool WriteObject(
		int m_iPixelNum,//������ڲ���Ԫ����
		double m_bMinPixelValue,//��С�Ҷ�ֵ
		double m_bMaxPixelValue,//���Ҷ�ֵ
		double m_dPixelMeanValue,//������ڲ����ص�ƽ��ֵ
		double m_dPixelMeanSquareValue,//������ڲ����صľ�����
		double m_dRasterArea,//դ�����
		double m_dVectorArea//ʸ�����
		);   //����ȡ�ĵ����ռ������Ϣ���Ƶ��´������ļ���,д��ͳ����Ϣ

	bool CloseVectorFile();  //�ر�ʸ���ļ�

private:
	GDALDataset *rDataset; //��ȡ��ʸ���ļ�
	
	GDALDataset *wDataset;  //�½���ʸ���ļ�


};
