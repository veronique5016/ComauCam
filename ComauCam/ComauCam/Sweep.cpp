#include "stdafx.h"
#include "Sweep.h"


CSweep::CSweep():m_dDistance(5)
{

}
CSweep::~CSweep()
{
	unsigned int szL = m_vpSliceLayers.size();
	for (unsigned int i = 0; i < szL; i++)
	{
		delete m_vpSliceLayers[i];
		m_vpSliceLayers[i] = NULL;
	}
	m_vpSliceLayers.clear();

	unsigned int szW = m_vpSweepLayers.size();
	for (unsigned int i = 0; i < szW; i++)
	{
		delete m_vpSweepLayers[i];
		m_vpSweepLayers[i] = NULL;
	}
	m_vpSweepLayers.clear();
}


void CSweep::Sweep()
{
	double x_min, x_max, x, z;
	CPoint3D tmp_point;
	for (unsigned int i = 0; i < m_vpSweepLayers.size(); i++)
	{
		if (i == 0)
		{
			CSweepLayer* layer =m_vpSweepLayers[i];
			CBoundary* tmp_boundary = new CBoundary(*layer->m_vpBoundaries[0]);
			::Offset(tmp_boundary, -5, layer->m_vCoordinate);
			unsigned int szB = tmp_boundary->m_vpSegments.size();
			m_vpSweepLayers[i]->m_vpRoute.push_back(new CPoint3D(tmp_boundary->m_vpSegments[0]->m_ptStart));
			for (unsigned int j = 0; j < szB; j++)
			{
				m_vpSweepLayers[i]->m_vpRoute.push_back(new CPoint3D(tmp_boundary->m_vpSegments[j]->m_ptEnd));
			}
		}
		//将最外层轮廓存入路径中
		unsigned int szB = m_vpSweepLayers[i]->m_vpBoundaries[0]->m_vpSegments.size();
		m_vpSweepLayers[i]->m_vpRoute.push_back(new CPoint3D(m_vpSweepLayers[i]->m_vpBoundaries[0]->m_vpSegments[0]->m_ptStart));
		for (unsigned int j = 0; j < szB; j++)
		{
			m_vpSweepLayers[i]->m_vpRoute.push_back(new CPoint3D(m_vpSweepLayers[i]->m_vpBoundaries[0]->m_vpSegments[j]->m_ptEnd));
		}

		int sum = 0;
		CBoundary* tmp = m_vpSweepLayers[i]->m_vpBoundaries[0];
		while (sum < 1)
		{
			CBoundary* offsetBoundary = new CBoundary(*tmp);
			::Offset(offsetBoundary, 1, m_vpSweepLayers[i]->m_vCoordinate);
			unsigned int szOffset = offsetBoundary->m_vpSegments.size();
			m_vpSweepLayers[i]->m_vpRoute.push_back(new CPoint3D(offsetBoundary->m_vpSegments[0]->m_ptStart));
			for (unsigned int j = 0; j < szOffset; j++)
			{
				m_vpSweepLayers[i]->m_vpRoute.push_back(new CPoint3D(offsetBoundary->m_vpSegments[j]->m_ptEnd));
			}
			tmp = offsetBoundary;
			sum++;
		}
		//变法向的层切面因为太小所以不切，只走轮廓
		/*if (m_vpSweepLayers[i]->m_vCoordinate[2].dz == 1.0)
		{
			//设置偏置大小
			Offset(m_vpSweepLayers[i], 0.9);
			//寻找轮廓 x 方向的极值
			unsigned int sz = m_vpSweepLayers[i]->m_vpOffsetBoundaries[0]->m_vpSegments.size();
			x_min = x_max = m_vpSweepLayers[i]->m_vpOffsetBoundaries[0]->m_vpSegments[0]->m_ptStart.x;
			for (unsigned int j = 0; j < sz; j++)
			{
				if (x_max <= m_vpSweepLayers[i]->m_vpOffsetBoundaries[0]->m_vpSegments[j]->m_ptEnd.x)
					x_max = m_vpSweepLayers[i]->m_vpOffsetBoundaries[0]->m_vpSegments[j]->m_ptEnd.x;
				if (x_min >= m_vpSweepLayers[i]->m_vpOffsetBoundaries[0]->m_vpSegments[j]->m_ptEnd.x)
				{
					x_min = m_vpSweepLayers[i]->m_vpOffsetBoundaries[0]->m_vpSegments[j]->m_ptEnd.x;
					z = m_vpSweepLayers[i]->m_vpOffsetBoundaries[0]->m_vpSegments[j]->m_ptEnd.z;
				}
			}

			//给第一条扫描线赋初值
			x = x_min + 0.1;
			while (true)
			{
				CSweepLine* tmp_sweep = new CSweepLine();
				tmp_sweep->m_ptLinePoint = CPoint3D(x, 0, z);
				tmp_sweep->m_vLineVec = CVector3D(0, 1, 0);
				tmp_sweep->m_vLineNormal = m_vpSweepLayers[i]->m_vCoordinate[2];
				m_vpSweepLayers[i]->m_vpSweepLines.push_back(tmp_sweep);
				YaxisSweep(m_vpSweepLayers[i]);

				if (x >= (x_max - m_dDistance))
				{  
					break;
				}
				x = x + m_vpSweepLayers[i]->m_vCoordinate[0].dx*m_dDistance;
				//z = z + m_vpSweepLayers[i]->m_vCoordinate[0].dz*m_dDistance;
			}
		}
		else
		{
			unsigned szT = m_vpSweepLayers[i]->m_vpRoute.size();
			for (unsigned int j = 0; j < szT; j++)
			{
				m_vpSweepLayers[i]->m_vpTurnRoute.push_back(new CSweepPoint(*m_vpSweepLayers[i]->m_vpRoute[(j+1)%szT],0,0));
			}
			CalACAngle(m_vpSweepLayers[i]);
			CalFiveAxisValue(m_vpSweepLayers[i]);
		}*/
	}
}

