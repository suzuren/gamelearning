
#include <stdio.h>
#include <assert.h>
#include <iostream>
#include "vec2.hpp"
#include "quad_tree.hpp"

// http://www.brucezero.com:8888/%E4%B8%80%E7%A7%8D%E7%AE%80%E5%8D%95%E7%9A%84aoi%E7%9A%84%E5%AE%9E%E7%8E%B0/
// https://www.brucezero.com/%E4%B8%80%E7%A7%8D%E7%AE%80%E5%8D%95%E7%9A%84aoi%E7%9A%84%E5%AE%9E%E7%8E%B0/

void test_vec2()
{
	math::vec2 tempVector2_0;
	std::cout << "test_vec2  tempVector2_0 - " << "x:" << tempVector2_0.x << "\t\t" << " y:" << tempVector2_0.y << std::endl;

	math::vec2 tempVector2_1(3.1, 6.8);
	std::cout << "test_vec2  tempVector2_1 - " << "x:" << tempVector2_1.x << "\t" << " y:" << tempVector2_1.y << std::endl;

	math::vec2 tempVector2_2(tempVector2_1);
	std::cout << "test_vec2  tempVector2_2 - " << "x:" << tempVector2_2.x << "\t" << " y:" << tempVector2_2.y << std::endl;

	tempVector2_2.add(tempVector2_1);
	std::cout << "test_vec2  tempVector2_2 - " << "x:" << tempVector2_2.x << "\t" << " y:" << tempVector2_2.y << std::endl;

	tempVector2_0.subtract(tempVector2_1);
	std::cout << "test_vec2  tempVector2_0 - " << "x:" << tempVector2_0.x << "\t" << " y:" << tempVector2_0.y << std::endl;

	tempVector2_2.clamp(tempVector2_0, tempVector2_1);
	std::cout << "test_vec2  tempVector2_2 - " << "x:" << tempVector2_2.x << "\t" << " y:" << tempVector2_2.y << std::endl;

	tempVector2_2.negate();
	std::cout << "test_vec2  tempVector2_2 - " << "x:" << tempVector2_2.x << "\t" << " y:" << tempVector2_2.y << std::endl;

	tempVector2_2.normalize();
	std::cout << "test_vec2  tempVector2_2 - " << "x:" << tempVector2_2.x << "\t" << " y:" << tempVector2_2.y << std::endl;

	tempVector2_2.scale(2.1f);
	std::cout << "test_vec2  tempVector2_2 - " << "x:" << tempVector2_2.x << "\t" << " y:" << tempVector2_2.y << std::endl;

	tempVector2_2.scale(tempVector2_1);
	std::cout << "test_vec2  tempVector2_2 - " << "x:" << tempVector2_2.x << "\t" << " y:" << tempVector2_2.y << std::endl;

	tempVector2_2.rotate(tempVector2_1, 3.1f);
	std::cout << "test_vec2  tempVector2_2 - " << "x:" << tempVector2_2.x << "\t" << " y:" << tempVector2_2.y << std::endl;

	auto tempVector2_3 = tempVector2_2.project(tempVector2_1);
	std::cout << "test_vec2  tempVector2_3 - " << "x:" << tempVector2_3.x << "\t" << " y:" << tempVector2_3.y << std::endl;

	auto tempVector2_4 = tempVector2_2.mid_point(tempVector2_1);
	std::cout << "test_vec2  tempVector2_4 - " << "x:" << tempVector2_4.x << "\t" << " y:" << tempVector2_4.y << std::endl;

	auto tempVector2_5 = tempVector2_4.lerp(tempVector2_1, 0.f);
	std::cout << "test_vec2  tempVector2_5 - " << "x:" << tempVector2_5.x << "\t" << " y:" << tempVector2_5.y << std::endl;

	tempVector2_5 = tempVector2_4.lerp(tempVector2_1, 1.f);
	std::cout << "test_vec2  tempVector2_5 - " << "x:" << tempVector2_5.x << "\t" << " y:" << tempVector2_5.y << std::endl;

	tempVector2_5 = tempVector2_4.lerp(tempVector2_1, 1.4);
	std::cout << "test_vec2  tempVector2_5 - " << "x:" << tempVector2_5.x << "\t" << " y:" << tempVector2_5.y << std::endl;

	tempVector2_5.smooth(tempVector2_1, 3.1f, 0.f);
	std::cout << "test_vec2  tempVector2_2 - " << "x:" << tempVector2_2.x << "\t" << " y:" << tempVector2_2.y << std::endl;


	// -----------------------------------------------------------------------------------------------------------------

	std::cout << "test_vec2  tempVector2_2 - " << " iszero:" << tempVector2_2.iszero() << std::endl;

	std::cout << "test_vec2  tempVector2_2 - " << " isone:" << tempVector2_2.isone() << std::endl;

	auto dot_xy = tempVector2_2.dot(tempVector2_1);
	std::cout << "test_vec2  tempVector2_2 - " << "dot_xy:" << dot_xy << std::endl;
	auto cross_xy = tempVector2_2.cross(tempVector2_1);
	std::cout << "test_vec2  tempVector2_2 - " << "cross_xy:" << cross_xy << std::endl;

	auto distance_xy = tempVector2_2.distance(tempVector2_1);
	std::cout << "test_vec2  tempVector2_2 - " << "distance_xy:" << distance_xy << std::endl;

	auto distance_sqrt_xy = tempVector2_2.distance_sqrt(tempVector2_1);
	std::cout << "test_vec2  tempVector2_2 - " << "distance_sqrt_xy:" << distance_sqrt_xy << std::endl;

	auto length_xy = tempVector2_2.length();
	std::cout << "test_vec2  tempVector2_2 - " << "length_xy:" << length_xy << std::endl;

	auto length_sqrt_xy = tempVector2_2.length_sqrt();
	std::cout << "test_vec2  tempVector2_2 - " << "length_sqrt_xy:" << length_sqrt_xy << std::endl;

	auto angle_xy = tempVector2_2.angle();
	std::cout << "test_vec2  tempVector2_2 - " << "angle_xy:" << angle_xy << std::endl;

	auto angle_vec_xy = tempVector2_2.angle(tempVector2_1);
	std::cout << "test_vec2  tempVector2_2 - " << "angle_vec_xy:" << angle_vec_xy << std::endl;

	auto equals_xy = tempVector2_2.equals(tempVector2_2);
	std::cout << "test_vec2  tempVector2_2 - " << "equals_xy:" << equals_xy << std::endl;


}

