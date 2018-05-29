/*
** EPITECH PROJECT, 2018
** Player
** File description:
** player
*/

#include "Player.hpp"

Player::Player(float x, float y, unsigned int id)
	:_speed(1.0f), _bombs(2), _pow(3)
{
	_x = x;
	_y = y;
	_id = id;
	_alive = true;
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

float	Player::getSpeed() const
{
	return (_speed);
}

int	Player::getBombCount() const
{
	return (_bombs);
}

bool 	Player::isAlive() const
{
	return (_alive);
}

void 	Player::setAlive(bool alive)
{
	_alive = alive;
}

pairUC	Player::getPos() const
{
	return std::make_pair(_x, _y);
}

int	Player::getPower() const
{
	return _pow;
}

void	Player::addSpeed()
{
	_speed += 0.1;
}

void	Player::addPower()
{
	_pow += 1;
}

void	Player::pickupItem(std::unique_ptr<IEntity> &item)
{
	eItem	tmp = static_cast<Item *>(item.get())->getItemType();

	if (tmp == eItem::BOMB_UP) {
		this->addBomb();
		std::cout << "Picked up Bomb" << std::endl;
	} else if (tmp == eItem::POWER_UP) {
		this->addPower();
		std::cout <<  "Picked up Power up" << std::endl;
	} else if (tmp == eItem::SPEED) {
		std::cout << "Picked up Speed bonus" << std::endl;
		this->addSpeed();
	}
}

Entity	Player::getType() const
{
	return Entity::PLAYER;
}

void Player::setRotation(float rotation)
{
	_rotation = rotation;
}
