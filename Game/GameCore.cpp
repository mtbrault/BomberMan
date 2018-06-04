/*
** EPITECH PROJECT, 2018
** game
** File description:
** game
*/

#include "GameCore.hpp"

GameCore::GameCore()
	:_id(1), _player1(-1, -1, -1), _player2(-1, -1, -1), _nbPlayer(0)
{
}

void GameCore::createEntities(std::vector<std::vector<char>> &map,
unsigned int &x, unsigned int &y, const parameters &params)
{
	y = 0;
	for (auto &line : map) {
		x = 0;
		_vectorEntities.push_back(std::vector<std::unique_ptr<EntityPos>>());
		for (auto &c : line) {
			if (c == '0') {
				_vectorEntities[y].push_back(std::make_unique<EntityPos>(ItemStatic::WALL, static_cast<float>(x), static_cast<float>(y), _id));
				_id++;
			} else if (c == '1') {
				_vectorEntities[y].push_back(std::make_unique<EntityPos>(ItemStatic::CRATE, static_cast<float>(x), static_cast<float>(y), _id));
				_id++;
			} else if (c == '2') {
				if (_nbPlayer == 0) {
					_mobileEntities.push_back(std::make_unique<Player>(static_cast<float>(x), static_cast<float>(y), _id, 0));
					_player1 = Player(static_cast<float>(x), static_cast<float>(y), _id);
					_vectorEntities[y].push_back(std::make_unique<EntityPos>());
					_nbPlayer++;
				} else {
					_mobileEntities.push_back(std::make_unique<Player>(static_cast<float>(x), static_cast<float>(y), _id, 1));
					_player2 = Player(static_cast<float>(x), static_cast<float>(y), _id);
					_vectorEntities[y].push_back(std::make_unique<EntityPos>());
				}
				_id++;
			} else if (c == '4') {
				_vectorEntities[y].push_back(std::make_unique<EntityPos>());
				if (static_cast<int>(_iaList.size()) < params.nbBots) {
					_mobileEntities.push_back(std::make_unique<Player>(static_cast<float>(x), static_cast<float>(y), _id));
					_iaList.push_back(Player(static_cast<float>(x), static_cast<float>(y), _id));
					_id++;
				}
			} else
				_vectorEntities[y].push_back(std::make_unique<EntityPos>());
			x += 1;
		}
		y += 1;
	}
}

void    GameCore::init(parameters params)
{
	unsigned int x = 0;
	unsigned int y = 0;
	MapGenerator generator(10, 10);
	_nbPlayer = 0;

	std::cout << "Initializing new game" << std::endl;

	generator.generateMap();
	generator.generatePlayers(2, params.nbBots);
	generator.dispMap();
	std::vector<std::vector<char>> map = generator.getMap();
	createEntities(map, x, y, params);
	_pauseitem.push_back(std::unique_ptr<IEntity>(new MenuItem(Entity::BUTTON, PAUSE_ID, "Resume", (SCREEN_WIDTH / 2) - 200, 200, 400, 100)));
	_pauseitem.push_back(std::unique_ptr<IEntity>(new MenuItem(Entity::BUTTON, PAUSE_ID + 1, "Save and Quit", (SCREEN_WIDTH / 2) - 200, 350, 400, 100)));
	_pauseitem.push_back(std::unique_ptr<IEntity>(new MenuItem(Entity::BUTTON, PAUSE_ID + 2, "Main Menu", (SCREEN_WIDTH / 2) - 200, 500, 400, 100)));
	_pauseitem.push_back(std::unique_ptr<IEntity>(new MenuItem(Entity::BUTTON, PAUSE_ID + 3, "Quit", (SCREEN_WIDTH / 2) - 200, 650, 400, 100)));
	_size.x = x;
	_size.y = y;
}

GameCore::~GameCore()
{
	if (_updateEntities.size() > 0)
		releaseUpdateEntities();
}

pairUC GameCore::getSize() const
{
	return (std::make_pair(_size.x, _size.y));
}

std::vector<std::vector<std::unique_ptr<EntityPos>>> &GameCore::getEntities()
{
	return _vectorEntities;
}

std::vector<std::unique_ptr<IEntity>> &GameCore::getMobileEntities()
{
	return _mobileEntities;
}

void GameCore::releaseUpdateEntities()
{
	for (auto &i : _updateEntities)
		i.release();
	_updateEntities.clear();
}

