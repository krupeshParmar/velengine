#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <vector>
#include <string>

struct aiNodeAnim;

namespace vel
{
	struct KeyPosition
	{
		glm::vec3 position;
		float timeStamp;
	};

	struct KeyRotation
	{
		glm::quat orientation;
		float timeStamp;
	};

	struct KeyScale
	{
		glm::vec3 scale;
		float timeStamp;
	};

	class BoneAnim
	{
	private:
		std::vector<KeyPosition> m_Positions;
		std::vector<KeyRotation> m_Rotations;
		std::vector<KeyScale> m_Scales;

		int m_NumPositions;
		int m_NumRotations;
		int m_NumScalings;

		glm::mat4 m_LocalTransform;
		std::string m_Name;
		int m_ID;

	public:
		BoneAnim(const std::string& name, int ID, const aiNodeAnim* channel);
		void Update(float animationTime);

		/* Gets the current index on mKeyPositions to interpolate to based on
		the current animation time*/
		int GetPositionIndex(float animationTime) const;

		/* Gets the current index on mKeyRotations to interpolate to based on the
		current animation time*/
		int GetRotationIndex(float animationTime) const;

		/* Gets the current index on mKeyScalings to interpolate to based on the
		current animation time */
		int GetScaleIndex(float animationTime) const;


		inline const glm::mat4& GetLocalTransform() const { return m_LocalTransform; }
		inline const std::string& GetBoneName() const { return m_Name; }
		inline const int& GetBoneID() { return m_ID; }

	private:
		/* Gets normalized value for Lerp & Slerp*/
		float GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime) const;

		/*figures out which position keys to interpolate b/w and performs the interpolation
		and returns the translation matrix*/
		glm::mat4 InterpolatePosition(float animationTime);

		/*figures out which rotations keys to interpolate b/w and performs the interpolation
		and returns the rotation matrix*/
		glm::mat4 InterpolateRotation(float animationTime);

		/*figures out which scaling keys to interpolate b/w and performs the interpolation
		and returns the scale matrix*/
		glm::mat4 InterpolateScaling(float animationTime);

	};
}