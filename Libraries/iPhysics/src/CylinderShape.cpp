#include "CylinderShape.h"

namespace physics
{
	CylinderShape::CylinderShape(float radius, float height)
		: iShape(ShapeType::Cylinder)
		, mRadius(radius), mHeight(height)
	{ }

	CylinderShape::~CylinderShape()
	{ }

	CylinderShape* CylinderShape::Cast(iShape* shape)
	{
		return dynamic_cast<CylinderShape*>(shape);
	}
}