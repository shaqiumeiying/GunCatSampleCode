//// Header
//#include "world_system.hpp"
//#include "world_init.hpp"
//#include "level_system.hpp";
//#include "weapons/bullet_system.hpp"
//#include "physics/physics_system.hpp"
//#include "weapons/weapon_system.hpp"
//
//
//// stlib
//#include <cassert>
//#include <sstream>
//#include <iostream>
//#include <tuple>
//
//////////////////////// Load LDtk level data///////////////////////////
//const std::string level_1 = ldtk_path("Level1.ldtk");
//const ldtk::Level *loadLDtkFile(const std::string &filepath)
//{
//	try
//	{
//		static ldtk::Project project;
//		project.loadFromFile(filepath);
//
//		const auto &world = project.getWorld();
//		const auto &level = world.getLevel("Level_0");
//
//		return &level; // Return the level pointer
//	}
//	catch (const std::invalid_argument &e)
//	{
//		std::cerr << "Error loading LDtk data: " << e.what() << std::endl;
//	}
//	catch (const std::exception &e)
//	{
//		std::cerr << "Unexpected error: " << e.what() << std::endl;
//	}
//	return nullptr; // Return nullptr on error
//}
//
//
//// load Player position from layer
//vec2 getPlayerPosition(const ldtk::Level *level)
//{
//	if (!level)
//	{
//		std::cerr << "Level not loaded." << std::endl;
//		return {100, window_height_px - 400}; // Default position
//	}
//
//	for (const auto &layer : level->allLayers())
//	{
//		if (layer.getName() == "Entities" && layer.hasEntity("Player"))
//		{
//			const auto &players = layer.getEntitiesByName("Player");
//			if (!players.empty())
//			{
//				const auto &player = players[0].get();
//				auto pos = player.getPosition();
//				return {pos.x, pos.y};
//			}
//		}
//	}
//	return {100, window_height_px - 400}; // Default if Player is not found
//}
//// load enemy position and corresponding patrol boxes
//// tuple (patrol_box, list_of_enemie_positions)
//std::unordered_map<int, std::tuple<std::tuple<vec2, vec2>, std::vector<vec2>>>
//getEnemyFlyerData(const ldtk::Level *level)
//{
//	std::unordered_map<int, std::tuple<std::tuple<vec2, vec2>, std::vector<vec2>>> enemyFlyerData;
//
//	if (!level)
//	{
//		std::cerr << "Level not loaded." << std::endl;
//		return enemyFlyerData; // Return empty vector if level not loaded
//	}
//
//	for (const auto &layer : level->allLayers())
//	{
//		if (layer.getName() == "Entities" && layer.hasEntity("Enemy_Flyer"))
//		{
//			const auto &enemy_flyers = layer.getEntitiesByName("Enemy_Flyer");
//			for (const auto &entity_ref : enemy_flyers)
//			{
//				const auto &entity = entity_ref.get();
//				// Extract the position using __worldX and __worldY attributes
//				auto pos = entity.getPosition();
//
//				int id = entity.getField<int>("Enemy_Patrol_Box_id").value();
//				if (enemyFlyerData.find(id) != enemyFlyerData.end())
//				{
//					std::get<1>(enemyFlyerData[id])
//						.push_back(vec2{pos.x - 32.f, pos.y + 32.f}); // align with background
//				}
//				else
//				{
//					enemyFlyerData[id] = std::make_tuple(std::make_tuple(vec2{}, vec2{}),
//														 std::vector<vec2>{vec2{pos.x - 32.f, pos.y + 32.f}});
//				}
//			}
//		}
//		if (layer.getName() == "Entities" && layer.hasEntity("Enemy_Patrol_Box"))
//		{
//			for (const auto &entity_ref : layer.getEntitiesByName("Enemy_Patrol_Box"))
//			{
//				const auto &entity = entity_ref.get();
//				// Extract the position using __worldX and __worldY attributes
//				auto pos = entity.getPosition();
//				auto width = entity.getSize().x; // get size
//				auto height = entity.getSize().y;
//
//				// -32px to align with background
//				vec2 topLeft{pos.x - 32.f, pos.y - 32.f};
//				vec2 bottomRight{pos.x + width - 32.f, pos.y + height - 32.f};
//
//				int id = entity.getField<int>("Enemy_Patrol_Box_id").value();
//				if (enemyFlyerData.find(id) != enemyFlyerData.end())
//				{
//					std::get<0>(enemyFlyerData[id]) = std::make_tuple(topLeft, bottomRight);
//				}
//				else
//				{
//					enemyFlyerData[id] = std::make_tuple(std::make_tuple(topLeft, bottomRight), std::vector<vec2>());
//				}
//			}
//		}
//	}
//	return enemyFlyerData;
//}
//
//////////////////////// Load LDtk level data///////////////////////////
//
//// Reset the world state to its initial state, useful for debugging
//void WorldSystem::restart_game(int level_index)
//{
//	// Debugging for memory/component leaks
//	registry.list_all_components();
//	printf("Restarting\n");
//
//	// Remove all entities that we created
//	// All that have a motion, we could also iterate over all fish, eels, ... but that would be more cumbersome
//	while (registry.motions.entities.size() > 0)
//		registry.remove_all_components_of(registry.motions.entities.back());
//
//	// Debugging for memory/component leaks
//	registry.list_all_components();
//
//	const ldtk::Level *level1 = loadLDtkFile(level_1);
//	// float
//	level_width = static_cast<float>(level1->size.x);
//	// float
//	level_height = static_cast<float>(level1->size.y);
//	vec2 playerPosition = getPlayerPosition(level1);
//
//	// Create background with calculated scaling factor
//	createBackground(renderer, {level_width / 2 - 32, level_height / 2 - 32}, {level_width, level_height});
//
//	// create a new Player
//	player = createPlayer(renderer, playerPosition);
//
//	// crate a new Crosshair
//	if (registry.crosshairs.size() == 0)
//		crosshair = createCrosshair();
//
//
//	create_world(&level1->getLayer("Wall"));
//
//	// create weapon
//	weapon_system.set_equipped_weapon(createWeapon(renderer, {100, 100}, 200.0, 250.0, 400.0, 9, 2500.0));
//	equipped_weapon = weapon_system.get_equipped_weapon();
//
//	// create enemies - location from LDtk
//	// Retrieve all enemy flyer positions nad patrol boxes
//	std::unordered_map<int, std::tuple<std::tuple<vec2, vec2>, std::vector<vec2>>> enemyPositions =
//		getEnemyFlyerData(level1);
//
//	// Spawn each enemy at its respective position
//	for (const auto &enemyPosition : enemyPositions)
//	{
//		for (auto &pos : std::get<1>(enemyPosition.second))
//		{
//			auto enemy = createEnemyFlyer(renderer, pos, std::get<0>(std::get<0>(enemyPosition.second)),
//										  std::get<1>(std::get<0>(enemyPosition.second)));
//		}
//		// std::cout << "Spawned Enemy_Flyer at position (" << enemyPosition.x << ", " << enemyPosition.y << ")" <<
//		// std::endl;
//	}
//}
//
//// TODO: might want to move this to a dedicated level_system file
//
//// void WorldSystem::create_world() {
//// 	// floor
//// 	createObstacle(renderer, {window_width_px / 2, window_height_px + 50}, ObstacleType::PLATFORM );
//// 	// ceiling
//// 	createObstacle(renderer, {window_width_px / 2, -125}, ObstacleType::PLATFORM);
//// 	// right wall
//// 	createObstacle(renderer, {window_width_px + 120, window_height_px - 150}, ObstacleType::WALL);
//// 	// left wall
//// 	createObstacle(renderer, {-130, window_height_px - 150}, ObstacleType::WALL);
//
//// Create Layer
//
//void WorldSystem::create_world(const ldtk::Layer *layer)
//{
//	float cell_size = layer->getCellSize();
//
//	vec2 cell_scale = {cell_size, cell_size};
//	std::cout << cell_size << std::endl;
//
//	for (const ldtk::Tile &tile : layer->allTiles())
//	{
//		vec2 pos = vec2(tile.getPosition().x, tile.getPosition().y);
//		// std::cout << pos.x << " " << pos.y << std::endl;
//
//		createObstacle(renderer, pos, ObstacleType::PLATFORM, cell_scale);
//	}
//
//	//// floor
//	// createFloor(renderer, {window_width_px / 2, window_height_px + 50});
//	//// ceiling
//	// createCeiling(renderer, {window_width_px / 2, -125});
//	//// right wall
//	// registry.rightWalls.emplace(createWall(renderer, {window_width_px + 120, window_height_px - 150}));
//	//// left wall
//	// registry.leftWalls.emplace(createWall(renderer, {-130, window_height_px - 150}));
//}
