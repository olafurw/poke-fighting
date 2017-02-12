#include <random>
#include <algorithm>
#include <chrono>
#include <vector>
#include <cassert>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <string>

static const auto locSeed = std::chrono::system_clock::now().time_since_epoch().count();
static const int locGridSize = 128;
static const int locIterationCount = 3000;
static const unsigned int locTypeCount = 18;

enum Type
{
	Normal = 0,
	Fire,
	Water,
	Electric,
	Grass,
	Ice,
	Fighting,
	Poison,
	Ground,
	Flying,
	Psychic,
	Bug,
	Rock,
	Ghost,
	Dragon,
	Dark,
	Steel,
	Fairy
};

static std::map<Type, int> locWinCount;
static std::map<Type, int> locLossCount;
static std::map<Type, int> locAliveCount;
static int locFightCount = 0;

static const float locDamageGrid[locTypeCount][locTypeCount] = {
	{ 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.5f, 0.0f, 1.0f, 1.0f, 0.5f, 1.0f }, // Normal
	{ 1.0f, 0.5f, 0.5f, 1.0f, 2.0f, 2.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 2.0f, 0.5f, 1.0f, 0.5f, 1.0f, 2.0f, 1.0f }, // Fire
	{ 1.0f, 2.0f, 0.5f, 1.0f, 0.5f, 1.0f, 1.0f, 1.0f, 2.0f, 1.0f, 1.0f, 1.0f, 2.0f, 1.0f, 0.5f, 1.0f, 1.0f, 1.0f }, // Water
	{ 1.0f, 1.0f, 2.0f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 2.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.5f, 1.0f, 1.0f, 1.0f }, // Electric
	{ 1.0f, 0.5f, 2.0f, 1.0f, 0.5f, 1.0f, 1.0f, 0.5f, 2.0f, 0.5f, 1.0f, 0.5f, 2.0f, 1.0f, 0.5f, 1.0f, 0.5f, 1.0f }, // Grass
	{ 1.0f, 0.5f, 0.5f, 1.0f, 2.0f, 0.5f, 1.0f, 1.0f, 2.0f, 2.0f, 1.0f, 1.0f, 1.0f, 1.0f, 2.0f, 1.0f, 0.5f, 1.0f }, // Ice
	{ 2.0f, 1.0f, 1.0f, 1.0f, 1.0f, 2.0f, 1.0f, 0.5f, 1.0f, 0.5f, 0.5f, 0.5f, 2.0f, 0.0f, 1.0f, 2.0f, 2.0f, 0.5f }, // Fighting
	{ 1.0f, 1.0f, 1.0f, 1.0f, 2.0f, 1.0f, 1.0f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 2.0f }, // Poison
	{ 1.0f, 2.0f, 1.0f, 2.0f, 0.5f, 1.0f, 1.0f, 2.0f, 1.0f, 0.0f, 1.0f, 0.5f, 2.0f, 1.0f, 1.0f, 1.0f, 2.0f, 1.0f }, // Ground
	{ 1.0f, 1.0f, 1.0f, 0.5f, 2.0f, 1.0f, 2.0f, 1.0f, 1.0f, 1.0f, 1.0f, 2.0f, 0.5f, 1.0f, 1.0f, 1.0f, 0.5f, 1.0f }, // Flying
	{ 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 2.0f, 2.0f, 1.0f, 1.0f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.5f, 1.0f }, // Psychic
	{ 1.0f, 0.5f, 1.0f, 1.0f, 2.0f, 1.0f, 0.5f, 0.5f, 1.0f, 0.5f, 2.0f, 1.0f, 1.0f, 0.5f, 1.0f, 2.0f, 0.5f, 0.5f }, // Bug
	{ 1.0f, 2.0f, 1.0f, 1.0f, 1.0f, 2.0f, 0.5f, 1.0f, 0.5f, 2.0f, 1.0f, 2.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.5f, 1.0f }, // Rock
	{ 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 2.0f, 1.0f, 1.0f, 2.0f, 1.0f, 0.5f, 1.0f, 1.0f }, // Ghost
	{ 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 2.0f, 1.0f, 0.5f, 0.0f }, // Dragon
	{ 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.5f, 1.0f, 1.0f, 1.0f, 2.0f, 1.0f, 1.0f, 2.0f, 1.0f, 0.5f, 1.0f, 0.5f }, // Dark
	{ 1.0f, 0.5f, 0.5f, 0.5f, 1.0f, 2.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 2.0f, 1.0f, 1.0f, 1.0f, 0.5f, 2.0f }, // Steel
	{ 1.0f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 2.0f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 2.0f, 2.0f, 0.5f, 1.0f }  // Fairy
};

