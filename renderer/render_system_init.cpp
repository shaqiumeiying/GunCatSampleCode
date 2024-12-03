// internal
#include "renderer/render_system.hpp"

#include <array>
#include <fstream>

#include "../ext/stb_image/stb_image.h"

// This creates circular header inclusion, that is quite bad.
#include "engine/tiny_ecs_registry.hpp"
#include "animation/animation_system.hpp"

// stlib
#include <iostream>
#include <sstream>
#include <freetype/freetype.h>
#include <glm/gtc/type_ptr.hpp>

// World initialization
bool RenderSystem::init(GLFWwindow* window_arg)
{
	this->window = window_arg;

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // vsync

	// Load OpenGL function pointers
	const int is_fine = gl3w_init();
	assert(is_fine == 0);

	initializeCrosshair();

	// Create a frame buffer
	frame_buffer = 0;
	glGenFramebuffers(1, &frame_buffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
	gl_has_errors();

	// For some high DPI displays (ex. Retina Display on Macbooks)
	// https://stackoverflow.com/questions/36672935/why-retina-screen-coordinate-value-is-twice-the-value-of-pixel-value
	int frame_buffer_width_px, frame_buffer_height_px;
	glfwGetFramebufferSize(window, &frame_buffer_width_px, &frame_buffer_height_px);  // Note, this will be 2x the resolution given to glfwCreateWindow on retina displays
	if (frame_buffer_width_px != window_width_px)
	{
		printf("WARNING: retina display! https://stackoverflow.com/questions/36672935/why-retina-screen-coordinate-value-is-twice-the-value-of-pixel-value\n");
		printf("glfwGetFramebufferSize = %d,%d\n", frame_buffer_width_px, frame_buffer_height_px);
		printf("window width_height = %d,%d\n", window_width_px, window_height_px);
	}

	// Hint: Ask your TA for how to setup pretty OpenGL error callbacks. 
	// This can not be done in macOS, so do not enable
	// it unless you are on Linux or Windows. You will need to change the window creation
	// code to use OpenGL 4.3 (not suported in macOS) and add additional .h and .cpp
	// glDebugMessageCallback((GLDEBUGPROC)errorCallback, nullptr);

	// We are not really using VAO's but without at least one bound we will crash in
	// some systems.
	//GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	gl_has_errors();

	initScreenTexture();
    initializeGlTextures();
	initializeGlEffects();
	initializeGlGeometryBuffers();

	return true;
}

void RenderSystem::initializeCrosshair()
{
	int cursor_width, cursor_height, cursor_channels;

	unsigned char *cursor_data = stbi_load(textures_path("crosshair.png").c_str(), &cursor_width, &cursor_height, &cursor_channels, 4);

	GLFWimage cursor_image;
	cursor_image.width = cursor_width;
	cursor_image.height = cursor_height;
	cursor_image.pixels = cursor_data;

	int hotspotX = cursor_width/2;
	int hotspotY = cursor_height/2;

	GLFWcursor *cursor = glfwCreateCursor(&cursor_image, hotspotX, hotspotY);
	glfwSetCursor(window, cursor);

	stbi_image_free(cursor_data);
}


void RenderSystem::initializeGlTextures()
{
	///////////////////////////////////BACKGROUNDS///////////////////////////////////////
	texture_paths[(int)TEXTURE_ASSET_ID::LEVEL0] = textures_path("level_0.png");
	texture_paths[(int)TEXTURE_ASSET_ID::LEVEL1] = textures_path("level_1.png");
	texture_paths[(int)TEXTURE_ASSET_ID::LEVEL2] = textures_path("level_2.png");
	texture_paths[(int)TEXTURE_ASSET_ID::LEVEL3] = textures_path("level_3.png");
	///////////////////////////////////GAMEPLAY///////////////////////////////////////
	texture_paths[(int)TEXTURE_ASSET_ID::CAT_IDLE] = textures_path("gun_cat_idle.png");
	texture_paths[(int)TEXTURE_ASSET_ID::CAT_WALK] = textures_path("gun_cat_walk.png");
	texture_paths[(int)TEXTURE_ASSET_ID::CAT_JUMP] = textures_path("gun_cat_jump.png");
	texture_paths[(int)TEXTURE_ASSET_ID::CAT_SPIN] = textures_path("gun_cat_spin.png");
	texture_paths[(int)TEXTURE_ASSET_ID::CAT_DEATH] = textures_path("gun_cat_death.png");
	texture_paths[(int)TEXTURE_ASSET_ID::FLOOR] = textures_path("temp_block.png");
	texture_paths[(int)TEXTURE_ASSET_ID::WALL] = textures_path("wall.png");
	texture_paths[(int)TEXTURE_ASSET_ID::ENEMY_BOSS_IDLE] = textures_path("enemy_boss_idle_anim.png");
	texture_paths[(int)TEXTURE_ASSET_ID::ENEMY_BOSS_JUMP] = textures_path("enemy_boss_jump.png");
	texture_paths[(int)TEXTURE_ASSET_ID::ENEMY_BOSS_DEATH] = textures_path("enemy_boss_death.png");
	texture_paths[(int)TEXTURE_ASSET_ID::ENEMY_FLYER] = textures_path("enemy_flyer_anim.png");
	texture_paths[(int)TEXTURE_ASSET_ID::ENEMY_FLYER_DEATH] = textures_path("enemy_flyer_death.png");
	texture_paths[(int)TEXTURE_ASSET_ID::ENEMY_CHARGER] = textures_path("enemy_charger_walk.png");
	texture_paths[(int)TEXTURE_ASSET_ID::ENEMY_CHARGER_ATTACK] = textures_path("enemy_charger_attack.png");
	texture_paths[(int)TEXTURE_ASSET_ID::ENEMY_CHARGER_DEATH] = textures_path("enemy_charger_death.png");
	texture_paths[(int)TEXTURE_ASSET_ID::ENEMY_BULLET] = textures_path("enemy_bullet.png");
	texture_paths[(int)TEXTURE_ASSET_ID::ENEMY_HEALTH_BAR_INNER] = textures_path("enemy_health_inner.png");
	texture_paths[(int)TEXTURE_ASSET_ID::ENEMY_HEALTH_BAR_OUTER] = textures_path("enemy_health_outer.png");
	texture_paths[(int)TEXTURE_ASSET_ID::RIFLE_IDLE] = textures_path("weapon.png");
	texture_paths[(int)TEXTURE_ASSET_ID::RIFLE_ATTACK] = textures_path("weapon_fire.png");
	texture_paths[(int)TEXTURE_ASSET_ID::BULLET_LONG] = textures_path("player_ammo_bullet.png");
	texture_paths[(int)TEXTURE_ASSET_ID::BULLET_SHORT] = textures_path("player_ammo_bullet_2.png");
	texture_paths[(int)TEXTURE_ASSET_ID::BULLET_ROUND] = textures_path("player_ammo_bullet_3.png");
	texture_paths[(int)TEXTURE_ASSET_ID::LAB_TILESET] = textures_path("tilesLab.png");
	texture_paths[(int)TEXTURE_ASSET_ID::GRENADE_LAUNCHER_IDLE] = textures_path("grenade_launcher.png");
	texture_paths[(int)TEXTURE_ASSET_ID::GRENADE_LAUNCHER_ATTACK] = textures_path("grenade_launcher_fire.png");
	texture_paths[(int)TEXTURE_ASSET_ID::GRENADE] = textures_path("grenade.png");
	texture_paths[(int)TEXTURE_ASSET_ID::GRENADE_EXPLODE] = textures_path("grenade_explode.png");
	texture_paths[(int)TEXTURE_ASSET_ID::DEAD] = textures_path("dead_text.png");

	texture_paths[(int)TEXTURE_ASSET_ID::CAT_IDLE_XMAS] = textures_path("gun_cat_idle_xmas.png");
	texture_paths[(int)TEXTURE_ASSET_ID::CAT_WALK_XMAS] = textures_path("gun_cat_walk_xmas.png");
	texture_paths[(int)TEXTURE_ASSET_ID::CAT_JUMP_XMAS] = textures_path("gun_cat_jump_xmas.png");
	texture_paths[(int)TEXTURE_ASSET_ID::CAT_SPIN_XMAS] = textures_path("gun_cat_spin_xmas.png");
	texture_paths[(int)TEXTURE_ASSET_ID::CAT_DEATH_XMAS] = textures_path("gun_cat_xmas_death.png");

	texture_paths[(int)TEXTURE_ASSET_ID::CAT_IDLE_SLIME] = textures_path("gun_cat_idle_slime.png");
	texture_paths[(int)TEXTURE_ASSET_ID::CAT_WALK_SLIME] = textures_path("gun_cat_walk_slime.png");
	texture_paths[(int)TEXTURE_ASSET_ID::CAT_JUMP_SLIME] = textures_path("gun_cat_jump_slime.png");
	texture_paths[(int)TEXTURE_ASSET_ID::CAT_SPIN_SLIME] = textures_path("gun_cat_spin_slime.png");
	texture_paths[(int)TEXTURE_ASSET_ID::CAT_DEATH_SLIME] = textures_path("gun_cat_slime_death.png");

	texture_paths[(int)TEXTURE_ASSET_ID::CAT_IDLE_SCH_ALIVE] = textures_path("gun_cat_idle_sch_alive.png");
	texture_paths[(int)TEXTURE_ASSET_ID::CAT_WALK_SCH_ALIVE] = textures_path("gun_cat_walk_sch_alive.png");
	texture_paths[(int)TEXTURE_ASSET_ID::CAT_JUMP_SCH_ALIVE] = textures_path("gun_cat_jump_sch_alive.png");
	texture_paths[(int)TEXTURE_ASSET_ID::CAT_SPIN_SCH_ALIVE] = textures_path("gun_cat_spin_sch_alive.png");
	texture_paths[(int)TEXTURE_ASSET_ID::CAT_IDLE_SCH_DEAD] = textures_path("gun_cat_idle_sch_dead.png");
	texture_paths[(int)TEXTURE_ASSET_ID::CAT_WALK_SCH_DEAD] = textures_path("gun_cat_walk_sch_dead.png");
	texture_paths[(int)TEXTURE_ASSET_ID::CAT_JUMP_SCH_DEAD] = textures_path("gun_cat_jump_sch_dead.png");
	texture_paths[(int)TEXTURE_ASSET_ID::CAT_SPIN_SCH_DEAD] = textures_path("gun_cat_spin_sch_dead.png");
	texture_paths[(int)TEXTURE_ASSET_ID::CAT_DEATH_SCH] = textures_path("gun_cat_sch_dead_for_real.png");

	texture_paths[(int)TEXTURE_ASSET_ID::CAT_IDLE_RAINBOW] = textures_path("gun_cat_idle_rainbow.png");
	texture_paths[(int)TEXTURE_ASSET_ID::CAT_WALK_RAINBOW] = textures_path("gun_cat_walk_rainbow.png");
	texture_paths[(int)TEXTURE_ASSET_ID::CAT_JUMP_RAINBOW] = textures_path("gun_cat_jump_rainbow.png");
	texture_paths[(int)TEXTURE_ASSET_ID::CAT_SPIN_RAINBOW] = textures_path("gun_cat_spin_rainbow.png");
	texture_paths[(int)TEXTURE_ASSET_ID::CAT_DEATH_RAINBOW] = textures_path("gun_cat_rainbow_death.png");


	////////////////////////////////UI///////////////////////////////////////
	texture_paths[(int)TEXTURE_ASSET_ID::MAIN_MENU] = textures_path("main_menu_screen.png");
	texture_paths[(int)TEXTURE_ASSET_ID::PAUSE_MENU] = textures_path("paused_screen.png");
	texture_paths[(int)TEXTURE_ASSET_ID::LEVEL_MENU] = textures_path("level_select_screen.png");
	texture_paths[(int)TEXTURE_ASSET_ID::SUMMARY_MENU] = textures_path("summary_menu.png");
	texture_paths[(int)TEXTURE_ASSET_ID::BOX_MENU] = textures_path("box_menu_screen.png");
	texture_paths[(int)TEXTURE_ASSET_ID::LVL_1_BUTTON] = textures_path("lvl_1_button.png");
	texture_paths[(int)TEXTURE_ASSET_ID::LVL_2_BUTTON] = textures_path("lvl_2_button.png");
	texture_paths[(int)TEXTURE_ASSET_ID::LVL_3_BUTTON] = textures_path("lvl_3_button.png");
	texture_paths[(int)TEXTURE_ASSET_ID::START_BUTTON] = textures_path("start_button.png");
	texture_paths[(int)TEXTURE_ASSET_ID::MENU_BUTTON] = textures_path("menu_button.png");
	texture_paths[(int)TEXTURE_ASSET_ID::OPTIONS_BUTTON] = textures_path("options_button.png");
	texture_paths[(int)TEXTURE_ASSET_ID::RESUME_BUTTON] = textures_path("resume_button.png");
	texture_paths[(int)TEXTURE_ASSET_ID::BACK_BUTTON] = textures_path("back_button.png");
	texture_paths[(int)TEXTURE_ASSET_ID::QUIT_BUTTON] = textures_path("quit_button.png");
	texture_paths[(int)TEXTURE_ASSET_ID::RESTART_BUTTON] = textures_path("restart_button.png");
	texture_paths[(int)TEXTURE_ASSET_ID::START_BUTTON_CLICKED] = textures_path("start_button_clicked.png");
	texture_paths[(int)TEXTURE_ASSET_ID::MENU_BUTTON_CLICKED] = textures_path("menu_button_clicked.png");
	texture_paths[(int)TEXTURE_ASSET_ID::OPTIONS_BUTTON_CLICKED] = textures_path("options_button_clicked.png");
	texture_paths[(int)TEXTURE_ASSET_ID::RESUME_BUTTON_CLICKED] = textures_path("resume_button_clicked.png");
	texture_paths[(int)TEXTURE_ASSET_ID::BACK_BUTTON_CLICKED] = textures_path("back_button_clicked.png");
	texture_paths[(int)TEXTURE_ASSET_ID::QUIT_BUTTON_CLICKED] = textures_path("quit_button_clicked.png");
	texture_paths[(int)TEXTURE_ASSET_ID::OUTFIT_BUTTON] = textures_path("outfit_button.png");
	texture_paths[(int)TEXTURE_ASSET_ID::OUTFIT_BUTTON_CLICKED] = textures_path("outfit_button_clicked.png");
	texture_paths[(int)TEXTURE_ASSET_ID::RESTART_BUTTON_CLICKED] = textures_path("restart_button_clicked.png");
	texture_paths[(int)TEXTURE_ASSET_ID::LORE_BUTTON] = textures_path("lore_button.png");
	texture_paths[(int)TEXTURE_ASSET_ID::LORE_BUTTON_CLICKED] = textures_path("lore_button_clicked.png");
	texture_paths[(int)TEXTURE_ASSET_ID::LVL_1_BUTTON_CLICKED] = textures_path("lvl_1_button_clicked.png");
	texture_paths[(int)TEXTURE_ASSET_ID::LVL_2_BUTTON_CLICKED] = textures_path("lvl_2_button_clicked.png");
	texture_paths[(int)TEXTURE_ASSET_ID::LVL_3_BUTTON_CLICKED] = textures_path("lvl_3_button_clicked.png");
	texture_paths[(int)TEXTURE_ASSET_ID::LVL_2_BUTTON_CLICKED_LOCKED] = textures_path("lvl_2_button_clicked_locked.png");
	texture_paths[(int)TEXTURE_ASSET_ID::LVL_3_BUTTON_CLICKED_LOCKED] = textures_path("lvl_3_button_clicked_locked.png");
	texture_paths[(int)TEXTURE_ASSET_ID::CAT_SKIN] = textures_path("cat_default.png");
	texture_paths[(int)TEXTURE_ASSET_ID::CAT_SKIN_SELECTED] = textures_path("cat_default_selected.png");
	texture_paths[(int)TEXTURE_ASSET_ID::CAT_SKIN_XMAS] = textures_path("cat_xmas.png");
	texture_paths[(int)TEXTURE_ASSET_ID::CAT_SKIN_XMAS_LOCKED] = textures_path("cat_xmas_locked.png");
	texture_paths[(int)TEXTURE_ASSET_ID::CAT_SKIN_XMAS_SELECTED] = textures_path("cat_xmas_selected.png");
	texture_paths[(int)TEXTURE_ASSET_ID::CAT_SKIN_SLIME] = textures_path("cat_slime.png");
	texture_paths[(int)TEXTURE_ASSET_ID::CAT_SKIN_SLIME_LOCKED] = textures_path("cat_slime_locked.png");
	texture_paths[(int)TEXTURE_ASSET_ID::CAT_SKIN_SLIME_SELECTED] = textures_path("cat_slime_selected.png");
	texture_paths[(int)TEXTURE_ASSET_ID::CAT_SKIN_SCH] = textures_path("cat_sch.png");
	texture_paths[(int)TEXTURE_ASSET_ID::CAT_SKIN_SCH_LOCKED] = textures_path("cat_sch_locked.png");
	texture_paths[(int)TEXTURE_ASSET_ID::CAT_SKIN_SCH_SELECTED] = textures_path("cat_sch_selected.png");
	texture_paths[(int)TEXTURE_ASSET_ID::CAT_SKIN_RAINBOW] = textures_path("cat_rainbow.png");
	texture_paths[(int)TEXTURE_ASSET_ID::CAT_SKIN_RAINBOW_LOCKED] = textures_path("cat_rainbow_locked.png");
	texture_paths[(int)TEXTURE_ASSET_ID::CAT_SKIN_RAINBOW_SELECTED] = textures_path("cat_rainbow_selected.png");
	texture_paths[(int)TEXTURE_ASSET_ID::NOTE_1] = textures_path("note_1.png");
	texture_paths[(int)TEXTURE_ASSET_ID::NOTE_2] = textures_path("note_2.png");
	texture_paths[(int)TEXTURE_ASSET_ID::NOTE_3] = textures_path("note_3.png");
	texture_paths[(int)TEXTURE_ASSET_ID::NOTE_1_ICON] = textures_path("note_icon.png");
	texture_paths[(int)TEXTURE_ASSET_ID::NOTE_2_ICON] = textures_path("note_icon.png");
	texture_paths[(int)TEXTURE_ASSET_ID::NOTE_3_ICON] = textures_path("note_icon.png");
	texture_paths[(int)TEXTURE_ASSET_ID::NOTE_1_ICON_SELECTED] = textures_path("note_1_icon_selected.png");
	texture_paths[(int)TEXTURE_ASSET_ID::NOTE_2_ICON_SELECTED] = textures_path("note_2_icon_selected.png");
	texture_paths[(int)TEXTURE_ASSET_ID::NOTE_3_ICON_SELECTED] = textures_path("note_3_icon_selected.png");
	texture_paths[(int)TEXTURE_ASSET_ID::NOTE_PICKUP] = textures_path("note_icon_no_outline.png");
	texture_paths[(int)TEXTURE_ASSET_ID::CLOSE] = textures_path("close_button.png");
	texture_paths[(int)TEXTURE_ASSET_ID::CLOSE_SELECTED] = textures_path("close_button_selected.png");

	texture_paths[(int)TEXTURE_ASSET_ID::CAT_DEFAULT_TEXT] = textures_path("cat_default_text.png");
	texture_paths[(int)TEXTURE_ASSET_ID::CAT_XMAS_TEXT] = textures_path("cat_xmas_text.png");
	texture_paths[(int)TEXTURE_ASSET_ID::CAT_SLIME_TEXT] = textures_path("cat_slime_text.png");
	texture_paths[(int)TEXTURE_ASSET_ID::CAT_SCH_TEXT] = textures_path("cat_sch_text.png");
	texture_paths[(int)TEXTURE_ASSET_ID::CAT_RAINBOW_TEXT] = textures_path("cat_rainbow_text.png");
	///////////////////////////////////HUD///////////////////////////////////////
	texture_paths[(int)TEXTURE_ASSET_ID::HUD_CONTAINER] = textures_path("hud_container.png");
	texture_paths[(int)TEXTURE_ASSET_ID::PLAYER_HEALTH] = textures_path("player_health_ui.png");
	texture_paths[(int)TEXTURE_ASSET_ID::RELOADED_TEXT] = textures_path("reloaded_text.png");
	texture_paths[(int)TEXTURE_ASSET_ID::BOSS_HEALTH_BAR] = textures_path("boss_health_bar_inner.png");
	texture_paths[(int)TEXTURE_ASSET_ID::BOSS_HEALTH_BACKGROUND] = textures_path("boss_health_bar.png");
	texture_paths[(int)TEXTURE_ASSET_ID::END_GAME_TRIGGER] = textures_path("end_door.png");
	texture_paths[(int)TEXTURE_ASSET_ID::CREDIT_LIST] = textures_path("credit_list.png");



	glGenTextures((GLsizei)texture_gl_handles.size(), texture_gl_handles.data());

    for(uint i = 0; i < texture_paths.size(); i++)
    {
		const std::string& path = texture_paths[i];
		ivec2& dimensions = texture_dimensions[i];

		stbi_uc* data;
		data = stbi_load(path.c_str(), &dimensions.x, &dimensions.y, NULL, 4);

		if (data == NULL)
		{
			const std::string message = "Could not load the file " + path + ".";
			fprintf(stderr, "%s", message.c_str());
			assert(false);
		}
		glBindTexture(GL_TEXTURE_2D, texture_gl_handles[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, dimensions.x, dimensions.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		gl_has_errors();
		stbi_image_free(data);
    }
	gl_has_errors();
}

void RenderSystem::initializeGlEffects()
{
	for(uint i = 0; i < effect_paths.size(); i++)
	{
		const std::string vertex_shader_name = effect_paths[i] + ".vs.glsl";
		const std::string fragment_shader_name = effect_paths[i] + ".fs.glsl";

		bool is_valid = loadEffectFromFile(vertex_shader_name, fragment_shader_name, effects[i]);
		assert(is_valid && (GLuint)effects[i] != 0);
	}
}

// One could merge the following two functions as a template function...
template <class T>
void RenderSystem::bindVBOandIBO(GEOMETRY_BUFFER_ID gid, std::vector<T> vertices, std::vector<uint16_t> indices)
{
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffers[(uint)gid]);
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(vertices[0]) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
	gl_has_errors();

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffers[(uint)gid]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		sizeof(indices[0]) * indices.size(), indices.data(), GL_STATIC_DRAW);
	gl_has_errors();
}

void RenderSystem::initializeGlMeshes()
{
	for (uint i = 0; i < mesh_paths.size(); i++)
	{
		// Initialize meshes
		GEOMETRY_BUFFER_ID geom_index = mesh_paths[i].first;
		std::string name = mesh_paths[i].second;
		Mesh::loadFromOBJFile(name, 
			meshes[(int)geom_index].vertices,
			meshes[(int)geom_index].vertex_indices,
			meshes[(int)geom_index].original_size);

		bindVBOandIBO(geom_index,
			meshes[(int)geom_index].vertices, 
			meshes[(int)geom_index].vertex_indices);
	}
}

void RenderSystem::initializeGlGeometryBuffers()
{
	// Vertex Buffer creation.
	glGenBuffers((GLsizei)vertex_buffers.size(), vertex_buffers.data());
	// Index Buffer creation.
	glGenBuffers((GLsizei)index_buffers.size(), index_buffers.data());

	// Index and Vertex buffer data initialization.
	initializeGlMeshes();
	// Initialize animations
	initializeAnimations(*this);

	//////////////////////////
	// Initialize sprite
	// The position corresponds to the center of the texture.
	std::vector<TexturedVertex> textured_vertices(4);
	textured_vertices[0].position = { -1.f/2, +1.f/2, 0.f };
	textured_vertices[1].position = { +1.f/2, +1.f/2, 0.f };
	textured_vertices[2].position = { +1.f/2, -1.f/2, 0.f };
	textured_vertices[3].position = { -1.f/2, -1.f/2, 0.f };
	textured_vertices[0].texcoord = { 0.f, 1.f };
	textured_vertices[1].texcoord = { 1.f, 1.f };
	textured_vertices[2].texcoord = { 1.f, 0.f };
	textured_vertices[3].texcoord = { 0.f, 0.f };

	// Counterclockwise as it's the default opengl front winding direction.
	const std::vector<uint16_t> textured_indices = { 0, 3, 1, 1, 3, 2 };
	bindVBOandIBO(GEOMETRY_BUFFER_ID::SPRITE, textured_vertices, textured_indices);

	//////////////////////////
	// Initialize LabTiles
	// The position corresponds to the center of the texture.
	std::vector<TexturedVertex> labTile_vertices(4);
	labTile_vertices[0].position = {-1.f / 2, +1.f / 2, 0.f};
	labTile_vertices[1].position = {+1.f / 2, +1.f / 2, 0.f};
	labTile_vertices[2].position = {+1.f / 2, -1.f / 2, 0.f};
	labTile_vertices[3].position = {-1.f / 2, -1.f / 2, 0.f};
	labTile_vertices[0].texcoord = {0.f, 1.f/3}; 
	labTile_vertices[1].texcoord = {1.f / 8, 1.f/3}; 
	labTile_vertices[2].texcoord = {1.f / 8, 0}; 
	labTile_vertices[3].texcoord = {0.f, 0.f};
	bindVBOandIBO(GEOMETRY_BUFFER_ID::LAB_TILESET, labTile_vertices, textured_indices);

	//////////////////////////////////
	// Initialize debug line
	std::vector<ColoredVertex> line_vertices;
	std::vector<uint16_t> line_indices;

	constexpr float depth = 0.5f;
	constexpr vec3 red = { 0.8,0.1,0.1 };

	// Corner points
	line_vertices = {
		{{-0.5,-0.5, depth}, red},
		{{-0.5, 0.5, depth}, red},
		{{ 0.5, 0.5, depth}, red},
		{{ 0.5,-0.5, depth}, red},
	};

	// Two triangles
	line_indices = {0, 1, 3, 1, 2, 3};
	
	int geom_index = (int)GEOMETRY_BUFFER_ID::DEBUG_LINE;
	meshes[geom_index].vertices = line_vertices;
	meshes[geom_index].vertex_indices = line_indices;
	bindVBOandIBO(GEOMETRY_BUFFER_ID::DEBUG_LINE, line_vertices, line_indices);

	//////////////////////////////////
	// Initialize particle
	std::vector<ColoredVertex> particle_vertices;
	std::vector<uint16_t> particle_indices;

	constexpr vec3 white = {1.0, 1.0, 1.0};

	// Corner points
	particle_vertices = {
		{{-0.5, -0.5, 0.1f}, white},
		{{-0.5, 0.5, 0.1f}, white},
		{{0.5, 0.5, 0.1f}, white},
		{{0.5, -0.5, 0.1f}, white},
	};

	// Two triangles
	particle_indices = {0, 1, 3, 1, 2, 3};

	int geom_index_p = (int)GEOMETRY_BUFFER_ID::PARTICLE;
	meshes[geom_index_p].vertices = particle_vertices;
	meshes[geom_index_p].vertex_indices = particle_indices;
	bindVBOandIBO(GEOMETRY_BUFFER_ID::PARTICLE, particle_vertices, particle_indices);

	///////////////////////////////////////////////////////
	// Initialize screen triangle (yes, triangle, not quad; its more efficient).
	std::vector<vec3> screen_vertices(3);
	screen_vertices[0] = { -1, -6, 0.f };
	screen_vertices[1] = { 6, -1, 0.f };
	screen_vertices[2] = { -1, 6, 0.f };

	// Counterclockwise as it's the default opengl front winding direction.
	const std::vector<uint16_t> screen_indices = { 0, 1, 2 };
	bindVBOandIBO(GEOMETRY_BUFFER_ID::SCREEN_TRIANGLE, screen_vertices, screen_indices);
}

RenderSystem::~RenderSystem()
{
	// Don't need to free gl resources since they last for as long as the program,
	// but it's polite to clean after yourself.
	glDeleteBuffers((GLsizei)vertex_buffers.size(), vertex_buffers.data());
	glDeleteBuffers((GLsizei)index_buffers.size(), index_buffers.data());
	glDeleteTextures((GLsizei)texture_gl_handles.size(), texture_gl_handles.data());
	glDeleteTextures(1, &off_screen_render_buffer_color);
	glDeleteRenderbuffers(1, &off_screen_render_buffer_depth);
	gl_has_errors();

	for(uint i = 0; i < effect_count; i++) {
		glDeleteProgram(effects[i]);
	}
	// delete allocated resources
	glDeleteFramebuffers(1, &frame_buffer);
	gl_has_errors();

	// remove all entities created by the render system
	while (registry.renderRequests.entities.size() > 0)
	    registry.remove_all_components_of(registry.renderRequests.entities.back());
}

// Initialize the screen texture from a standard sprite
bool RenderSystem::initScreenTexture()
{
	registry.screenStates.emplace(screen_state_entity);

	int framebuffer_width, framebuffer_height;
	glfwGetFramebufferSize(const_cast<GLFWwindow*>(window), &framebuffer_width, &framebuffer_height);  // Note, this will be 2x the resolution given to glfwCreateWindow on retina displays

	glGenTextures(1, &off_screen_render_buffer_color);
	glBindTexture(GL_TEXTURE_2D, off_screen_render_buffer_color);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, framebuffer_width, framebuffer_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	gl_has_errors();

	glGenRenderbuffers(1, &off_screen_render_buffer_depth);
	glBindRenderbuffer(GL_RENDERBUFFER, off_screen_render_buffer_depth);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, off_screen_render_buffer_color, 0);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, framebuffer_width, framebuffer_height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, off_screen_render_buffer_depth);
	gl_has_errors();

	assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

	return true;
}

