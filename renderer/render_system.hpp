#pragma once

#include <array>
#include <utility>

#include "common.hpp"
#include "engine/components.hpp"
#include "engine/tiny_ecs.hpp"
#include "menu/menu_system.hpp"
#include <map>

// System responsible for setting up OpenGL and for rendering all the
// visual entities in the game
class RenderSystem {
	/**
	 * The following arrays store the assets the game will use. They are loaded
	 * at initialization and are assumed to not be modified by the render loop.
	 *
	 * Whenever possible, add to these lists instead of creating dynamic state
	 * it is easier to debug and faster to execute for the computer.
	 */
	std::array<GLuint, texture_count> texture_gl_handles;
	std::array<ivec2, texture_count> texture_dimensions;

	// Make sure these paths remain in sync with the associated enumerators.
	// Associated id with .obj path
	const std::vector < std::pair<GEOMETRY_BUFFER_ID, std::string>> mesh_paths =
	{
		std::pair<GEOMETRY_BUFFER_ID, std::string>(GEOMETRY_BUFFER_ID::SALMON, mesh_path("cat_mesh.obj"))
		  // specify meshes of other assets here
	};

	// Make sure these paths remain in sync with the associated enumerators.
	std::array<std::string, texture_count> texture_paths;

	std::array<GLuint, effect_count> effects;
	// Make sure these paths remain in sync with the associated enumerators.
	const std::array<std::string, effect_count> effect_paths = {
		shader_path("coloured"),																
		shader_path("salmon"),
		shader_path("textured"),
		shader_path("water"),
		/*shader_path("reload")*/
		shader_path("egg")};

	std::array<GLuint, geometry_count> vertex_buffers;
	std::array<GLuint, geometry_count> index_buffers;
	std::array<Mesh, geometry_count> meshes;

public:
	// Initialize the window
	bool init(GLFWwindow* window);

	template <class T>
	void bindVBOandIBO(GEOMETRY_BUFFER_ID gid, std::vector<T> vertices, std::vector<uint16_t> indices);

	void initializeGlTextures();

	void initializeGlEffects();

	void initializeGlMeshes();
	Mesh& getMesh(GEOMETRY_BUFFER_ID id) { return meshes[(int)id]; };

	void initializeGlGeometryBuffers();
	// Initialize the screen texture used as intermediate render target
	// The draw loop first renders to this texture, then it is used for the wind
	// shader
	bool initScreenTexture();

	// Destroy resources associated to one or all entities created by the system
	~RenderSystem();

	// Draw all entities
	void draw(GAME_STATE current_state, float elapsed_ms, WorldSystem &world);
	
	mat3 createProjectionMatrix();

	//HUD and MENU
	static mat3 createOrthographicProjection(float width, float height);
	void renderMenu(GAME_STATE current_state, int w, int h);
	Entity createMenu(TEXTURE_ASSET_ID texture_id, vec2 pos, vec2 scale);
	Entity createMenu(const MenuElementAttributes &menu_attr);
	Entity createButton(TEXTURE_ASSET_ID default_texture_id, vec2 pos, vec2 scale);
	Entity createButton(const MenuElementAttributes &btn_attr);
	void initializeMenuEntities(GAME_STATE state, WorldSystem &world);
	void clearMenuEntities(GAME_STATE state);
	void renderButtons();
	void initializeCrosshair();

	friend void initializeAnimations(RenderSystem &renderSystem);

	void renderText(const mat3 &projection);

	bool fontInit();

	// Cached entities for each menu state
	std::unordered_map<GAME_STATE, std::vector<Entity>> cached_entities;

private:
	// Internal drawing functions for each entity type
	void drawTexturedMesh(Entity entity, const mat3 &projection, int &atFrame, GLfloat &frameWidth, float elapsed_ms);
	void drawToScreen();

	// Window handle
	GLFWwindow* window;

	// Screen texture handles
	GLuint frame_buffer;
	GLuint off_screen_render_buffer_color;
	GLuint off_screen_render_buffer_depth;

	Entity screen_state_entity;

	int place_holder_int = 0;
	GLfloat place_holder_float = 0;

	GLuint vao;
	GLuint vbo;

	// Fonts
	std::map<char, Character> m_ftCharacters;
	GLuint m_font_shaderProgram;
	GLuint m_font_VAO;
	GLuint m_font_VBO;
};

bool loadEffectFromFile(
	const std::string& vs_path, const std::string& fs_path, GLuint& out_program);


