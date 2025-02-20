#include "Transform.h"

namespace EM
{
	//Transform
	Transform::Transform() : mPosition{ vec2D(1.0f,1.0f) }, mScale{ vec2D(1.0f,1.0f) }, mRot{ 0.0f } {}

	bool Transform::Deserialize(const rapidjson::Value& obj)
	{
		mPosition = vec2D(obj["posX"].GetFloat(), obj["posY"].GetFloat());
		mScale = vec2D(obj["scaleX"].GetFloat(), obj["scaleY"].GetFloat());
		mRot = obj["Rot"].GetFloat();
		return true;
	}
	bool Transform::Serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer) const
	{
		writer->StartObject();
		writer->Key("posX");
		writer->Double(mPosition.x);
		writer->Key("posY");
		writer->Double(mPosition.y);
		writer->Key("scaleX");
		writer->Double(mScale.x);
		writer->Key("scaleY");
		writer->Double(mScale.y);
		writer->Key("Rot");
		writer->Double(mRot);
		writer->EndObject();
		return true;
	}
	//End Transform
}