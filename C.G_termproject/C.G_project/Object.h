#pragma once
class Object
{
public:
	Object();
	~Object();

public:
    float x_trans{}, y_trans{}, z_trans{};
    float x_trans_aoc{}, y_trans_aoc{}, z_trans_aoc{};
    float x_rotate{}, y_rotate{}, z_rotate{};
    float x_rotate_aoc{}, y_rotate_aoc{}, z_rotate_aoc{};
    float x_scale{}, y_scale{}, z_scale{};
    float x_scale_aoc{}, y_scale_aoc{}, z_scale_aoc{};
    float color_r{}, color_g{}, color_b{};
    float r{};

public:

};

class Bullet : public Object
{
public:
    Bullet();
    ~Bullet();
};

class Building : public Object
{
public:
    Building();
    ~Building();
};