std::vector<std::pair<int, Entity>> &GameCore::getEntitiesToRemove()
{
	return _entitiesToRemove;
}

void GameCore::playerDropBomb(Player &player)
{
	auto pos = player.getPos();
	for (auto &a : _bombs) {
			if (a.getPos().first == std::ceil(pos.first - 0.5) &&
			    std::ceil(pos.second - 0.5) == a.getPos().second) {
				pos.first = -1;
				pos.second = -1;
				return;
			}
	}
	if (pos.first != -1 && pos.second != -1) {
		Bomb tmp(std::ceil(pos.first - 0.5), std::ceil(pos.second - 0.5), _id, player.getId());
		player.dropBomb();
		tmp.setPower(player.getPower());
		tmp.setSuper(player.getSuper());
		_bombs.push_back(tmp);
		_updateEntities.push_back(std::make_unique<Bomb>(std::ceil(pos.first - 0.5), std::ceil(pos.second - 0.5), _id, player.getId()));
		_id++;
	}
}

void GameCore::bombManager(Actions &act)
{
	for (auto &a : _bombs) {
		if (a.isOutFire()) {
			for (auto &b : a.getFlames())
				_entitiesToRemove.push_back(std::make_pair<int, Entity>(b.getId(),
							b.getType()));
		}
	} // Plus tard les deux boucles seront assemblables
	_bombs.erase(std::remove_if(_bombs.begin(), _bombs.end(), [](const Bomb &x) {
			     return x.isOver();
		     }),
		     _bombs.end());
	if (act.space == true && _player1.getBombCount() > 0 && _player1.isAlive())
		playerDropBomb(_player1);
	if (act.W == true && _player2.getBombCount() > 0 && _player2.isAlive()) // remplacer 0 par player2.
		playerDropBomb(_player2);
	for (auto &a : _bombs) {
		a.tick(_id, _vectorEntities, _entitiesToRemove, _updateEntities);
		if (a.isExplode()) {
			std::vector<Fire> &vec = a.getFlames();
			for (auto &b : vec) {
				_updateEntities.push_back(std::unique_ptr<IEntity>(&b));
				if (std::round(_player1.getPos().first) == b.getPos().first &&
				std::round(_player1.getPos().second) == b.getPos().second) {
					_player1.setAlive(false);
					_updateEntities.push_back(std::unique_ptr<IEntity>(&_player1));
				}
				if (std::round(_player2.getPos().first) == b.getPos().first &&
				std::round(_player2.getPos().second) == b.getPos().second) {
					_player2.setAlive(false);
					_updateEntities.push_back(std::unique_ptr<IEntity>(&_player2));
				}
				for (auto &c : _bombs) {
					if (c.isAlive() && c.getPos().first == b.getPos().first
					&& c.getPos().second == b.getPos().second)
						c.detonate();
				}
				for (auto &it : _iaList) {
					if (std::round(it.getPos().first) == b.getPos().first &&
					    std::round(it.getPos().second) == b.getPos().second)
					{
						it.setAlive(false);
						_updateEntities.push_back(std::unique_ptr<IEntity>(&it));
					}
				}
			}
			_updateEntities.push_back(std::unique_ptr<IEntity>(&a));
			if (a.getOwner() == static_cast<unsigned int>(_player1.getId()))
				_player1.addBomb();
			if (a.getOwner() == static_cast<unsigned int>(_player2.getId()))
				_player2.addBomb();
			break;
		}
	}
}

bool GameCore::thereIsBomb(int x, int y)
{
	for (auto &it : _bombs)
	{
		if (it.getPos().first == x && it.getPos().second == y) {
			return (true);
		}
	}
	return (false);
}