std::mt19937&
RandomGenerator()
{
	static std::mt19937 gen(locSeed);
	return gen;
}

int
RandomValue(
	const int	aStart,
	const int	aEnd)
{
	std::uniform_int_distribution<> dis(aStart, aEnd);

	return dis(RandomGenerator());
}

struct Position
{
			Position(): myX(0), myY(0) {}
			Position(const int aX, const int aY): myX(aX), myY(aY) {}
			Position(const Position& aPosition): myX(aPosition.myX), myY(aPosition.myY) {}

	void	DecrementX();
	void	IncrementX();
	void	DecrementY();
	void	IncrementY();

	int		myX;
	int		myY;
};

void
Position::DecrementX()
{
	myX -= 1;
	if (myX < 0)
	{
		myX = locGridSize - 1;
	}
}

void
Position::IncrementX()
{
	myX += 1;
	if (myX == locGridSize)
	{
		myX = 0;
	}
}

void
Position::DecrementY()
{
	myY -= 1;
	if (myY < 0)
	{
		myY = locGridSize - 1;
	}
}

void
Position::IncrementY()
{
	myY += 1;
	if (myY == locGridSize)
	{
		myY = 0;
	}
}

char
TypeToCharacter(
	const Type	aType)
{
	return static_cast<char>(aType + 65);
}

std::string
TypeToString(
	const Type	aType)
{
	switch (aType)
	{
		case Type::Normal: return "Normal";
		case Type::Fire: return "Fire";
		case Type::Water: return "Water";
		case Type::Electric: return "Electric";
		case Type::Grass: return "Grass";
		case Type::Ice: return "Ice";
		case Type::Fighting: return "Fighting";
		case Type::Poison: return "Poison";
		case Type::Ground: return "Ground";
		case Type::Flying: return "Flying";
		case Type::Psychic: return "Psychic";
		case Type::Bug: return "Bug";
		case Type::Rock: return "Rock";
		case Type::Ghost: return "Ghost";
		case Type::Dragon: return "Dragon";
		case Type::Dark: return "Dark";
		case Type::Steel: return "Steel";
		case Type::Fairy: return "Fairy";
		default: return "ERROR";
	}
}

float
GetEffectiveness(
	const Type	aAttacker,
	const Type	aDefender)
{
	return locDamageGrid[static_cast<unsigned int>(aAttacker)][static_cast<unsigned int>(aDefender)];
}

struct Pokemon
{
	int		myHealth;
	int		myDamage;
	Type	myType;
};

int
DamageTo(
	const Pokemon&	aPokemonA,
	const Pokemon&	aPokemonB)
{
	return aPokemonA.myDamage * GetEffectiveness(aPokemonA.myType, aPokemonB.myType);
}

void
Fight(
	Pokemon&	aPokemonA,
	Pokemon&	aPokemonB)
{
	aPokemonB.myHealth -= DamageTo(aPokemonA, aPokemonB);
}

Pokemon
Generate()
{
	Pokemon pokemon;
	pokemon.myDamage = RandomValue(1, 10);
	pokemon.myHealth = RandomValue(1, 20);
	pokemon.myType = static_cast<Type>(RandomValue(0, locTypeCount - 1));

	return pokemon;
}

void
Regenerate(
	const Type	aType,
	Pokemon&	aOutPokemon)
{
	aOutPokemon.myDamage = RandomValue(1, 10);
	aOutPokemon.myHealth = RandomValue(1, 20);
	aOutPokemon.myType = aType;
}

void
WeakestNeighbour(
	const Position								aPos,
	Position&									aOutNeighbourPos,
	const std::vector<std::vector<Pokemon>>&	aPokemans)
{
	std::vector<int> neighbourHealth;
	std::vector<Position> neigbourPos;

	neigbourPos.emplace_back(Position(aPos));
	Position& p0 = neigbourPos[0];
	p0.DecrementY();
	neighbourHealth.push_back(aPokemans[p0.myX][p0.myY].myHealth);

	neigbourPos.emplace_back(Position(aPos));
	Position& p1 = neigbourPos[1];
	p1.IncrementX();
	neighbourHealth.push_back(aPokemans[p1.myX][p1.myY].myHealth);

	neigbourPos.emplace_back(Position(aPos));
	Position& p2 = neigbourPos[2];
	p2.IncrementY();
	neighbourHealth.push_back(aPokemans[p2.myX][p2.myY].myHealth);

	neigbourPos.emplace_back(Position(aPos));
	Position& p3 = neigbourPos[3];
	p3.DecrementX();
	neighbourHealth.push_back(aPokemans[p3.myX][p3.myY].myHealth);

	int lowestHealth = -1;
	int lowestHealthIndex = 0;

	for (int i = 0; i < neighbourHealth.size(); ++i)
	{
		if (lowestHealth == -1)
		{
			lowestHealth = neighbourHealth[i];
			continue;
		}

		if (lowestHealth > neighbourHealth[i])
		{
			lowestHealth = neighbourHealth[i];
			lowestHealthIndex = i;
		}
	}

	aOutNeighbourPos.myX = neigbourPos[lowestHealthIndex].myX;
	aOutNeighbourPos.myY = neigbourPos[lowestHealthIndex].myY;
}

