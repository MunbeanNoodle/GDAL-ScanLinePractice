#include "StdAfx.h"
#include "VectorWYF.h"

CVectorWYF::CVectorWYF(void)
{
}

CVectorWYF::~CVectorWYF(void)
{
}

CScanLineWYF Scan1;

//打开矢量文件并得到基本信息
bool CVectorWYF::OpenVectorFile(CString strFilePath)
{
	GDALAllRegister();
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");  //支持中文路径
	rDataset = (GDALDataset *)GDALOpenEx(strFilePath, GDAL_OF_VECTOR, NULL, NULL, NULL);
	if(NULL == rDataset)	
		return FALSE;

	iLayerNum = rDataset -> GetLayerCount();//获取图层数

	OGRLayer *rLayer = rDataset -> GetLayer(0);//仅有一个图层？
	if(NULL == rLayer)
		return FALSE;

	SpatialRef = rLayer -> GetSpatialRef();//获取图层空间参考

	rGeoType = rLayer -> GetGeomType();//获取图层图元类型

	i64ObjectNum = rLayer -> GetFeatureCount();//获取图层对象个数

	//属性表
	OGRFeatureDefn *rAttribute = rLayer -> GetLayerDefn();
	AttTable.iFieldNum = rAttribute -> GetFieldCount();//字段数
	AttTable.enFieldType.clear();
	AttTable.strFieldName.clear();
	OGRFieldType TmpType;
	CString TmpName;
	for(int i = 0; i < AttTable.iFieldNum; i++)
	{
		OGRFieldDefn *tField = rAttribute -> GetFieldDefn(i);
		TmpType = tField -> GetType();
		TmpName = tField -> GetNameRef();
		AttTable.enFieldType.push_back(TmpType);
		AttTable.strFieldName.push_back(TmpName);
	}

	return TRUE;
}

//新建文件，创建属性表
bool CVectorWYF::CreateVectorFile(CString strFilePath)
{
	
	GDALDriver *wDriver = NULL;
	wDriver = GetGDALDriverManager() -> GetDriverByName("ESRI Shapefile");
	wDataset = wDriver -> Create(strFilePath, 0, 0, 0, GDT_Unknown, NULL);
	
	CPLSetConfigOption( "SHAPE_ENCODING", "" );//支持中文字段

	OGRLayer *wLayer = wDataset -> CreateLayer("NewLayer", SpatialRef, rGeoType, NULL); 
	if(NULL == wLayer)
		return FALSE;
	
	//创建属性表
	/*CPLSetConfigOption( "SHAPE_ENCODING", "" );*/
	for(int i = 0; i < AttTable.iFieldNum; i++)
	{
		OGRFieldDefn tField(AttTable.strFieldName[i], AttTable.enFieldType[i]);
		wLayer -> CreateField(&tField);
	}
	
	//添加新的字段
	vector<CString> strFieldNameNew;
	vector<OGRFieldType> enFieldTypeNew;
	
	strFieldNameNew.push_back("像元数");
	strFieldNameNew.push_back("最小灰度值");
	strFieldNameNew.push_back("最大灰度值");
	strFieldNameNew.push_back("均值");
	strFieldNameNew.push_back("均方差");
	strFieldNameNew.push_back("栅格面积");
	strFieldNameNew.push_back("矢量面积");

	for(int i = 0; i < 7; i++)
	{
		enFieldTypeNew.push_back(OFTReal);
	}

	for(int i = 0; i < 7; i++)
	{
		OGRFieldDefn tField(strFieldNameNew[i], enFieldTypeNew[i]);
		wLayer -> CreateField(&tField);
	}


	return TRUE;
	
}

