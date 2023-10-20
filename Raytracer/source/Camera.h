#pragma once
#include <cassert>
#include <SDL_keyboard.h>
#include <SDL_mouse.h>
#include <iostream>

#include "Math.h"
#include "Timer.h"

namespace dae
{
	struct Camera
	{
		Camera() = default;

		Camera(const Vector3& _origin, float _fovAngle) :
			origin{ _origin },
			fovAngle{ _fovAngle }
		{
		}


		Vector3 origin{};
		float fovAngle{ 90.f };

		Vector3 forward{ 0.266f, -0.453f, 0.860f };
		//Vector3 forward{ Vector3::UnitZ };
		Vector3 up{ Vector3::UnitY };
		Vector3 right{ Vector3::UnitX };

		float totalPitch{ 0.f };
		float totalYaw{ 0.f };

		Matrix cameraToWorld{  };


		Matrix CalculateCameraToWorld() const 
		{
			Vector3 rightVector{ Vector3::Cross(Vector3::UnitY, forward).Normalized() };
			Vector3 upVector{ Vector3::Cross(forward, rightVector) };
			Matrix cameraToWorld{ rightVector, upVector, forward, origin };
			return cameraToWorld;
		}

		void Update(Timer* pTimer)
		{
			const float deltaTime{ pTimer->GetElapsed() };
			constexpr float moveSpeed{ 5.0f };
			constexpr float rotationSpeed{ 0.05f };
			Vector3 right{ Vector3::Cross(Vector3::UnitY, forward).Normalized() };

			//Keyboard Input
			const uint8_t* pKeyboardState = SDL_GetKeyboardState(nullptr);
			origin += forward * moveSpeed * deltaTime * pKeyboardState[SDL_SCANCODE_W];
			origin -= right * moveSpeed * deltaTime * pKeyboardState[SDL_SCANCODE_A];
			origin -= forward * moveSpeed * deltaTime * pKeyboardState[SDL_SCANCODE_S];
			origin += right * moveSpeed * deltaTime * pKeyboardState[SDL_SCANCODE_D];

			//Mouse Input
			int mouseX{}, mouseY{};
			const uint32_t mouseState = SDL_GetRelativeMouseState(&mouseX, &mouseY);

			if ((mouseState & SDL_BUTTON(SDL_BUTTON_LEFT)) || mouseState & SDL_BUTTON(SDL_BUTTON_RIGHT))
			{
				totalPitch += -mouseY * rotationSpeed * deltaTime;
				totalYaw += mouseX * rotationSpeed * deltaTime;
			}
			Matrix finalRotation{ Matrix::CreateRotationX(totalPitch) * Matrix::CreateRotationY(totalYaw) };
			forward = finalRotation.TransformVector(Vector3::UnitZ);
			forward.Normalize();
		}
	};
}