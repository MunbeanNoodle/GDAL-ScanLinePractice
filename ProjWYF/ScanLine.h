#pragma once
#include"RasterWYF.h"
#include"VectorWYF.h"

#include<vector>
using namespace std;

//扫描类型枚举
typedef enum tag_Scan_Method
{
	Border_Not_Count_In, //落在边界上的点不算入多边形内部
	Border_Count_In //落在边界上的点算入多边形内部
}Scan_Method;

//边的结构体
typedef struct tag_Edge
{
	double YMax;//该边Y的最大值
	double X;//记录当前X的位置，初始值为X = minX + deltaX * ((int)minY + 1 - minY)
	double deltaX;//斜率的倒数，deltaX = (X[i]-X[i+1])/(Y[i]-Y[i+1])
}Edge;

//ET表的结构体
typedef struct tag_ET
{
	vector<Edge> ET_Edge;//当前ET中存储的边的信息
}ET;

//Y值为整数的水平边的结构体
typedef struct tag_Horizon_Line
{
	double X1;//左侧点的X值
	double X2;//右侧点的X值
}Horizon_Line;

typedef struct tag_HL_ET
{
	//int Y;//当前ET对应的Y值
    vector<Horizon_Line> HL;//当前ET存储的平行边
}HL_ET;

class CScanLineWYF
{
public:
	CScanLineWYF();
	~CScanLineWYF();

private:
	vector<ET> m_ET;//ET表
	vector<Edge> m_AEL;//活化边表
	vector<HL_ET> m_iHL;//Y值为整型的水平边的ET表;

	double m_TransformCoeffcient[6]; //记录坐标转换的系数
	Scan_Method m_ScanMethod;//记录扫描方式的标识
	int m_iBandNo;

	//为矢量数据新增的属性信息，每完成一个多边形初始化一次
	vector<double>m_PolygonArea;//外环和内环的面积，其中外环的面积存储在0的位置
	int m_iPixelValueSum;//多边形内部像素值的和
	int m_iPixelValueSquareSum;//多边形内部像素值的平方和
	


	bool StartScanLine(INT64 i64ObjectNo);//扫描第i个对象
	bool ChuLiQuDuan(double X1, double X2, int Y);

public:
	int m_iPixelNum;//多边形内部像元数量
	double m_bMinPixelValue;//最小灰度值
	double m_bMaxPixelValue;//最大灰度值
	double m_dPixelMeanValue;//多边形内部像素的平均值
	double m_dPixelMeanSquareValue;//多边形内部像素的均方差
	double m_dRasterArea;//栅格面积
	double m_dVectorArea;//矢量面积
	bool InputFilePath(
		// 参数包括：
		CString RasterFilePath, //栅格文件路径
		CString VectorFilePath, //矢量文件路径
		CString OutputPath, //输出路径
		int iBand,//选取的波段编号
		Scan_Method  ScanMethod//判断以哪一种方式进行扫描
		);
};