//获取对象信息
bool CVectorWYF::ReadObject(INT64 i64ObjID)
{
	OGRLayer *rLayer = rDataset -> GetLayer(0);
	OGRFeature *rFeature = rLayer -> GetFeature(i64ObjID);
	if(NULL == rFeature)
		return FALSE;
	
	//获取属性信息
	PolygonAttData.clear();
	for(int i = 0; i < AttTable.iFieldNum; i++)
	{
		PolygonAttData.push_back(rFeature -> GetFieldAsString(i));
	}
	
	//获取空间信息
	OGRGeometry *rGeometry = rFeature -> GetGeometryRef();
	OGRwkbGeometryType type = wkbFlatten(rGeometry -> getGeometryType());
	OGRPolygon *rOGRPolygon = (OGRPolygon *)rGeometry;

	MY_POINT TmpPoint;
	MY_RING TmpExRing;
	
	//外环
	OGRLinearRing *rRing = rOGRPolygon -> getExteriorRing();
	TmpExRing.iPtNum = rRing -> getNumPoints();
	TmpExRing.Pts.clear();
	for(int i = 0; i < TmpExRing.iPtNum; i++)
	{
		TmpPoint.dX = rRing -> getX(i);
		TmpPoint.dY = rRing -> getY(i);
		TmpExRing.Pts.push_back(TmpPoint);
	}
	PolygonGeoData.Rings.push_back(TmpExRing);

	//内环
	MY_RING TmpInRing;
	PolygonGeoData.iRingNum = rOGRPolygon -> getNumInteriorRings() + 1;
	//PolygonGeoData.InRings.clear();
	for(int i = 1; i < PolygonGeoData.iRingNum; i++)
	{
		OGRLinearRing *rInRing = rOGRPolygon -> getInteriorRing(i - 1);
		TmpInRing.iPtNum = rInRing -> getNumPoints();
		TmpInRing.Pts.clear();
		for(int j = 0; j < TmpInRing.iPtNum; j++)
		{
			TmpPoint.dX = rInRing -> getX(j);
			TmpPoint.dY = rInRing -> getY(j);
			TmpInRing.Pts.push_back(TmpPoint);
		}
		PolygonGeoData.Rings.push_back(TmpInRing);
	}
	return TRUE;
}

bool CVectorWYF::WriteObject(
		int m_iPixelNum,
		double m_bMinPixelValue,
		double m_bMaxPixelValue,
		double m_dPixelMeanValue,
		double m_dPixelMeanSquareValue,
		double m_dRasterArea,
		double m_dVectorArea
		)
{
	OGRLayer *wLayer = wDataset -> GetLayer(0);
	OGRFeature *wFeature = NULL;
	wFeature = OGRFeature::CreateFeature(wLayer -> GetLayerDefn());
	
	//写入属性信息
	for(int i = 0; i < AttTable.iFieldNum; i++)
	{	
		//旧信息复制
		wFeature -> SetField(AttTable.strFieldName[i], PolygonAttData[i]);
		//新字段赋值
		wFeature -> SetField("像元数", m_iPixelNum);
		wFeature -> SetField("最小灰度值", m_bMinPixelValue);
		wFeature -> SetField("最大灰度值", m_bMaxPixelValue);
		wFeature -> SetField("均值", m_dPixelMeanValue);
		wFeature -> SetField("均方差", m_dPixelMeanSquareValue);
		wFeature -> SetField("栅格面积", m_dRasterArea);
		wFeature -> SetField("矢量面积", m_dVectorArea);
	}
	

	//写入旧的空间信息
	OGRPolygon TmpPolygon;
	TmpPolygon.empty();
	
	//先外环后内环
	OGRLinearRing TmpRing;
	for(int i = 0; i < PolygonGeoData.Rings[0].iPtNum; i++)
	{
		TmpRing.addPoint(PolygonGeoData.Rings[0].Pts[i].dX, PolygonGeoData.Rings[0].Pts[i].dY);
	}
	TmpRing.closeRings();
	TmpPolygon.addRing(&TmpRing);
	
	for(int i = 1; i < PolygonGeoData.iRingNum; i++)
	{
		TmpRing.empty();
		for(int j = 0; j < PolygonGeoData.Rings[i].iPtNum; j++)
		{
			TmpRing.addPoint(PolygonGeoData.Rings[i].Pts[j].dX, PolygonGeoData.Rings[i].Pts[j].dY);
		}
		TmpRing.closeRings();
		TmpPolygon.addRing(&TmpRing);
	}

	OGRErr Ret = wFeature -> SetGeometry(&TmpPolygon);//
	Ret = wLayer -> CreateFeature (wFeature);//
	
	return TRUE;
}

bool CVectorWYF::CloseVectorFile()
{
	GDALClose(rDataset);
	GDALClose(wDataset);
	return TRUE;
}