void	GameCore::movePlayer(std::pair<float, float> from, std::pair<int, int> dir, Player &player, float rotation)
{
	if (_vectorEntities[std::round(from.second + 0.5 * dir.second)][std::round(from.first + 0.5 * dir.first)]->isEmpty() == true) {
		if ((thereIsBomb(std::round(from.first + 0.5 * dir.first), std::round(from.second + 0.5 * dir.second)) == false ||
		thereIsBomb(std::round(from.first), std::round(from.second)) == true))
			player.setPos(from.first + (0.07 + player.getSpeed()) * dir.first, from.second + (0.07 + player.getSpeed()) * dir.second);
	} else if (_vectorEntities[std::round(from.second + 0.5 * dir.second)][std::round(from.first + 0.5 * dir.first)]->getType() == Entity::ITEM) {
		player.pickupItem(_vectorEntities[std::round(from.second + 0.5 * dir.second)][std::round(from.first + 0.5 * dir.first)]->getEntity());
		_entitiesToRemove.push_back(std::make_pair<int, Entity>(_vectorEntities[std::round(from.second + 0.5 * dir.second)][std::round(from.first + 0.5 * dir.first)]->getId(),
		_vectorEntities[std::round(from.second + 0.5 * dir.second)][std::round(from.first + 0.5 * dir.first)]->getType()));
		_vectorEntities[std::round(from.second + 0.5 * dir.second)][std::round(from.first + 0.5 * dir.first)]->removeFirstEntity();
		player.setPos(from.first + (0.07 + player.getSpeed()) * dir.first, from.second + (0.07 + player.getSpeed()) * dir.second);
	}
	player.setRotation(rotation);
}

bool 	GameCore::playerMovement(Actions act)
{
	bool changed = false;
	std::pair<float, float>	playerPos;

	movement	movement_table[8] = {
		{act.right, std::make_pair(1, 0), 0.0f, 1},
		{act.left, std::make_pair(-1, 0), 180.0f, 1},
		{act.up, std::make_pair(0, 1), -90.0f, 1},
		{act.down, std::make_pair(0, -1), 90.0f, 1},
		{act.D, std::make_pair(1, 0), 0.0f, 2},
		{act.Q, std::make_pair(-1, 0), 180.0f, 2},
		{act.Z, std::make_pair(0, 1), -90.0f, 2},
		{act.S, std::make_pair(0, -1), 90.0f, 2}
	};
	for (int i = 0; i < 8; i++) {
		if (movement_table[i].action == true) {
			playerPos = (movement_table[i].player == 1) ? _player1.getPos() : _player2.getPos();
			movePlayer(playerPos, movement_table[i].dir, (movement_table[i].player == 1) ? _player1 : _player2, movement_table[i].rotation);
			changed = true;
		}
	}
	return (changed);
}

bool GameCore::checkEnd(STATE &state)
{
	if (!_player1.isAlive() || !_player2.isAlive()) {
		state = STATE::END;
		initEndScreen();
		return true;
	}
	return false;
}

int	GameCore::dodgeBomb(std::pair<float, float> bombPos, std::pair<float, float> myPos, Player &player)
{
	if (bombPos.second == std::round(myPos.second)) {
		if (_vectorEntities[std::round(myPos.second + 1)][std::round(myPos.first)]->isEmpty()) {
			movePlayer(myPos, {0, 1}, player, -90.0f);
		}
		else if (_vectorEntities[std::round(myPos.second - 1)][std::round(myPos.first)]->isEmpty())
			movePlayer(myPos, {0, -1}, player, 90.0f);
		else {
			if (bombPos.second > myPos.second)
				movePlayer(myPos, {-1, 0}, player, 0.0f);
			else
				movePlayer(myPos, {1, 0}, player, 180.0f);
		}
	}
	else if (bombPos.first == std::round(myPos.first)) {
		if (_vectorEntities[std::round(myPos.second)][std::round(myPos.first + 1)]->isEmpty())
			movePlayer(myPos, {1, 0}, player, 0.0f);
		else if (_vectorEntities[std::round(myPos.second)][std::round(myPos.first - 1)]->isEmpty())
			movePlayer(myPos, {-1, 0}, player, 180.0f);
		else {
			if (bombPos.first > myPos.first)
				movePlayer(myPos, {0, -1}, player, 90.0f);
			else
				movePlayer(myPos, {0, 1}, player, -90.0f);
		}
	}
	return 1;
}

bool	GameCore::haveBombed(Player player)
{
	int	id = player.getId();
	int	posed = 0;

	for (auto &it : _bombs) {
		if (it.getOwner() == static_cast<size_t>(id))
			posed = 1;
	}
	return (posed == 1) ? true : false;
}

void	GameCore::iaAction(std::unique_ptr<EntityPos> &entity, Player &player, std::pair<int, int> dir, float orientation)
{
	if (entity->isEmpty())
		movePlayer(player.getPos(), dir, player, orientation);
	else if (entity->getSubType() == ItemStatic::CRATE && !haveBombed(player))
	        playerDropBomb(player);
	else
		player.setIa(std::make_pair(0, 0));
}

