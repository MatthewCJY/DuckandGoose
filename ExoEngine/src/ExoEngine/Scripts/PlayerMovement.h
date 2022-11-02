#pragma once
#include "IScript.h"
#include "ExoEngine/Input/Input.h"
#include "GLFW/glfw3.h"
#include "ExoEngine/Math/physics.h"


namespace EM
{
	class PlayerMovement : public IScript
	{
	public:
		PlayerMovement() = default;
		~PlayerMovement() = default;
		virtual PlayerMovement* Clone() const override;
		virtual void Start() override;
		virtual void Update(float Frametime) override;
		virtual void End() override;
		virtual std::string GetScriptName() override;
	private:
		entityPhysics phys;
	};
}