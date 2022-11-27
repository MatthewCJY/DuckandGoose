/*!*************************************************************************
****
\file Nametag.h
\author Elton Teo Zhe Wei
\par DP email: e.teo@digipen.edu
\par Course: CSD2400
\par Section: a
\par Assignment GAM200
\date 2/11/2022
\brief  NameTag Component for tagging each entity with a string name

****************************************************************************
***/

#pragma once
#include "IComponent.h"
#include "empch.h"

namespace EM
{
	class NameTag : public IComponent
	{
	public:
		NameTag();
		~NameTag() = default;
		virtual bool Deserialize(const rapidjson::Value& obj);
		virtual bool Serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer) const;

		void SetNameTag(std::string name) { mName = name; }
		std::string& GetNameTag() { return mName; }

		Entity& GetComponentEntityID() { return entityID; }
		void SetComponentEntityID(Entity& entity) { entityID = entity; }

	private:
		std::string mName;
	};
}