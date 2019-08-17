#include "StdAfx.h"
#include "ScanLine.h"
#include "math.h"
#include <algorithm>
using namespace std;


CScanLineWYF::CScanLineWYF()
{
	//������ʼ��
	
	
	m_iPixelValueSum =0;//������ڲ�����ֵ�ĺ�
	m_iPixelValueSquareSum = 0;//������ڲ�����ֵ��ƽ����
	m_iPixelNum = 0;//������ڲ���Ԫ����
	
	//������
	m_bMaxPixelValue = 0;//���Ҷ�ֵ
	m_bMinPixelValue = 0;//��С�Ҷ�ֵ
	m_dPixelMeanValue = 0;//������ڲ����ص�ƽ��ֵ
	m_dPixelMeanSquareValue = 0;//������ڲ����صľ�����
	m_dRasterArea = 0;//դ�����
	m_dVectorArea = 0;//ʸ�����
	
}

CScanLineWYF::~CScanLineWYF()
{

}

CRasterWYF RasterFile;
CVectorWYF VectorFile;

bool Compare(const Edge &Edge1, const Edge &Edge2)
{
	if(Edge1.X == Edge2.X)
		return Edge1.deltaX < Edge2.deltaX;
	else
		return Edge1.X < Edge2.X;
}

bool CScanLineWYF::InputFilePath(
	CString RasterFilePath, //դ���ļ�·��
	CString VectorFilePath, //ʸ���ļ�·��
	CString OutputPath, //���·��
	int iBand, //ѡȡ�Ĳ��α��
	Scan_Method  ScanMethod //�ж�����һ�ַ�ʽ����ɨ��
	)
{
	
	RasterFile.OpenRasterFile(RasterFilePath);
	VectorFile.OpenVectorFile(VectorFilePath);
	//��RasterFile.Transform[6]ת��Ϊ���������Ϊͼ�������ϵ�����洢��m_TransformCoeffcient[6]��
	RasterFile.ComputeTran();
	m_ScanMethod = ScanMethod;
	VectorFile.CreateVectorFile(OutputPath);
	m_iBandNo = iBand;


	for(INT64 i = 0; i < VectorFile.i64ObjectNum; i++)
	{
		//��������Ҫ������ʸ����Ϣ��ʼ��
		StartScanLine(i);//��ʼ��i�������ɨ����ɨ��
		//����ɨ��õ�������,���������ֵ�ܺ������ֵ�ȵ�
		m_dPixelMeanValue = m_iPixelValueSum / m_iPixelNum;//������ڲ����ص�ƽ��ֵ
		m_dPixelMeanSquareValue = sqrt(m_iPixelValueSquareSum / m_iPixelNum - sqrt(m_dPixelMeanValue));//������ڲ����صľ�����
		m_dRasterArea = m_iPixelNum * 1.0 * 1.0;//դ�����
		m_dVectorArea = fabs(m_dVectorArea);

		VectorFile.WriteObject(
			m_iPixelNum,
			m_bMinPixelValue,
			m_bMaxPixelValue,
			m_dPixelMeanValue,
			m_dPixelMeanSquareValue,
			m_dRasterArea,
			m_dVectorArea
			);	
	}
	RasterFile.CloseRasterFile();
	VectorFile.CloseVectorFile();
	
	int x = 0;
	return TRUE;
}