bool	GameCore::existBomb(float second, float first)
{
	std::pair<float, float>	bomb;
	
	for (auto &it : _bombs) {
		bomb = it.getPos();
		if (bomb.first == std::round(first) || bomb.second == std::round(second))
			return true;
	}
	return false;
}

void	GameCore::iaMoving(Player &player)
{
	std::pair<int, int>	dir = player.getIa();
	std::pair<float, float>	myPos = player.getPos();
	int			rand;

	if (dir == std::make_pair(0, 1) && !existBomb(myPos.second + 1, myPos.first))
		iaAction(_vectorEntities[std::round(myPos.second + 1)][std::round(myPos.first)], player, {0, 1}, -90.0f);
	else if (dir == std::make_pair(0, -1)  && !existBomb(myPos.second - 1, myPos.first))
	        iaAction(_vectorEntities[std::round(myPos.second - 1)][std::round(myPos.first)], player, {0, -1}, 90.0f);
	else if (dir == std::make_pair(1, 0) && !existBomb(myPos.second, myPos.first + 1))
		iaAction(_vectorEntities[std::round(myPos.second)][std::round(myPos.first + 1)], player, {1, 0}, 0.0f);
	else if (dir == std::make_pair(-1, 0) && !existBomb(myPos.second, myPos.first - 1))
		iaAction(_vectorEntities[std::round(myPos.second)][std::round(myPos.first - 1)], player, {-1, 0}, 180.0f);
	else if (dir == std::make_pair(0, 0)) {
		std::default_random_engine re(std::chrono::system_clock::now().time_since_epoch().count());
		std::uniform_int_distribution<int>	distrib{1, 4};
		rand = distrib(re);
		if (rand == 1)
			player.setIa(std::make_pair(0, 1));
		else if (rand == 2)
			player.setIa(std::make_pair(0, -1));
		else if (rand == 3)
			player.setIa(std::make_pair(1, 0));
		else
		        player.setIa(std::make_pair(-1, 0));
	}
}

void	GameCore::handleIA()
{
	int			dodging = 0;
	std::pair<float, float>	myPos;
	std::pair<float, float>	bombPos;

	for (auto &it : _iaList) {
		if (it.isAlive() == false)
			continue ;
		myPos = it.getPos();
	        for (auto &i : _bombs) {
			bombPos = i.getPos();
			if (bombPos.second == std::round(myPos.second) || bombPos.first == std::round(myPos.first))
				dodging = dodgeBomb(bombPos, myPos, it);
		}
		if (dodging == 0)
			iaMoving(it);
		_updateEntities.push_back(std::unique_ptr<IEntity>(&it));
	}
}

std::vector<std::unique_ptr<IEntity>> &GameCore::calc(Actions act, STATE &state)
{
	bool changed;

	if (_updateEntities.size() > 0)
		releaseUpdateEntities();
	if (checkEnd(state) == true)
		return _updateEntities;
	changed = playerMovement(act);
	if (changed)
	{
		_updateEntities.push_back(std::unique_ptr<IEntity>(&_player1));
		_updateEntities.push_back(std::unique_ptr<IEntity>(&_player2));
	}
	handleIA();
	bombManager(act);
	return (_updateEntities);
}

void GameCore::initEndScreen()
{
	if (!_player1.isAlive() && !_player2.isAlive())
		_endItem.push_back(std::unique_ptr<IEntity>(new MenuItem(Entity::LABEL, LOSE_ID, "You lose", (SCREEN_WIDTH / 2) - 200, 200, 400, 100)));
	else if (!_player1.isAlive())
		_endItem.push_back(std::unique_ptr<IEntity>(new MenuItem(Entity::LABEL, WIN_P2_ID, "Player 2 won", (SCREEN_WIDTH / 2) - 200, 200, 400, 100)));
	else if (!_player2.isAlive())
		_endItem.push_back(std::unique_ptr<IEntity>(new MenuItem(Entity::LABEL, WIN_P1_ID, "Player 1 won", (SCREEN_WIDTH / 2) - 200, 200, 400, 100)));
	else
		_endItem.push_back(std::unique_ptr<IEntity>(new MenuItem(Entity::LABEL, LOSE_ID, "You lose", (SCREEN_WIDTH / 2) - 200, 200, 400, 100)));
	_endItem.push_back(std::unique_ptr<IEntity>(new MenuItem(Entity::BUTTON, PLAY_AGAIN_ID, "Play again", (SCREEN_WIDTH / 2) - 200, 400, 400, 100)));
	_endItem.push_back(std::unique_ptr<IEntity>(new MenuItem(Entity::BUTTON, QUIT_END_ID, "Quit", (SCREEN_WIDTH / 2) - 200, 600, 400, 100)));
}