std::string readShaderFile(const std::string &filename)
{
	std::cout << "Loading shader filename: " << filename << std::endl;

	std::ifstream ifs(filename);

	if (!ifs.good())
	{
		std::cerr << "ERROR: invalid filename loading shader from file: " << filename << std::endl;
		return "";
	}

	std::ostringstream oss;
	oss << ifs.rdbuf();
	std::cout << oss.str() << std::endl;
	return oss.str();
}

// credits to simpleGL lecture 3
bool RenderSystem::fontInit()
{

	std::string font_filename = PROJECT_SOURCE_DIR + std::string("data/fonts/Kenney_Pixel_Square.ttf");
	unsigned int font_default_size = 48;

	// read in our shader files
	std::string vertexShaderSource = readShaderFile(PROJECT_SOURCE_DIR + std::string("shaders/font.vs.glsl"));
	std::string fragmentShaderSource = readShaderFile(PROJECT_SOURCE_DIR + std::string("shaders/font.fs.glsl"));
	const char *vertexShaderSource_c = vertexShaderSource.c_str();
	const char *fragmentShaderSource_c = fragmentShaderSource.c_str();


	// enable blending or you will just get solid boxes instead of text
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// font buffer setup
	glGenVertexArrays(1, &m_font_VAO);
	glGenBuffers(1, &m_font_VBO);

	// font vertex shader
	unsigned int font_vertexShader;
	font_vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(font_vertexShader, 1, &vertexShaderSource_c, NULL);
	glCompileShader(font_vertexShader);

	// font fragement shader
	unsigned int font_fragmentShader;
	font_fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(font_fragmentShader, 1, &fragmentShaderSource_c, NULL);
	glCompileShader(font_fragmentShader);

	// font shader program
	m_font_shaderProgram = glCreateProgram();
	glAttachShader(m_font_shaderProgram, font_vertexShader);
	glAttachShader(m_font_shaderProgram, font_fragmentShader);
	glLinkProgram(m_font_shaderProgram);

	glUseProgram(m_font_shaderProgram);

	// apply orthographic projection matrix for font, i.e., screen space
	glm::mat4 projection =
		glm::ortho(0.0f, static_cast<float>(window_width_px), 0.0f, static_cast<float>(window_height_px));

	GLint project_location = glGetUniformLocation(m_font_shaderProgram, "projection");
	assert(project_location > -1);
	std::cout << "project_location: " << project_location << std::endl;
	glUniformMatrix4fv(project_location, 1, GL_FALSE, glm::value_ptr(projection));


	// clean up shaders
	glDeleteShader(font_vertexShader);
	glDeleteShader(font_fragmentShader);

	// init FreeType fonts
	FT_Library ft;
	if (FT_Init_FreeType(&ft))
	{
		std::cerr << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
		return false;
	}

	FT_Face face;
	if (FT_New_Face(ft, font_filename.c_str(), 0, &face))
	{
		std::cerr << "ERROR::FREETYPE: Failed to load font: " << font_filename << std::endl;
		return false;
	}

	// extract a default size
	FT_Set_Pixel_Sizes(face, 0, font_default_size);

	// disable byte-alignment restriction in OpenGL
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// load each of the chars - note only first 128 ASCII chars
	for (unsigned char c = (unsigned char)0; c < (unsigned char)128; c++)
	{
		// load character glyph
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			std::cerr << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			continue;
		}

		// generate texture
		unsigned int texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);

		// std::cout << "texture: " << c << " = " << texture << std::endl;

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, GL_RED,
					 GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);

		// set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// now store character for later use
		Character character = {texture, glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
							   glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
							   static_cast<unsigned int>(face->glyph->advance.x), (char)c};
		m_ftCharacters.insert(std::pair<char, Character>(c, character));
	}
	glBindTexture(GL_TEXTURE_2D, 0);

	// clean up
	FT_Done_Face(face);
	FT_Done_FreeType(ft);

	// bind buffers
	glBindVertexArray(m_font_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_font_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);

	// release buffers
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return true;
}