void CSweep::YaxisSweep(CSweepLayer* layer)
{
	unsigned int szSeg = layer->m_vpOffsetBoundaries[0]->m_vpSegments.size();
	unsigned int szLine = layer->m_vpSweepLines.size();
	CPoint3D sweepline_point;
	CPoint3D pout;
	CVector3D vec1, vec2;
	sweepline_point = layer->m_vpSweepLines[szLine-1]->m_ptLinePoint;

	// 遍历轮廓所有线段，求得和扫描线的交点
	for (unsigned int i = 0; i < szSeg; i++)
	{
		vec1 = CVector3D(sweepline_point, layer->m_vpOffsetBoundaries[0]->m_vpSegments[i]->m_ptStart);
		vec2 = CVector3D(sweepline_point, layer->m_vpOffsetBoundaries[0]->m_vpSegments[i]->m_ptEnd);
		if ((layer->m_vpSweepLines[layer->m_vpSweepLines.size() - 1]->m_vLineVec*vec1).dz 
			*(layer->m_vpSweepLines[layer->m_vpSweepLines.size() - 1]->m_vLineVec*vec2).dz <0)
		{
			GetCrossPoint(pout, *layer->m_vpSweepLines[layer->m_vpSweepLines.size() - 1], *layer->m_vpOffsetBoundaries[0]->m_vpSegments[i]);
			layer->m_vpSweepLines[szLine-1]->m_ptCrossPoint.push_back(CSweepPoint(pout, 0, 0));
		}
	}

/*	///连成 zigzag 路径的算法需要很大的改动
	unsigned int szP = crosspoint.size();

	if (layer->m_vpRoute.size() == 0)
	{
		layer->m_vpRoute.push_back(new CPoint3D(crosspoint[0]));
		layer->m_vpRoute.push_back(new CPoint3D(crosspoint[1]));
	}
	else
	{
		double dist1 = GetDistance(*layer->m_vpRoute[layer->m_vpRoute.size() - 1], crosspoint[0]);
		double dist2 = GetDistance(*layer->m_vpRoute[layer->m_vpRoute.size() - 1], crosspoint[1]);
		if (dist1 < dist2)
		{
			layer->m_vpRoute.push_back(new CPoint3D(crosspoint[0]));
			layer->m_vpRoute.push_back(new CPoint3D(crosspoint[1]));
		}
		else
		{
			layer->m_vpRoute.push_back(new CPoint3D(crosspoint[1]));
			layer->m_vpRoute.push_back(new CPoint3D(crosspoint[0]));
		}
	}	*/
	/////////
}