struct player
{
	//uid-version, version用于优化差异比较计算，通过2次遍历即可得出进出视野结果
	using entity_view_t = std::unordered_map<math::objectid_t, int32_t>;
	int32_t version = 1;
	float view_w = 20.0f;//视野长宽
	float view_h = 20.0f;
	entity_view_t view;
};

void print_view_event(math::objectid_t uid, const player& p)
{
	for (auto& v : p.view)
	{
		if (v.second == p.version)
		{
			//进入视野
			std::cout << "player " << v.first << " enter " << uid << "view" << std::endl;
		}
		else if (v.second != (p.version - 1))//出视野
		{
			std::cout << "player " << v.first << " leave " << uid << "view" << std::endl;
			//注意这里要从 p.view 删除已经出视野的玩家
		}
	}
}

void example()
{
	float width = 512.0f;
	float height = 512.0f;
	math::rect rc(-width / 2.f, -height / 2.f, width, height);
	math::quad_tree<32> qtree(rc);

	std::unordered_map<math::objectid_t, player> players;

	auto insert = [&](math::objectid_t uid, float x, float y) {
		qtree.insert(uid, x, y);
		players.try_emplace(uid, player());
	};

	insert(1, 10.0f, 25.0f);
	insert(2, 10.0f, 20.0f);
	insert(3, 10.0f, 200.0f);
	insert(4, 100.0f, 200.0f);

	auto update = [&]()
	{
		for (auto& p : players)
		{
			p.second.version += 2;
			qtree.query(p.first, p.second.view_w, p.second.view_h, [&p](math::objectid_t id) {
				if (id == p.first)
				{
					return;
				}
				auto res = p.second.view.try_emplace(id, p.second.version);//新进入玩家
				if (!res.second)//玩家已经在视野内
				{
					res.first->second = p.second.version - 1;//更新已经在视野内玩家的版本号,比当前版本号小1
				}
			});
			print_view_event(p.first, p.second);//第一次遍历打印结果
		}
	};

	update();// 1 -><- 2;
	qtree.update(3, 20, 35);
	update();// 1 -><- 3;
	qtree.update(3, 25, 100);
	update();// 1 <--> 3; //离开
}

int main(int argc, const char** argv)
{
	printf("hello test!\n");
	//math::test_vec2();


	example();

	return 0;
}
