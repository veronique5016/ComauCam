#pragma once

#include "afx.h"
//#include "Entity.h"
#include "GeomBase.h"
#include "OpenGL.h"

class AFX_CLASS_EXPORT CPointSet : public CEntity
{
public:
	CPointSet(void);
	~CPointSet(void);

	double FindExtreme(int nWhichDir) const;
	Point3DAry m_pts;
	enum{MIN_X, MAX_X, MIN_Y, MAX_Y, MIN_Z, MAX_Z};

	virtual void Draw(COpenGLDC* pDC, bool ShowTri) = 0;
};