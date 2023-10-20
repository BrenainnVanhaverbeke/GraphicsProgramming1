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

	private:
		SDL_Window* m_pWindow{};

		SDL_Surface* m_pBuffer{};
		uint32_t* m_pBufferPixels{};

		inline Ray GetViewRay(int px, int py, Camera& camera, const Matrix& cameraToWorld) const;

		int m_Width{};
		int m_Height{};
	};
}
