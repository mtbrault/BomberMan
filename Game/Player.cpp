/*
** EPITECH PROJECT, 2018
** Player
** File description:
** player
*/

#include "Player.hpp"

Player::Player(float x, float y, unsigned int id)
	:_bombs(2)
{
	_x = x;
	_y = y;
	_id = id;
}

void 	Player::poke()
{
	std::cout << "Player here, pos " << _x << ":" << _y << std::endl;
}

void	Player::dropBomb()
{
	_bombs -= 1;
}

void	Player::addBomb()
{
	_bombs += 1;
}

int	Player::getBombCount() const
{
	return (_bombs);
}

bool 	Player::isAlive() const
{
	std::cout << "Player here, pos " << _x << ":" << _y << std::endl;
	return (_alive);
}

pairUC	Player::getPos() const
{
	return std::make_pair(_x, _y);
}

Entity	Player::getType() const
{
	return Entity::PLAYER;
}

void Player::setRotation(float rotation)
{
	_rotation = rotation;
}
