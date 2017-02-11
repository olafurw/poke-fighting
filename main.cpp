#include <random>
#include <algorithm>
#include <chrono>
#include <vector>
#include <cassert>
#include <iostream>
#include <fstream>
#include <sstream>

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

char
TypeToCharacter(
	const Type	aType)
{
	return static_cast<char>(aType + 65);
}

static const auto locSeed = std::chrono::system_clock::now().time_since_epoch().count();
static const int locGridSize = 800;

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

static const unsigned int locTypeCount = 18;
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

void
Fight(
	Pokemon&	aPokemonA,
	Pokemon&	aPokemonB)
{
	const int damageA = aPokemonA.myDamage * GetEffectiveness(aPokemonA.myType, aPokemonB.myType);
	aPokemonB.myHealth -= damageA;
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
RandomNeighbour(
	const int	aX,
	const int	aY,
	int&		aOutX,
	int&		aOutY)
{
	aOutX = aX;
	aOutY = aY;

	const int neighbourDirection = RandomValue(0, 3);
	
	if (neighbourDirection == 0)
	{
		aOutY -= 1;
		if (aOutY < 0)
		{
			aOutY = locGridSize - 1;
		}

		return;
	}
	else if (neighbourDirection == 1)
	{
		aOutX += 1;
		if (aOutX == locGridSize)
		{
			aOutX = 0;
		}

		return;
	}
	else if (neighbourDirection == 2)
	{
		aOutY += 1;
		if (aOutY == locGridSize)
		{
			aOutY = 0;
		}

		return;
	}
	else if (neighbourDirection == 3)
	{
		aOutX -= 1;
		if (aOutX < 0)
		{
			aOutX = locGridSize - 1;
		}

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

			int enemyX = 0;
			int enemyY = 0;
			RandomNeighbour(x, y, enemyX, enemyY);

			Pokemon& pokemonB = aPokemans[enemyX][enemyY];

			Fight(pokemonA, pokemonB);

			if (pokemonB.myHealth <= 0)
			{
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

	for (int i = 1; i < 400; ++i)
	{
		Iterate(pokemans);
		WriteToFile(i, pokemans);
	}

	return 0;
}