bool gl_compile_shader(GLuint shader)
{
	glCompileShader(shader);
	gl_has_errors();
	GLint success = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (success == GL_FALSE)
	{
		GLint log_len;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_len);
		std::vector<char> log(log_len);
		glGetShaderInfoLog(shader, log_len, &log_len, log.data());
		glDeleteShader(shader);

		gl_has_errors();

		fprintf(stderr, "GLSL: %s", log.data());
		return false;
	}

	return true;
}

bool loadEffectFromFile(
	const std::string& vs_path, const std::string& fs_path, GLuint& out_program)
{
	// Opening files
	std::ifstream vs_is(vs_path);
	std::ifstream fs_is(fs_path);
	if (!vs_is.good() || !fs_is.good())
	{
		fprintf(stderr, "Failed to load shader files %s, %s", vs_path.c_str(), fs_path.c_str());
		assert(false);
		return false;
	}

	// Reading sources
	std::stringstream vs_ss, fs_ss;
	vs_ss << vs_is.rdbuf();
	fs_ss << fs_is.rdbuf();
	std::string vs_str = vs_ss.str();
	std::string fs_str = fs_ss.str();
	const char* vs_src = vs_str.c_str();
	const char* fs_src = fs_str.c_str();
	GLsizei vs_len = (GLsizei)vs_str.size();
	GLsizei fs_len = (GLsizei)fs_str.size();

	GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vs_src, &vs_len);
	GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fs_src, &fs_len);
	gl_has_errors();

	// Compiling
	if (!gl_compile_shader(vertex))
	{
		fprintf(stderr, "Vertex compilation failed");
		assert(false);
		return false;
	}
	if (!gl_compile_shader(fragment))
	{
		fprintf(stderr, "Vertex compilation failed");
		assert(false);
		return false;
	}

	// Linking
	out_program = glCreateProgram();
	glAttachShader(out_program, vertex);
	glAttachShader(out_program, fragment);
	glLinkProgram(out_program);
	gl_has_errors();

	{
		GLint is_linked = GL_FALSE;
		glGetProgramiv(out_program, GL_LINK_STATUS, &is_linked);
		if (is_linked == GL_FALSE)
		{
			GLint log_len;
			glGetProgramiv(out_program, GL_INFO_LOG_LENGTH, &log_len);
			std::vector<char> log(log_len);
			glGetProgramInfoLog(out_program, log_len, &log_len, log.data());
			gl_has_errors();

			fprintf(stderr, "Link error: %s", log.data());
			assert(false);
			return false;
		}
	}

	// No need to carry this around. Keeping these objects is only useful if we recycle
	// the same shaders over and over, which we don't, so no need and this is simpler.
	glDetachShader(out_program, vertex);
	glDetachShader(out_program, fragment);
	glDeleteShader(vertex);
	glDeleteShader(fragment);
	gl_has_errors();

	return true;
}

