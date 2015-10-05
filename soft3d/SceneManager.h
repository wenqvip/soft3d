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
		~SceneManager();
		void Init();
		void Update();

	private:
		SceneManager();
		static std::shared_ptr<SceneManager> s_instance;
	};

}