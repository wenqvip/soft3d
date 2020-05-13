#pragma once
#include <memory>

namespace soft3d
{

	class SceneManager
	{
	public:
		static std::shared_ptr<SceneManager> Instance() {
			return s_instance;
		}
		SceneManager(const SceneManager&) = delete;
		virtual ~SceneManager();
		virtual void InitScene(uint16_t width, uint16_t height);
		virtual void Update();

	protected:
		SceneManager();
		static std::shared_ptr<SceneManager> s_instance;

		uint16_t m_width;
		uint16_t m_height;
	};

}