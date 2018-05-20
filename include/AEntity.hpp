/*
** EPITECH PROJECT, 2018
** Indie studio
** File description:
** header of abstract entity
*/

#ifndef AENTITY_HPP_
# define AENTITY_HPP_

# include <string>
# include "IEntity.hpp"

class AEntity : public IEntity {
public:
    ~AEntity() {}
    bool isAlive() const { return _alive; }
    int getX() const { return _x; }
    int getY() const { return _y; }
    bool isSuperpoz() const { return _superposable; }
    bool isDestruct() const { return _destructible; }
    Entity getType() const { return _type; };

private:
    bool _alive;
    bool _superposable;
    bool _destructible;
    Entity _type;
    int _x;
    int _y;
};

#endif /* !AENTITY_HPP_ */