void
RandomNeighbour(
	const Position	aPos,
	Position&		aOutNeighbourPos)
{
	aOutNeighbourPos.myX = aPos.myX;
	aOutNeighbourPos.myY = aPos.myY;

	const int neighbourDirection = RandomValue(0, 3);
	
	if (neighbourDirection == 0)
	{
		aOutNeighbourPos.DecrementY();

		return;
	}
	else if (neighbourDirection == 1)
	{
		aOutNeighbourPos.IncrementX();

		return;
	}
	else if (neighbourDirection == 2)
	{
		aOutNeighbourPos.IncrementY();

		return;
	}
	else if (neighbourDirection == 3)
	{
		aOutNeighbourPos.DecrementX();

		return;
	}
}

void
Initialize(
	std::vector<std::vector<Pokemon>>&			aPokemans)
{
	for (int x = 0; x < locGridSize; x++)
	{
		std::vector<Pokemon> innerPokemans;

		for (int y = 0; y < locGridSize; y++)
		{
			innerPokemans.emplace_back(Generate());
		}

		aPokemans.emplace_back(innerPokemans);
	}
}

void
WriteToFile(
	const int									aIterationId,
	const std::vector<std::vector<Pokemon>>&	aPokemans)
{
	std::stringstream ss;
	for (int x = 0; x < locGridSize; ++x)
	{
		for (int y = 0; y < locGridSize; ++y)
		{
			ss << TypeToCharacter(aPokemans[x][y].myType);
		}

		ss << std::endl;
	}

	std::ofstream out("data/" + std::to_string(aIterationId) + ".txt", std::ios::out);
	out << ss.str();
	out.close();
}

void
Iterate(
	std::vector<std::vector<Pokemon>>&			aPokemans)
{
	for (int x = 0; x < locGridSize; ++x)
	{
		for (int y = 0; y < locGridSize; ++y)
		{
			Pokemon& pokemonA = aPokemans[x][y];

			Position pos(x, y);
			Position enemyPos;
			WeakestNeighbour(pos, enemyPos, aPokemans);

			Pokemon& pokemonB = aPokemans[enemyPos.myX][enemyPos.myY];

			Fight(pokemonA, pokemonB);

			if (pokemonB.myHealth <= 0)
			{
				if (pokemonA.myType != pokemonB.myType)
				{
					locWinCount[pokemonA.myType]++;
					locLossCount[pokemonB.myType]++;
					locFightCount++;
				}

				Regenerate(pokemonA.myType, pokemonB);	
			}
		}
	}
}

int main()
{
	std::vector<std::vector<Pokemon>> pokemans;

	Initialize(pokemans);
	WriteToFile(0, pokemans);

	for (int i = 1; i < locIterationCount; ++i)
	{
		Iterate(pokemans);
		WriteToFile(i, pokemans);

		if (i % 100 == 0)
		{
			std::cout << i << std::endl;
		}
	}

	std::cout << "Win Ratio" << std::endl;
	for (int i = 0; i < locTypeCount; ++i)
	{
		const float ratio = locWinCount[static_cast<Type>(i)] / static_cast<float>(locLossCount[static_cast<Type>(i)]);
		std::cout << TypeToString(static_cast<Type>(i)) << " " << ratio << std::endl;
	}

	

	for (int x = 0; x < locGridSize; ++x)
	{
		for (int y = 0; y < locGridSize; ++y)
		{
			locAliveCount[pokemans[x][y].myType]++;
		}
	}

	std::cout << "Alive Count" << std::endl;

	for (int i = 0; i < locTypeCount; ++i)
	{
		std::cout << TypeToString(static_cast<Type>(i)) << " " << locAliveCount[static_cast<Type>(i)] << std::endl;
	}

	std::cout << "Fight count" << std::endl;
	std::cout << locFightCount << std::endl;

	return 0;
}