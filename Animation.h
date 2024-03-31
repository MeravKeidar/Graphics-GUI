#pragma once
#include "Scene.h"
#include <functional>
#include <thread>
#include <chrono>


	void pulse(Model* model, bool repeat)
	{
		for (size_t i = 0; i < 10; i++)
		{
			model->Scale(1.05, 1.05, 1.05);
			std::this_thread::sleep_for(std::chrono::milliseconds(50));
		}
		for (size_t i = 0; i < 10; i++)
		{
			model->Scale(1/1.05, 1/1.05, 1/1.05);
			std::this_thread::sleep_for(std::chrono::milliseconds(50));
		}
		if (repeat)
			pulse();
	}

	void vibrate(Model* model, bool repeat)
	{
		float scale = model->size_scale / 20;
		for (size_t i = 0; i < 10; i++)
		{
			int random_num = rand() % 27;
			int x_dir, y_dir, z_dir;
			x_dir = random_num % 3 - 1;
			y_dir = (random_num/3) % 3 - 1;
			z_dir = (random_num/9) % 3 - 1;
			model->Translate(x_dir, y_dir, z_dir);
			std::this_thread::sleep_for(std::chrono::milliseconds(50));
			model->Translate(-x_dir, -y_dir, -z_dir);
			std::this_thread::sleep_for(std::chrono::milliseconds(50));

		}
		if (repeat)
			vibrate();
	}


