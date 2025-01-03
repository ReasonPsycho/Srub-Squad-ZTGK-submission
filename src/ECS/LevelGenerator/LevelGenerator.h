//
// Created by Seren on 24.04.2024.
//

#pragma once

#include <cstddef>
#include <cstdint>
#include <limits>
#include <ostream>
#include <random>
#include <utility>
#include <vector>
#include <glm/glm.hpp>
#include "ECS/Utils/Random.h"

struct ItemTemplate {
	static constexpr int unlimited = std::numeric_limits<int>::max();
	int id = -1;
	float chanceEarlyGame = 1.f;
	float chanceLateGame = 1.f;
	int maxCount = unlimited;
};

struct EnemyTemplate {
	char id = '?';
	float chanceEarlyGame = 1.f;
	float chanceLateGame = 1.f;
};

struct LevelLayout {
	struct Tile {
		enum class Type {
			wall,
			floor,
			chest,
			ore,
			core,
			unit,
			enemy,
		};

		Type type = Type::wall;
		int itemId = -1;
		char enemyId = '?';
		int pocketIndex = -1;
		bool dfsVisited = false;
	};

	PcgEngine::seed_type seed {};
	glm::ivec2 size {};
	std::vector<Tile> grid;
};

std::ostream& operator<<(std::ostream& os, const LevelLayout& level);

class LevelGenerator {
public:
	struct Config {
		PcgEngine::seed_type seed {};
		glm::ivec2 size {};
		float wallThickness {};
		float baseRadius {};
		float keyRadius {};
		float pocketRadius {};
		float noiseImpact {};
		std::vector<float> keyDistances {};
		int extraPocketAttempts {};
		int keyEnemies {};
		int minEnemies {};
		int maxEnemies {};
		int unitCount {};
		int chestCount {};
		std::vector<ItemTemplate> lootTable {};
		std::vector<EnemyTemplate> encounterTable {};
	};

	inline LevelGenerator(LevelLayout& level) noexcept : level(level) {}
	void operator()(const Config& config);

private:
	using Tile = LevelLayout::Tile;
	struct Pocket {
		enum class Type {
			standard,
			key,
			base,
		};
		Type type = Type::standard;
		glm::vec2 center {};
		float radius {};
		bool hasChest {};
	};

	void generatePerlinNoiseGrid(glm::ivec2 size, PcgEngine& rand) noexcept;
	float getPerlinNoise(glm::vec2 pos) const noexcept;

	bool inBounds(glm::ivec2 pos) const noexcept;
	const Tile* getTile(glm::ivec2 pos) const noexcept;
	Tile* getTile(glm::ivec2 pos) noexcept;

	template<class Pred>
	bool isTileAdjacentTo(glm::ivec2 pos, Pred&& predicate) const noexcept(noexcept(predicate(glm::ivec2 {})));
	template<class F>
	void dfs(glm::ivec2 pos, int index, F&& f) noexcept(noexcept(f(glm::ivec2 {})));
	void clearDfsVisitedFlags(glm::ivec2 pos) noexcept;
	template<class F>
	void forEachPocketTile(int index, F&& f) noexcept(noexcept(f(glm::ivec2 {})));
	template<class Pred>
	std::vector<glm::ivec2> reservoirSamplePocket(int index, int count, PcgEngine& rand, Pred&& predicate) noexcept(noexcept(predicate(glm::ivec2 {})));

	bool hasRoomForPocket(glm::vec2 pos, float radius) const noexcept;
	int tryMakePocket(glm::vec2 pos, float radius, Pocket::Type type) noexcept;

	bool isTileSafeToHollowOut(glm::ivec2 pos) const noexcept;
	void hollowOutPocket(int index, float padding, float noiseImpact) noexcept;

	template<class Pred>
	int addAtRandomToPocket(int index, int count, LevelLayout::Tile::Type type, PcgEngine& rand, Pred&& predicate) noexcept(noexcept(predicate(glm::ivec2 {})));
	void addEnemiesToPocket(int index, int count, PcgEngine& rand) noexcept;
	bool addChestToPocket(int index, PcgEngine& rand) noexcept;

	void assignChestItems(glm::vec2 center, const std::vector<ItemTemplate>& lootTable, PcgEngine& rand);
	void assignEnemyTypes(glm::vec2 center, const std::vector<EnemyTemplate>& encounterTable, PcgEngine& rand);

	static constexpr glm::ivec2 directions[4] {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

	LevelLayout& level;
	std::vector<Pocket> pockets;
	std::vector<glm::vec2> perlinNoiseGrid;
	glm::ivec2 perlinNoiseGridSize {};
};

LevelLayout generateLevel(const LevelGenerator::Config& config);

template<class Pred>
inline bool LevelGenerator::isTileAdjacentTo(glm::ivec2 pos, Pred&& predicate) const noexcept(noexcept(predicate(glm::ivec2 {}))) {
	for (const auto& dir : directions) {
		if (predicate(pos + dir))
			return true;
	}
	return false;
}

template<class F>
inline void LevelGenerator::dfs(glm::ivec2 pos, int index, F&& f) noexcept(noexcept(f(glm::ivec2 {}))) {
	auto tile = getTile(pos);
	if (tile == nullptr || tile->pocketIndex != index || tile->dfsVisited)
		return;
	tile->dfsVisited = true;
	f(pos);
	for (const auto& dir : directions) {
		dfs(pos + dir, index, f);
	}
}

template<class F>
inline void LevelGenerator::forEachPocketTile(int index, F&& f) noexcept(noexcept(f(glm::ivec2 {}))) {
	glm::ivec2 pos(pockets[index].center);
	dfs(pos, index, std::forward<F>(f));
	clearDfsVisitedFlags(pos);
}

template<class Pred>
inline std::vector<glm::ivec2> LevelGenerator::reservoirSamplePocket(int index, int count, PcgEngine& rand, Pred&& predicate) noexcept(noexcept(predicate(glm::ivec2 {}))) {
	std::vector<glm::ivec2> result;
	int soFar = 0;
	forEachPocketTile(index, [&](glm::ivec2 pos) {
		if (predicate(pos)) {
			if (soFar < count) {
				result.push_back(pos);
			} else {
				std::uniform_int_distribution<int> dist(0, soFar);
				auto index = dist(rand);
				if (index < count)
					result[index] = pos;
			}
			soFar++;
		}
	});
	return result;
}

template<class Pred>
inline int LevelGenerator::addAtRandomToPocket(int index, int count, LevelLayout::Tile::Type type, PcgEngine& rand, Pred&& predicate) noexcept(noexcept(predicate(glm::ivec2 {}))) {
	auto positions = reservoirSamplePocket(index, count, rand, [&](glm::ivec2 pos) {
		return getTile(pos)->type == Tile::Type::floor && predicate(pos);
	});
	for (const auto& pos : positions) {
		getTile(pos)->type = type;
	}
	return static_cast<int>(positions.size());
}
