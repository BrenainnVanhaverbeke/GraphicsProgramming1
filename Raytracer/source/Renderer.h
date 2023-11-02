#pragma once

#include <cstdint>
#include "DataTypes.h"

struct SDL_Window;
struct SDL_Surface;

namespace dae
{
	class Camera;
	class Scene;

	class Renderer final
	{
	public:
		Renderer(SDL_Window* pWindow);
		~Renderer() = default;

		Renderer(const Renderer&) = delete;
		Renderer(Renderer&&) noexcept = delete;
		Renderer& operator=(const Renderer&) = delete;
		Renderer& operator=(Renderer&&) noexcept = delete;

		void Render(Scene* pScene) const;
		bool SaveBufferToImage() const;
		void ToggleShadows();
		void CycleLightingMode();

	private:
		SDL_Window* m_pWindow{};

		SDL_Surface* m_pBuffer{};
		uint32_t* m_pBufferPixels{};

		int m_Width{};
		int m_Height{};

		enum class LightingMode
		{
			ObservedArea,	// Lambert Cosine Law
			Radiance,		// Incident radiance
			BRDF,			// Light scattering
			Combined		// ObservedArea * Radiance * BRDF
		};

		LightingMode m_CurrentLightingMode;
		bool m_ShadowsEnabled;

		inline Ray GetViewRay(int px, int py, Camera& camera, const Matrix& cameraToWorld) const;
		inline void CalculateObservedArea() const;
		inline void CalculateShadows(const HitRecord& closestHit, ColorRGB& finalColor, auto& lights, Scene* pScene) const;
	};
}
