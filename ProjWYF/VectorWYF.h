#pragma once
#include "gdal/include/gdal_priv.h"
#include "gdal/include/ogrsf_frmts.h"
#pragma comment(lib,"gdal/gdal_i.lib")

#include<vector>
#include"ScanLine.h"
using namespace std;

//---属性表结构体--------------------------------------------------------------------------------------
typedef struct tag_ATT_TABLE
{
	int iFieldNum;	//字段个数					
	vector<OGRFieldType> enFieldType;	//字段类型
	vector<CString> strFieldName;    //字段名称

}ATT_TABLE;

//---节点坐标结构体--------------------------------------------------------------------------------------
typedef struct tag_MY_POINT
{
	double dX;
	double dY;
}MY_POINT;

//---单个多边形信息结构体--------------------------------------------------------------------------------------
typedef struct tag_MY_RING
{
	int iPtNum;     //节点个数
	vector<MY_POINT> Pts;  //存储节点的容器
}MY_RING;

//--多边形信息结构体--------------------------------------------------------------------------------------
typedef struct tag_MY_POLYGON  
{
	vector<MY_RING> Rings;   //所有环，外环在下标为0的位置
	int iRingNum;    //环的个数
}MY_POLYGON;

class CVectorWYF
{
public:
	CVectorWYF();
	~CVectorWYF();
	
	int iLayerNum;

	OGRSpatialReference *SpatialRef;	 //空间参考

	ATT_TABLE AttTable;   //属性表结构

	INT64 i64ObjectNum;  //对象个数

	MY_POLYGON PolygonGeoData;   //单个对象空间信息

	vector<CString> PolygonAttData;    //空间对象属性信息

	OGRwkbGeometryType rGeoType;	//打开的矢量文件的图层图元类型
	
	bool OpenVectorFile(CString strFilePath);   //打开矢量文件，获取空间参考，属性表，对象个数

	bool CreateVectorFile(CString strFilePath);   //新建文件，加入整个图层的属性信息以及空间对象的属性信息

	bool ReadObject(INT64 i64ObjID);  //通过Dataset句柄，输入对象编号获取其空间信息  0<=i64ObjID<=i64ObjectNum-1  1.get ID 2.

	bool WriteObject(
		int m_iPixelNum,//多边形内部像元数量
		double m_bMinPixelValue,//最小灰度值
		double m_bMaxPixelValue,//最大灰度值
		double m_dPixelMeanValue,//多边形内部像素的平均值
		double m_dPixelMeanSquareValue,//多边形内部像素的均方差
		double m_dRasterArea,//栅格面积
		double m_dVectorArea//矢量面积
		);   //将读取的单个空间对象信息复制到新创建的文件中,写入统计信息

	bool CloseVectorFile();  //关闭矢量文件

private:
	GDALDataset *rDataset; //读取的矢量文件
	
	GDALDataset *wDataset;  //新建的矢量文件


};