void GameCore::removeAll()
{
	for (auto &it : _vectorEntities)
		it.clear();
	_vectorEntities.clear();
	_mobileEntities.clear();
	_bombs.clear();
	_endItem.clear();
	_pauseitem.clear();
	_entitiesToRemove.clear();
	_entities.clear();
	// _updateEntities.clear();
	_iaList.clear();
	_id = 0;
}

void	GameCore::saveMap(std::ofstream &file)
{
	for (unsigned int idx = 0; idx != _vectorEntities.size(); idx++) {
		for (auto &i2 : _vectorEntities[idx]) {
			if (i2->isEmpty() == false && i2->getType() == Entity::CUBE) {
				if (i2->getSubType() == ItemStatic::WALL)
					file << "0";
				else if (i2->getSubType() == ItemStatic::CRATE)
					file << "1";
			}
			else
				file << " ";
		}
		file << "\n";
	}
}

std::map<eItem, std::string> myItem =
{
	{BOMB_UP, "BOMB_UP"},
	{POWER_UP, "POWER_UP"},
	{SPEED, "SPEED"},
	{SUPER_BOMB, "SUPER_BOMB"},
	{WALL_PASS, "WALL_PASS"},
	{KICK, "KICK"},
	{NONE, "NONE"}
};

void 	GameCore::saveMobileEntities(std::ofstream &file)
{
	file << "Player1 " << std::round(_player1.getX()) << " " << std::round(_player1.getY()) << "\n";
	if (true)
		file << "Player2 " << std::round(_player2.getX()) << " " << std::round(_player2.getY()) << "\n";
	for (auto &i : _bombs)
		file << "Bomb " << i.getX() << " " << i.getY() << "\n";
	for (unsigned int idx = 0; idx != _vectorEntities.size(); idx++) {
		for (unsigned int y = 0; y != _vectorEntities[idx].size(); y++) {
			if (_vectorEntities[idx][y]->isEmpty() == false &&
				_vectorEntities[idx][y]->getType() == Entity::ITEM) {
				auto pos = _vectorEntities[idx][y]->getEntity()->getPos();
				file << "Item " << pos.first << " " << pos.second << " " <<
				myItem[static_cast<Item *>(_vectorEntities[idx][y]->getEntity().get())->getItemType()] << "\n";
			}
		}
	}
}

void GameCore::handlePause(Actions actions, STATE &state)
{

	if (actions.buttonPressed == PAUSE_ID)
		state = STATE::GAME;
	if (actions.buttonPressed == PAUSE_ID + 1) {
		/*std::ofstream file("save.txt");
		saveMap(file);
		file << "separateur\n";
		saveMobileEntities(file);
		file.close();*/
	}
	if (actions.buttonPressed == PAUSE_ID + 2) {
		removeAll();
		state = STATE::MENU;
	}
	if (actions.buttonPressed == PAUSE_ID + 3)
		state = STATE::EXIT;
}

int GameCore::getEndId() const
{
	if (!_player1.isAlive() && !_player2.isAlive())
		return LOSE_ID;
	else if (!_player1.isAlive())
		return WIN_P2_ID;
	else if (!_player2.isAlive())
		return WIN_P1_ID;
	else
		return LOSE_ID;
}

void GameCore::handleEnd(Actions actions, STATE &state)
{
	if (actions.buttonPressed == PLAY_AGAIN_ID) {
		removeAll();
		state = STATE::GAME;
	} if (actions.buttonPressed == QUIT_END_ID) {
		removeAll();
		state = STATE::EXIT;
	}
}

std::vector<std::unique_ptr<IEntity>> &GameCore::createPause()
{
	return _pauseitem;
}

std::vector<std::unique_ptr<IEntity>> &GameCore::createEndScreen()
{
	return _endItem;
}
