/*!*************************************************************************
****
\file RigidBody.cpp
\author Elton Teo Zhe Wei
\par DP email: e.teo@digipen.edu
\par Course: CSD2400
\par Section: a
\par Assignment GAM200
\date 2/11/2022
\brief  RigidBody Component to describe material of object and the forces
acting on said object.

****************************************************************************
***/

#include "RigidBody.h"

namespace EM
{
	//RigidBody
	RigidBody::RigidBody() : mVelocity{vec2D()}, mDirection{vec2D()}, mFriction{0.0f}, mRestitution{0.0f} {}

	bool RigidBody::Deserialize(const rapidjson::Value& obj)
	{
		mVelocity = vec2D(obj["velX"].GetFloat(), obj["velY"].GetFloat());
		mDirection = vec2D(obj["dirX"].GetFloat(), obj["dirY"].GetFloat());
		mFriction = obj["Friction"].GetFloat();
		mRestitution = obj["Restitution"].GetFloat();
		return true;
	}
	bool RigidBody::Serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer) const
	{
		writer->StartObject();
		writer->Key("velX");
		writer->Double(mVelocity.x);
		writer->Key("velY");
		writer->Double(mVelocity.y);
		writer->Key("dirX");
		writer->Double(mDirection.x);
		writer->Key("dirY");
		writer->Double(mDirection.y);
		writer->Key("Friction");
		writer->Double(mFriction);
		writer->Key("Restitution");
		writer->Double(mRestitution);
		writer->EndObject();
		return true;
	}
	//End RigidBody
}