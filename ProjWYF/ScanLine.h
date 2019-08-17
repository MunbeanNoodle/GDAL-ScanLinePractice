#pragma once
#include"RasterWYF.h"
#include"VectorWYF.h"

#include<vector>
using namespace std;

//ɨ������ö��
typedef enum tag_Scan_Method
{
	Border_Not_Count_In, //���ڱ߽��ϵĵ㲻���������ڲ�
	Border_Count_In //���ڱ߽��ϵĵ����������ڲ�
}Scan_Method;

//�ߵĽṹ��
typedef struct tag_Edge
{
	double YMax;//�ñ�Y�����ֵ
	double X;//��¼��ǰX��λ�ã���ʼֵΪX = minX + deltaX * ((int)minY + 1 - minY)
	double deltaX;//б�ʵĵ�����deltaX = (X[i]-X[i+1])/(Y[i]-Y[i+1])
}Edge;

//ET��Ľṹ��
typedef struct tag_ET
{
	vector<Edge> ET_Edge;//��ǰET�д洢�ıߵ���Ϣ
}ET;

//YֵΪ������ˮƽ�ߵĽṹ��
typedef struct tag_Horizon_Line
{
	double X1;//�����Xֵ
	double X2;//�Ҳ���Xֵ
}Horizon_Line;

typedef struct tag_HL_ET
{
	//int Y;//��ǰET��Ӧ��Yֵ
    vector<Horizon_Line> HL;//��ǰET�洢��ƽ�б�
}HL_ET;

class CScanLineWYF
{
public:
	CScanLineWYF();
	~CScanLineWYF();

private:
	vector<ET> m_ET;//ET��
	vector<Edge> m_AEL;//��߱�
	vector<HL_ET> m_iHL;//YֵΪ���͵�ˮƽ�ߵ�ET��;

	double m_TransformCoeffcient[6]; //��¼����ת����ϵ��
	Scan_Method m_ScanMethod;//��¼ɨ�跽ʽ�ı�ʶ
	int m_iBandNo;

	//Ϊʸ������������������Ϣ��ÿ���һ������γ�ʼ��һ��
	vector<double>m_PolygonArea;//�⻷���ڻ�������������⻷������洢��0��λ��
	int m_iPixelValueSum;//������ڲ�����ֵ�ĺ�
	int m_iPixelValueSquareSum;//������ڲ�����ֵ��ƽ����
	


	bool StartScanLine(INT64 i64ObjectNo);//ɨ���i������
	bool ChuLiQuDuan(double X1, double X2, int Y);

public:
	int m_iPixelNum;//������ڲ���Ԫ����
	double m_bMinPixelValue;//��С�Ҷ�ֵ
	double m_bMaxPixelValue;//���Ҷ�ֵ
	double m_dPixelMeanValue;//������ڲ����ص�ƽ��ֵ
	double m_dPixelMeanSquareValue;//������ڲ����صľ�����
	double m_dRasterArea;//դ�����
	double m_dVectorArea;//ʸ�����
	bool InputFilePath(
		// ����������
		CString RasterFilePath, //դ���ļ�·��
		CString VectorFilePath, //ʸ���ļ�·��
		CString OutputPath, //���·��
		int iBand,//ѡȡ�Ĳ��α��
		Scan_Method  ScanMethod//�ж�����һ�ַ�ʽ����ɨ��
		);
};