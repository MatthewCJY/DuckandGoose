#include "SceneManager.h"

namespace EM
{

	std::unique_ptr<SceneManager> SceneManager::m_instance;

	std::unique_ptr<SceneManager>& SceneManager::GetInstance()
	{
		if (m_instance == nullptr)
		{
			m_instance = std::make_unique<SceneManager>();
		}
		return m_instance;
	}

	void SceneManager::Init()
	{
		p_ecs.RegisterComponent<Transform>();
		p_ecs.RegisterComponent<RigidBody>();
		p_ecs.RegisterComponent<Collider>();
		p_ecs.RegisterComponent<NameTag>();
		p_ecs.RegisterComponent<Sprite>();
	}

	bool SceneManager::Deserialize(const rapidjson::Value& obj)
	{
		p_ecs.ResetEntities();
		p_ecs.SetTotalEntitiesForWorldBuild(obj["Number of Entities"]["Entities"].GetUint());
		for (ComponentType i = 0; i < p_ecs.GetTotalRegisteredComponents(); ++i)
		{
			p_ecs.ClearArrayForWorldBuild(i);
			for (Entity j = 1; j <= p_ecs.GetTotalEntities(); ++j)
			{
				Signature signature(obj["EntitySignatures"][(j-1)].GetString());
				if (signature.test(i))
				{
					//ADD COMPONENTS HERE FOR DESERIALIZE
					if (p_ecs.GetComponentTypeName(i) == "Transform")
					{
						Transform transform;
						if (transform.Deserialize(obj["Components"][p_ecs.GetComponentTypeName(i).c_str()][j - 1].GetObj()))
						{
							p_ecs.AddComponent<Transform>(j, transform);
						}
					}
					if (p_ecs.GetComponentTypeName(i) == "RigidBody")
					{
						RigidBody rigidbody;
						if (rigidbody.Deserialize(obj["Components"][p_ecs.GetComponentTypeName(i).c_str()][j - 1].GetObj()))
						{
							p_ecs.AddComponent<RigidBody>(j, rigidbody);
						}
					}
					if (p_ecs.GetComponentTypeName(i) == "Collider")
					{
						Collider collider;
						if (collider.Deserialize(obj["Components"][p_ecs.GetComponentTypeName(i).c_str()][j - 1].GetObj()))
						{
							p_ecs.AddComponent<Collider>(j, collider);
						}
					}
					if (p_ecs.GetComponentTypeName(i) == "NameTag")
					{
						NameTag nametag;
						if (nametag.Deserialize(obj["Components"][p_ecs.GetComponentTypeName(i).c_str()][j - 1].GetObj()))
						{
							p_ecs.AddComponent<NameTag>(j, nametag);
						}
					}
					if (p_ecs.GetComponentTypeName(i) == "Sprite")
					{
						Sprite sprite;
						if (sprite.Deserialize(obj["Components"][p_ecs.GetComponentTypeName(i).c_str()][j - 1].GetObj()))
						{
							p_ecs.AddComponent<Sprite>(j, sprite);
						}
					}
					
				}
			}
			for (Entity j = 1; j < MAX_ENTITIES; ++j)
			{
				p_ecs.GetEntityToIndexMapECS(i)[j] = obj["EntityToIndexMap"][p_ecs.GetComponentTypeName(i).c_str()][j].GetUint();
				p_ecs.GetIndexToEntityMapECS(i)[j] = obj["IndexToEntityMap"][p_ecs.GetComponentTypeName(i).c_str()][j].GetUint();
			}
		}
		return true;
	}

	bool SceneManager::Serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer) const
	{
		writer->StartObject();

		writer->Key("Number of Entities");
		writer->StartObject();
		writer->Key("Entities");
		writer->Uint(p_ecs.GetTotalEntities());
		writer->EndObject();

		writer->Key("EntityToIndexMap");
		writer->StartObject();
		for (ComponentType i = 0; i < p_ecs.GetTotalRegisteredComponents(); ++i)
		{
			writer->Key(p_ecs.GetComponentTypeName(i).c_str());
			writer->StartArray();
			for (auto j = p_ecs.GetEntityToIndexMapECS(i).begin(); j != p_ecs.GetEntityToIndexMapECS(i).end(); ++j)
			{
				writer->Uint64(*j);
			}
			writer->EndArray();
		}
		writer->EndObject();

		writer->Key("IndexToEntityMap");
		writer->StartObject();
		for (ComponentType i = 0; i < p_ecs.GetTotalRegisteredComponents(); ++i)
		{
			writer->Key(p_ecs.GetComponentTypeName(i).c_str());
			writer->StartArray();
			for (auto j = p_ecs.GetIndexToEntityMapECS(i).begin(); j != p_ecs.GetIndexToEntityMapECS(i).end(); ++j)
			{
				writer->Uint(*j);
			}
			writer->EndArray();
		}
		writer->EndObject();

		writer->Key("EntitySignatures");
		writer->StartArray();
		for (Entity i = 1; i <= p_ecs.GetTotalEntities(); ++i)
		{
			writer->String(p_ecs.GetEntitySignature(i).to_string().c_str());
		}
		writer->EndArray();

		writer->Key("Components");
		writer->StartObject();
		for (ComponentType i = 0; i < p_ecs.GetTotalRegisteredComponents(); ++i)
		{
			writer->Key(p_ecs.GetComponentTypeName(i).c_str());
			writer->StartArray();
			for (Entity j = 1; j <= p_ecs.GetTotalEntities(); ++j)
			{
				if (p_ecs.GetEntitySignature(j).test(i))
				{
					//ADD COMPONENTS HERE FOR SERIALIZE
					std::cout << "Component Serialized" << std::endl;
					if (p_ecs.GetComponentTypeName(i) == "Transform")
					{
						p_ecs.GetComponent<Transform>(j).Serialize(writer);
					}
					if (p_ecs.GetComponentTypeName(i) == "RigidBody")
					{
						p_ecs.GetComponent<RigidBody>(j).Serialize(writer);
					}
					if (p_ecs.GetComponentTypeName(i) == "Collider")
					{
						p_ecs.GetComponent<Collider>(j).Serialize(writer);
					}
					if (p_ecs.GetComponentTypeName(i) == "NameTag")
					{
						p_ecs.GetComponent<NameTag>(j).Serialize(writer);
					}
					std::cout << "Component Serialized" << std::endl;
					if (p_ecs.GetComponentTypeName(i) == "Sprite")
					{
						p_ecs.GetComponent<Sprite>(j).Serialize(writer);
					}
					std::cout << "Component Serialized" << std::endl;
				}
			}
			writer->EndArray();
		}
		writer->EndObject();

		writer->EndObject();
		return true;
	}
}