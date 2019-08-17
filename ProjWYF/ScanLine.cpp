#include "StdAfx.h"
#include "ScanLine.h"
#include "math.h"
#include <algorithm>
using namespace std;


CScanLineWYF::CScanLineWYF()
{
	//变量初始化
	
	
	m_iPixelValueSum =0;//多边形内部像素值的和
	m_iPixelValueSquareSum = 0;//多边形内部像素值的平方和
	m_iPixelNum = 0;//多边形内部像元数量
	
	//有问题
	m_bMaxPixelValue = 0;//最大灰度值
	m_bMinPixelValue = 0;//最小灰度值
	m_dPixelMeanValue = 0;//多边形内部像素的平均值
	m_dPixelMeanSquareValue = 0;//多边形内部像素的均方差
	m_dRasterArea = 0;//栅格面积
	m_dVectorArea = 0;//矢量面积
	
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
	CString RasterFilePath, //栅格文件路径
	CString VectorFilePath, //矢量文件路径
	CString OutputPath, //输出路径
	int iBand, //选取的波段编号
	Scan_Method  ScanMethod //判断以哪一种方式进行扫描
	)
{
	
	RasterFile.OpenRasterFile(RasterFilePath);
	VectorFile.OpenVectorFile(VectorFilePath);
	//将RasterFile.Transform[6]转换为地理坐标变为图像坐标的系数，存储在m_TransformCoeffcient[6]中
	RasterFile.ComputeTran();
	m_ScanMethod = ScanMethod;
	VectorFile.CreateVectorFile(OutputPath);
	m_iBandNo = iBand;


	for(INT64 i = 0; i < VectorFile.i64ObjectNum; i++)
	{
		//将所有需要新增的矢量信息初始化
		StartScanLine(i);//开始第i个对象的扫描线扫描
		//处理扫描得到的数据,如根据像素值总和算出均值等等
		m_dPixelMeanValue = m_iPixelValueSum / m_iPixelNum;//多边形内部像素的平均值
		m_dPixelMeanSquareValue = sqrt(m_iPixelValueSquareSum / m_iPixelNum - sqrt(m_dPixelMeanValue));//多边形内部像素的均方差
		m_dRasterArea = m_iPixelNum * 1.0 * 1.0;//栅格面积
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
	//初始化
	m_ET.clear();
	m_AEL.clear();
	m_iHL.clear();
	
	//读取第i64ObjectNum个对象的空间信息
	VectorFile.ReadObject(i64ObjectNo); 
	
	//计算所有环面积，最后外环面积减去所有内环面积得矢量面积
	double Area = 0.0;
	for(int i = 0; i < VectorFile.PolygonGeoData.iRingNum; i++)
		for(int j = 0; j < VectorFile.PolygonGeoData.Rings[i].iPtNum - 1; j++)
		{
			Area += (VectorFile.PolygonGeoData.Rings[i].Pts[j].dX-VectorFile.PolygonGeoData.Rings[i].Pts[j+1].dX)
			       *(VectorFile.PolygonGeoData.Rings[i].Pts[j].dY+VectorFile.PolygonGeoData.Rings[i].Pts[j+1].dY) / 2; //这个操作在坐标转换之前，算出矢量面积
		}
	m_PolygonArea.push_back(Area);
	m_dVectorArea = m_PolygonArea[0];//矢量面积
	for(int i = 1; i < VectorFile.PolygonGeoData.iRingNum; i++)
		m_dVectorArea -= m_PolygonArea[i];

	//由地理坐标转换到图像坐标
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

	//建立ET
	//求最低、最高点Y值
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
			XP = VectorFile.PolygonGeoData.Rings[i].Pts[j].dX;//前一点横坐标
			XPO = VectorFile.PolygonGeoData.Rings[i].Pts[j+1].dX;//后一点横坐标
			
			YP = VectorFile.PolygonGeoData.Rings[i].Pts[j].dY;//前一点纵坐标
			YPO = VectorFile.PolygonGeoData.Rings[i].Pts[j+1].dY;//后一点纵坐标
			
			
			//斜边或者竖直边，且不夹在两条扫描线中间
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
				m_ET[ceil(TmpYS) - minY].ET_Edge.push_back(TmpEdge);//减去minY，使vector下标从0开始
				sort(m_ET[ceil(TmpYS) - minY].ET_Edge.begin(), m_ET[ceil(TmpYS) - minY].ET_Edge.end(), Compare);
			}
			
			//Y为整数的水平边表m_iHL
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
	for(int i = minY; i <= MAXY; i++)//从最小Y值开始，到最大Y值结束
	{	
		//删除YMax等于扫描线的边
		for(int j = 0; j < m_AEL.size(); j++)
		{
			if(i == m_AEL[j].YMax)
				m_AEL.erase(m_AEL.begin() + j);
		}

		//加入新边
		if(!m_ET[i - minY].ET_Edge.empty())
		{	
			for(int j = 0; j < m_ET[i - minY].ET_Edge.size(); j++)
				m_AEL.push_back(m_ET[i - minY].ET_Edge[j]);
		}

		//构建区段
		for(int j = 0; j < m_AEL.size() - 1; j += 2)
		{
			ChuLiQuDuan(m_AEL[j].X, m_AEL[j + 1].X, i);
		}
		
		for(int j = 0; j < m_AEL.size() - 1; j++)
			m_AEL[j].X += m_AEL[j].deltaX;//下一条
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
		//灰度值和
		m_iPixelValueSum += RasterFile.dByte;
		//灰度值平方和
		m_iPixelValueSquareSum += RasterFile.dByte * RasterFile.dByte;
		//像元数和
		m_iPixelNum ++;
		
		
		
		//最小灰度值
		if(RasterFile.dByte < m_bMinPixelValue)
			m_bMinPixelValue = RasterFile.dByte;
		//最大灰度值
		if(RasterFile.dByte > m_bMaxPixelValue)
			m_bMaxPixelValue = RasterFile.dByte;
	}
	return TRUE;
}


//AEL = NULL
//for(i = ymin     )
//删除线段
//插入线段
//统计区段