bool CScanLineWYF::StartScanLine(INT64 i64ObjectNo)
{
	//��ʼ��
	m_ET.clear();
	m_AEL.clear();
	m_iHL.clear();
	
	//��ȡ��i64ObjectNum������Ŀռ���Ϣ
	VectorFile.ReadObject(i64ObjectNo); 
	
	//�������л����������⻷�����ȥ�����ڻ������ʸ�����
	double Area = 0.0;
	for(int i = 0; i < VectorFile.PolygonGeoData.iRingNum; i++)
		for(int j = 0; j < VectorFile.PolygonGeoData.Rings[i].iPtNum - 1; j++)
		{
			Area += (VectorFile.PolygonGeoData.Rings[i].Pts[j].dX-VectorFile.PolygonGeoData.Rings[i].Pts[j+1].dX)
			       *(VectorFile.PolygonGeoData.Rings[i].Pts[j].dY+VectorFile.PolygonGeoData.Rings[i].Pts[j+1].dY) / 2; //�������������ת��֮ǰ�����ʸ�����
		}
	m_PolygonArea.push_back(Area);
	m_dVectorArea = m_PolygonArea[0];//ʸ�����
	for(int i = 1; i < VectorFile.PolygonGeoData.iRingNum; i++)
		m_dVectorArea -= m_PolygonArea[i];

	//�ɵ�������ת����ͼ������
	double TmpX, TmpY;
	for(int i = 0; i < VectorFile.PolygonGeoData.iRingNum; i++)
	{
		for(int j = 0; j < VectorFile.PolygonGeoData.Rings[i].iPtNum; j++)
		{	
			TmpX = VectorFile.PolygonGeoData.Rings[i].Pts[j].dX;
			TmpY = VectorFile.PolygonGeoData.Rings[i].Pts[j].dY;
			VectorFile.PolygonGeoData.Rings[i].Pts[j].dX = RasterFile.AfterTransform[0] * TmpX + RasterFile.AfterTransform[1] * TmpY + RasterFile.AfterTransform[2]+0.5;
			VectorFile.PolygonGeoData.Rings[i].Pts[j].dY = RasterFile.AfterTransform[3] * TmpX + RasterFile.AfterTransform[4] * TmpY + RasterFile.AfterTransform[5]+0.5;
		
			if(fabs(VectorFile.PolygonGeoData.Rings[i].Pts[j].dX - (int)(VectorFile.PolygonGeoData.Rings[i].Pts[j].dX + 0.5)) < 1e-4)
				VectorFile.PolygonGeoData.Rings[i].Pts[j].dX = (int)(VectorFile.PolygonGeoData.Rings[i].Pts[j].dX + 0.5);
			if(fabs(VectorFile.PolygonGeoData.Rings[i].Pts[j].dY - (int)(VectorFile.PolygonGeoData.Rings[i].Pts[j].dY + 0.5)) < 1e-4)
				VectorFile.PolygonGeoData.Rings[i].Pts[j].dY = (int)(VectorFile.PolygonGeoData.Rings[i].Pts[j].dY + 0.5);
		}
	}

	//����ET
	//����͡���ߵ�Yֵ
	int minY;
	double MINY = VectorFile.PolygonGeoData.Rings[0].Pts[0].dY;
	double MAXY = VectorFile.PolygonGeoData.Rings[0].Pts[0].dY;
	for(int i = 0; i < VectorFile.PolygonGeoData.Rings[0].iPtNum; i++)
	{
		if(VectorFile.PolygonGeoData.Rings[0].Pts[i].dY < MINY)
			MINY = VectorFile.PolygonGeoData.Rings[0].Pts[i].dY;
		if(VectorFile.PolygonGeoData.Rings[0].Pts[i].dY > MAXY)
			MAXY = VectorFile.PolygonGeoData.Rings[0].Pts[i].dY;
	}
	minY = ceil(MINY);
	
	m_ET.resize((int)MAXY - (int)MINY + 2);
	
	Edge TmpEdge;
	Horizon_Line TmpHL;
	double XP, XPO, YP, YPO;
	double TmpXS, TmpYS, TmpYL;
	for(int i = 0; i < VectorFile.PolygonGeoData.iRingNum; i++)
	{	
		for(int j = 0; j < VectorFile.PolygonGeoData.Rings[i].iPtNum - 1; j++)
		{
			XP = VectorFile.PolygonGeoData.Rings[i].Pts[j].dX;//ǰһ�������
			XPO = VectorFile.PolygonGeoData.Rings[i].Pts[j+1].dX;//��һ�������
			
			YP = VectorFile.PolygonGeoData.Rings[i].Pts[j].dY;//ǰһ��������
			YPO = VectorFile.PolygonGeoData.Rings[i].Pts[j+1].dY;//��һ��������
			
			
			//б�߻�����ֱ�ߣ��Ҳ���������ɨ�����м�
			if(YP != YPO && (int)YP != (int)YPO)
			{
				TmpEdge.deltaX = (XP - XPO) / (YP - YPO);
				if(YP < YPO)
				{
					TmpXS = XP;
					TmpYS = YP;
					TmpYL = YPO;
				}
				else
				{
					TmpXS = XPO;
					TmpYS = YPO;
					TmpYL = YP;
				}	
				TmpEdge.X = TmpX + TmpEdge.deltaX * (ceil(TmpYS) - TmpY);
				TmpEdge.YMax = TmpYL;
				m_ET[ceil(TmpYS) - minY].ET_Edge.push_back(TmpEdge);//��ȥminY��ʹvector�±��0��ʼ
				sort(m_ET[ceil(TmpYS) - minY].ET_Edge.begin(), m_ET[ceil(TmpYS) - minY].ET_Edge.end(), Compare);
			}
			
			//YΪ������ˮƽ�߱�m_iHL
			if(YP == YPO == (int)YP)
			{
				TmpHL.X1 = XP;
				TmpHL.X2 = XPO;
				m_iHL[(int)XP - minY].HL.push_back(TmpHL);
			}
		}
	}
	
	//AEL
	m_AEL.clear();
	for(int i = minY; i <= MAXY; i++)//����СYֵ��ʼ�������Yֵ����
	{	
		//ɾ��YMax����ɨ���ߵı�
		for(int j = 0; j < m_AEL.size(); j++)
		{
			if(i == m_AEL[j].YMax)
				m_AEL.erase(m_AEL.begin() + j);
		}

		//�����±�
		if(!m_ET[i - minY].ET_Edge.empty())
		{	
			for(int j = 0; j < m_ET[i - minY].ET_Edge.size(); j++)
				m_AEL.push_back(m_ET[i - minY].ET_Edge[j]);
		}

		//��������
		for(int j = 0; j < m_AEL.size() - 1; j += 2)
		{
			ChuLiQuDuan(m_AEL[j].X, m_AEL[j + 1].X, i);
		}
		
		for(int j = 0; j < m_AEL.size() - 1; j++)
			m_AEL[j].X += m_AEL[j].deltaX;//��һ��
	}

	return TRUE;

}


bool CScanLineWYF::ChuLiQuDuan(double X1, double X2, int Y)
{
	/*if
		
	RasterFile.ReadNodeData(m_iBandNo, ceil(X1), Y);*/
	m_bMinPixelValue = m_bMaxPixelValue = RasterFile.dByte;
	
	for(int i = ceil(X1); i <= floor(X2); i++)
	{
		RasterFile.ReadNodeData(m_iBandNo, i, Y);
		//�Ҷ�ֵ��
		m_iPixelValueSum += RasterFile.dByte;
		//�Ҷ�ֵƽ����
		m_iPixelValueSquareSum += RasterFile.dByte * RasterFile.dByte;
		//��Ԫ����
		m_iPixelNum ++;
		
		
		
		//��С�Ҷ�ֵ
		if(RasterFile.dByte < m_bMinPixelValue)
			m_bMinPixelValue = RasterFile.dByte;
		//���Ҷ�ֵ
		if(RasterFile.dByte > m_bMaxPixelValue)
			m_bMaxPixelValue = RasterFile.dByte;
	}
	return TRUE;
}


//AEL = NULL
//for(i = ymin     )
//ɾ���߶�
//�����߶�
//ͳ������