void CSweep::LoadSliceModel(CSlice* slicemodel)
{	
	unsigned int szLay = slicemodel->m_vpLayers.size();
	for (unsigned int i = 0; i < szLay; i++)
	{
		m_vpSliceLayers.push_back(slicemodel->m_vpLayers[i]);		//为CSweep对象拷贝一份层切面数据
		m_vpSweepLayers.push_back(new CSweepLayer(*slicemodel->m_vpLayers[i]));
	}
}

void CSweep::CalACAngle(CSweepLayer* layer)
{
	int szpt = layer->m_vpTurnRoute.size();
	CVector3D v_x(1.0, 0.0, 0.0);  //x轴向量
	CVector3D v_y(0.0, 1.0, 0.0);  //y轴向量，求法向量在X0Y用于判断C角大小；
	CVector3D v_z(0.0, 0.0, 1.0);  //z轴向量，求法向量与z向量的夹角为A角；
	for (int i = 0; i < szpt; i++)
	{
		//FPoint* ptFive = new FPoint();
		CVector3D v;
		v.ProjectionX0Y(layer->m_vCoordinate[2]);   //投影到X0Z平面的向量
													//v = lawPrintPoints[i]->Ltri->n;
		double C = v.GetC_Angle(v, v_y);
		double A = v.GetAngle(layer->m_vCoordinate[2], v_z);
		layer->m_vpTurnRoute[i]->A = A;
		layer->m_vpTurnRoute[i]->C = C;
	}
}

void CSweep::CalFiveAxisValue(CSweepLayer* layer)
{
	int szpt = layer->m_vpRoute.size();
	CVector3D v_x(1.0, 0.0, 0.0);  //x轴向量
	CVector3D v_y(0.0, 1.0, 0.0);  //y轴向量
	CVector3D v_z(0.0, 0.0, 1.0);  //z轴向量

	//CVector3D v_move(0, 0, m_z_move);   //57.7,二维转台中心和二维转台平台中心的距离补偿，因为打印件在二维转台平台中心，旋转中心是二维转台中心
	CVector3D v_move(0, 0, 100);
	CVector3D v_countermove(0, 0, -100);
	//CVector3D v_offset(m_x_offset, m_y_offset, m_z_offset);  //(1.39,-1.14,105.0)二维转台中心在delta世界坐标系中的坐标（世界坐标系的原点是喷绘头共点处），进行坐标补偿，生成在delta平台下xyz的坐标值
	//CVector3D v_offset(-2.12, 109.40, 138.4);
	//CVector3D v_offset1(m_x_offset, m_y_offset, m_z_offset + m_safeDistance); //保护调试距离
	//CVector3D v_offset1(-2.12, 109.40, 180);
	for (int i = 0; i < szpt; i++)
	{
		CMatrix3D matrix_rotate_z = CMatrix3D::CreateRotateMatrix(2 * PI - layer->m_vpTurnRoute[i]->C, v_z);   //创建旋转平移矩阵
		CMatrix3D matrix_rotate_x = CMatrix3D::CreateRotateMatrix(layer->m_vpTurnRoute[i]->A, v_x);
		CMatrix3D matrix_move = CMatrix3D::CreateTransferMatrix(v_move);
		CMatrix3D matrix_countermove = CMatrix3D::CreateTransferMatrix(v_countermove);
		//CMatrix3D matrix_offset = CMatrix3D::CreateTransferMatrix(v_offset);
		//CMatrix3D matrix_offset1 = CMatrix3D::CreateTransferMatrix(v_offset1);

		//*(fiveAxisPoints[i]->n) *= matrix_rotate_z;   //法向量变换
		//*(fiveAxisPoints[i]->n) *= matrix_rotate_x;
		*layer->m_vpTurnRoute[i] *= matrix_move;
		*layer->m_vpTurnRoute[i] *= matrix_rotate_z;
		*layer->m_vpTurnRoute[i] *= matrix_rotate_x;
		*layer->m_vpTurnRoute[i] *= matrix_countermove;
		//*layer->m_turnRoute[i] *= matrix_offset1;   //安全距离进行测试
		//*fiveAxisPoints[i] *= matrix_offset;  //正常距离打印
	}
}


