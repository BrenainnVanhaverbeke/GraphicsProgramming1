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

Renderer::Renderer(SDL_Window* pWindow) :
	m_pWindow(pWindow),
	m_pBuffer(SDL_GetWindowSurface(pWindow))
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
	const float width{ static_cast<float>(m_Width) };
	const float height{ static_cast<float>(m_Height) };
	const float aspectRatio{ width / height };
	const float fov{ tanf(camera.fovAngle / 2.0f) };
	const Matrix cameraToWorld{ camera.CalculateCameraToWorld() };
	for (int px{}; px < m_Width; ++px)
	{
		for (int py{}; py < m_Height; ++py)
		{
			float viewX{ ((2.0f * (static_cast<float>(px) + 0.5f)) / width - 1.0f) * (aspectRatio * fov) };
			float viewY{ (1.f - ((2.f * (static_cast<float>(py) + 0.5f)) / height)) * fov };
			Vector3 rayDirection{ viewX, viewY, 1.0f };
			rayDirection.Normalize();

			Ray viewRay{ camera.origin, cameraToWorld.TransformVector(rayDirection).Normalized() };

			ColorRGB finalColor{};
			HitRecord closestHit{};

			pScene->GetClosestHit(viewRay, closestHit);

			if (closestHit.didHit)
				finalColor = materials[closestHit.materialIndex]->Shade();

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