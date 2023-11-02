//External includes
#include "SDL.h"
#include "SDL_surface.h"

//Project includes
#include "Renderer.h"
#include "Math.h"
#include "Matrix.h"
#include "Material.h"
#include "Scene.h"
#include "Utils.h"

using namespace dae;

Renderer::Renderer(SDL_Window* pWindow)
	: m_pWindow(pWindow)
	, m_pBuffer(SDL_GetWindowSurface(pWindow))
	, m_CurrentLightingMode{ LightingMode::Combined }
	, m_ShadowsEnabled{ true }
{
	//Initialize
	SDL_GetWindowSize(pWindow, &m_Width, &m_Height);
	m_pBufferPixels = static_cast<uint32_t*>(m_pBuffer->pixels);
}

void Renderer::Render(Scene* pScene) const
{
	Camera& camera = pScene->GetCamera();
	auto& materials = pScene->GetMaterials();
	auto& lights = pScene->GetLights();
	const Matrix cameraToWorld{ camera.CalculateCameraToWorld() };
	for (int px{}; px < m_Width; ++px)
	{
		for (int py{}; py < m_Height; ++py)
		{
			Ray viewRay{ GetViewRay(px, py, camera, cameraToWorld) };

			ColorRGB finalColor{};
			HitRecord closestHit{};

			pScene->GetClosestHit(viewRay, closestHit);

			if (closestHit.didHit)
			{
				finalColor = materials[closestHit.materialIndex]->Shade();
				if (m_CurrentLightingMode == LightingMode::ObservedArea || m_CurrentLightingMode == LightingMode::Combined)
					CalculateObservedArea();
				if (m_ShadowsEnabled)
					CalculateShadows(closestHit, finalColor, lights, pScene);
			}
			//Update Color in Buffer
			finalColor.MaxToOne();

			m_pBufferPixels[px + (py * m_Width)] = SDL_MapRGB(m_pBuffer->format,
															  static_cast<uint8_t>(finalColor.r * 255),
															  static_cast<uint8_t>(finalColor.g * 255),
															  static_cast<uint8_t>(finalColor.b * 255));
		}
	}

	//@END
	//Update SDL Surface
	SDL_UpdateWindowSurface(m_pWindow);
}

bool Renderer::SaveBufferToImage() const
{
	return SDL_SaveBMP(m_pBuffer, "RayTracing_Buffer.bmp");
}

void dae::Renderer::ToggleShadows()
{
	m_ShadowsEnabled = !m_ShadowsEnabled;
}

void dae::Renderer::CycleLightingMode()
{
	int currentLightingMode{ static_cast<int>(m_CurrentLightingMode) };
	++currentLightingMode;
	currentLightingMode %= static_cast<int>(LightingMode::Combined) + 1;
	m_CurrentLightingMode = static_cast<LightingMode>(currentLightingMode);
}

inline Ray dae::Renderer::GetViewRay(int px, int py, Camera& camera, const Matrix& cameraToWorld) const
{
	const float width{ static_cast<float>(m_Width) };
	const float height{ static_cast<float>(m_Height) };
	const float aspectRatio{ width / height };
	const float fov{ tanf(camera.fovAngle / 2.0f) };

	float viewX{ ((2.0f * (static_cast<float>(px) + 0.5f)) / width - 1.0f) * (aspectRatio * fov) };
	float viewY{ (1.f - ((2.f * (static_cast<float>(py) + 0.5f)) / height)) * fov };
	Vector3 rayDirection{ viewX, viewY, 1.0f };
	rayDirection.Normalize();

	return { camera.origin, cameraToWorld.TransformVector(rayDirection).Normalized() };
}

inline void dae::Renderer::CalculateShadows(const HitRecord& closestHit, ColorRGB& finalColor, auto& lights, Scene* pScene) const
{
	Vector3 hitOrigin{ closestHit.origin + (closestHit.normal * 0.001f) };
	for (size_t i = 0; i < lights.size(); i++)
	{
		Vector3 lightDirection{ LightUtils::GetDirectionToLight(lights[i], hitOrigin) };
		const float lightMagnitude{ lightDirection.Magnitude() };
		Ray lightRay{ hitOrigin, lightDirection.Normalized() };
		lightRay.max = lightMagnitude;
		if (pScene->DoesHit(lightRay))
			finalColor *= 0.5f;
	}
}
