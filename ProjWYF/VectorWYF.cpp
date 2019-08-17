#include "StdAfx.h"
#include "VectorWYF.h"

CVectorWYF::CVectorWYF(void)
{
}

CVectorWYF::~CVectorWYF(void)
{
}

CScanLineWYF Scan1;

//��ʸ���ļ����õ�������Ϣ
bool CVectorWYF::OpenVectorFile(CString strFilePath)
{
	GDALAllRegister();
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");  //֧������·��
	rDataset = (GDALDataset *)GDALOpenEx(strFilePath, GDAL_OF_VECTOR, NULL, NULL, NULL);
	if(NULL == rDataset)	
		return FALSE;

	iLayerNum = rDataset -> GetLayerCount();//��ȡͼ����

	OGRLayer *rLayer = rDataset -> GetLayer(0);//����һ��ͼ�㣿
	if(NULL == rLayer)
		return FALSE;

	SpatialRef = rLayer -> GetSpatialRef();//��ȡͼ��ռ�ο�

	rGeoType = rLayer -> GetGeomType();//��ȡͼ��ͼԪ����

	i64ObjectNum = rLayer -> GetFeatureCount();//��ȡͼ��������

	//���Ա�
	OGRFeatureDefn *rAttribute = rLayer -> GetLayerDefn();
	AttTable.iFieldNum = rAttribute -> GetFieldCount();//�ֶ���
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

//�½��ļ����������Ա�
bool CVectorWYF::CreateVectorFile(CString strFilePath)
{
	
	GDALDriver *wDriver = NULL;
	wDriver = GetGDALDriverManager() -> GetDriverByName("ESRI Shapefile");
	wDataset = wDriver -> Create(strFilePath, 0, 0, 0, GDT_Unknown, NULL);
	
	CPLSetConfigOption( "SHAPE_ENCODING", "" );//֧�������ֶ�

	OGRLayer *wLayer = wDataset -> CreateLayer("NewLayer", SpatialRef, rGeoType, NULL); 
	if(NULL == wLayer)
		return FALSE;
	
	//�������Ա�
	/*CPLSetConfigOption( "SHAPE_ENCODING", "" );*/
	for(int i = 0; i < AttTable.iFieldNum; i++)
	{
		OGRFieldDefn tField(AttTable.strFieldName[i], AttTable.enFieldType[i]);
		wLayer -> CreateField(&tField);
	}
	
	//����µ��ֶ�
	vector<CString> strFieldNameNew;
	vector<OGRFieldType> enFieldTypeNew;
	
	strFieldNameNew.push_back("��Ԫ��");
	strFieldNameNew.push_back("��С�Ҷ�ֵ");
	strFieldNameNew.push_back("���Ҷ�ֵ");
	strFieldNameNew.push_back("��ֵ");
	strFieldNameNew.push_back("������");
	strFieldNameNew.push_back("դ�����");
	strFieldNameNew.push_back("ʸ�����");

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

//��ȡ������Ϣ
bool CVectorWYF::ReadObject(INT64 i64ObjID)
{
	OGRLayer *rLayer = rDataset -> GetLayer(0);
	OGRFeature *rFeature = rLayer -> GetFeature(i64ObjID);
	if(NULL == rFeature)
		return FALSE;
	
	//��ȡ������Ϣ
	PolygonAttData.clear();
	for(int i = 0; i < AttTable.iFieldNum; i++)
	{
		PolygonAttData.push_back(rFeature -> GetFieldAsString(i));
	}
	
	//��ȡ�ռ���Ϣ
	OGRGeometry *rGeometry = rFeature -> GetGeometryRef();
	OGRwkbGeometryType type = wkbFlatten(rGeometry -> getGeometryType());
	OGRPolygon *rOGRPolygon = (OGRPolygon *)rGeometry;

	MY_POINT TmpPoint;
	MY_RING TmpExRing;
	
	//�⻷
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

	//�ڻ�
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
	
	//д��������Ϣ
	for(int i = 0; i < AttTable.iFieldNum; i++)
	{	
		//����Ϣ����
		wFeature -> SetField(AttTable.strFieldName[i], PolygonAttData[i]);
		//���ֶθ�ֵ
		wFeature -> SetField("��Ԫ��", m_iPixelNum);
		wFeature -> SetField("��С�Ҷ�ֵ", m_bMinPixelValue);
		wFeature -> SetField("���Ҷ�ֵ", m_bMaxPixelValue);
		wFeature -> SetField("��ֵ", m_dPixelMeanValue);
		wFeature -> SetField("������", m_dPixelMeanSquareValue);
		wFeature -> SetField("դ�����", m_dRasterArea);
		wFeature -> SetField("ʸ�����", m_dVectorArea);
	}
	

	//д��ɵĿռ���Ϣ
	OGRPolygon TmpPolygon;
	TmpPolygon.empty();
	
	//���⻷���ڻ�
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