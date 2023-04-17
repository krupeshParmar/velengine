#pragma once

#include "iShape.h"
#include "PhyMath.h"

namespace physics
{
	class CylinderShape : public iShape
	{
	public:
		CylinderShape(float radius, float height);
		virtual ~CylinderShape();

		const Vector3& GetHalfExtents() const
		{
			return Vector3(mRadius);
		};

		float GetRadius() { return mRadius; }
		float GetHeight() { return mHeight; }

		static CylinderShape* Cast(iShape* shape);

	protected:
		CylinderShape(ShapeType shapeType)
			: iShape(shapeType) {}

	private:
		float mRadius, mHeight;
		CylinderShape(const CylinderShape&) : iShape(ShapeType::Cylinder) {}
		CylinderShape& operator=(const CylinderShape&) {
			return *this;
		}
	};
}
