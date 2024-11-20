// internal
#include "render_system.hpp"
#include "camera/camera_system.hpp"
#include <SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "engine/tiny_ecs_registry.hpp"
#include "animation/animation_system.hpp"
#include "weapons/weapon_system.hpp"
#include <glm/gtc/type_ptr.hpp>
// Note: drawTexturedMesh now takes a current frame, frame width, and elapsed time for animation purposes
void RenderSystem::drawTexturedMesh(Entity entity, const mat3 &projection, int &frameCurrent, GLfloat &frameWidth, float elapsed_ms)
{
	Motion &motion = registry.motions.get(entity);
	// Transformation code, see Rendering and Transformation in the template
	// specification for more info Incrementally updates transformation matrix,
	// thus ORDER IS IMPORTANT
	Transform transform;
	transform.translate(motion.position);
	// Weapon will definitely rotate, don't know about the cat.
	transform.rotate(motion.angle);
	transform.scale(motion.scale);

	assert(registry.renderRequests.has(entity));
	const RenderRequest &render_request = registry.renderRequests.get(entity);

	const GLuint used_effect_enum = (GLuint)render_request.used_effect;
	assert(used_effect_enum != (GLuint)EFFECT_ASSET_ID::EFFECT_COUNT);
	const GLuint program = (GLuint)effects[used_effect_enum];

	// Setting shaders
	glUseProgram(program);
	gl_has_errors();

	assert(render_request.used_geometry != GEOMETRY_BUFFER_ID::GEOMETRY_COUNT);
	const GLuint vbo = vertex_buffers[(GLuint)render_request.used_geometry];
	const GLuint ibo = index_buffers[(GLuint)render_request.used_geometry];

	// setting VAO
	glBindVertexArray(vao);

	// Setting vertex and index buffers
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	gl_has_errors();

	if (render_request.used_effect == EFFECT_ASSET_ID::EGG)
	{
		GLint in_position_loc = glGetAttribLocation(program, "in_position");
		GLint in_color_loc = glGetAttribLocation(program, "in_color");
		gl_has_errors();

		glEnableVertexAttribArray(in_position_loc);
		glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(ColoredVertex), (void *)0);
		gl_has_errors();

		glEnableVertexAttribArray(in_color_loc);
		glVertexAttribPointer(in_color_loc, 3, GL_FLOAT, GL_FALSE, sizeof(ColoredVertex), (void *)sizeof(vec3));
		gl_has_errors();
	}else if (render_request.used_effect == EFFECT_ASSET_ID::TEXTURED)
	{
		GLint in_position_loc = glGetAttribLocation(program, "in_position");
		GLint in_texcoord_loc = glGetAttribLocation(program, "in_texcoord");
		gl_has_errors();
		assert(in_texcoord_loc >= 0);

		glEnableVertexAttribArray(in_position_loc);
		glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE,
							  sizeof(TexturedVertex), (void *)0);
		gl_has_errors();

		glEnableVertexAttribArray(in_texcoord_loc);
		glVertexAttribPointer(
			in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex),
			(void *)sizeof(
				vec3)); // note the stride to skip the preceeding vertex position

		// Enabling and binding texture to slot 0
		glActiveTexture(GL_TEXTURE0);
		gl_has_errors();

		assert(registry.renderRequests.has(entity));
		GLuint texture_id =
			texture_gl_handles[(GLuint)registry.renderRequests.get(entity).used_texture];

		glBindTexture(GL_TEXTURE_2D, texture_id);
		gl_has_errors();

		///////////////////////ANIMATION//////////////////////////
		GLint frame_uloc = glGetUniformLocation(program, "atFrame");
		GLfloat frame_width_uloc = glGetUniformLocation(program, "frameWidth");
		glUniform1i(frame_uloc, frameCurrent);
		glUniform1f(frame_width_uloc, frameWidth);
		gl_has_errors();
		///////////////////////ANIMATION//////////////////////////
	}
	else
	{
		assert(false && "Type of render request not supported");
	}

	// Getting uniform locations for glUniform* calls
	GLint color_uloc = glGetUniformLocation(program, "fcolor");
	const vec3 color = registry.colors.has(entity) ? registry.colors.get(entity) : vec3(1);
	glUniform3fv(color_uloc, 1, (float *)&color);
	gl_has_errors();

	GLint opacity_uloc = glGetUniformLocation(program, "opacity");
	const float opacity = registry.opacities.has(entity) ? registry.opacities.get(entity) : 1.f;
	glUniform1f(opacity_uloc, opacity);
	gl_has_errors();

	// Get number of indices from index buffer, which has elements uint16_t
	GLint size = 0;
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
	gl_has_errors();

	GLsizei num_indices = size / sizeof(uint16_t);
	// GLsizei num_triangles = num_indices / 3;

	GLint currProgram;
	glGetIntegerv(GL_CURRENT_PROGRAM, &currProgram);
	// Setting uniform values to the currently bound program
	GLuint transform_loc = glGetUniformLocation(currProgram, "transform");
	glUniformMatrix3fv(transform_loc, 1, GL_FALSE, (float *)&transform.mat);
	GLuint projection_loc = glGetUniformLocation(currProgram, "projection");
	glUniformMatrix3fv(projection_loc, 1, GL_FALSE, (float *)&projection);
	gl_has_errors();
	// Drawing of num_indices/3 triangles specified in the index buffer
	glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_SHORT, nullptr);
	gl_has_errors();
}

// draw the intermediate texture to the screen, with some distortion to simulate
// water
void RenderSystem::drawToScreen()
{
	glBindVertexArray(vao);

	// Setting shaders
	// get the water texture, sprite mesh, and program
	glUseProgram(effects[(GLuint)EFFECT_ASSET_ID::WATER]);
	gl_has_errors();
	// Clearing backbuffer
	int w, h;
	glfwGetFramebufferSize(window, &w, &h); // Note, this will be 2x the resolution given to glfwCreateWindow on retina displays
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, w, h);
	glDepthRange(0, 10);
	glClearColor(1.f, 1.f, 1.f, 1.0);
	glClearDepth(1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	gl_has_errors();
	// Enabling alpha channel for textures
	glDisable(GL_BLEND);
	// glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);

	// Draw the screen texture on the quad geometry
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffers[(GLuint)GEOMETRY_BUFFER_ID::SCREEN_TRIANGLE]);
	glBindBuffer(
		GL_ELEMENT_ARRAY_BUFFER,
		index_buffers[(GLuint)GEOMETRY_BUFFER_ID::SCREEN_TRIANGLE]); // Note, GL_ELEMENT_ARRAY_BUFFER associates
																	 // indices to the bound GL_ARRAY_BUFFER
	gl_has_errors();
	const GLuint water_program = effects[(GLuint)EFFECT_ASSET_ID::WATER];
	// Set clock
	GLuint time_uloc = glGetUniformLocation(water_program, "time");
	GLuint dead_timer_uloc = glGetUniformLocation(water_program, "darken_screen_factor");
	glUniform1f(time_uloc, (float)(glfwGetTime() * 10.0f));
	ScreenState &screen = registry.screenStates.get(screen_state_entity);
	glUniform1f(dead_timer_uloc, screen.darken_screen_factor);
	gl_has_errors();
	// Set the vertex position and vertex texture coordinates (both stored in the
	// same VBO)
	GLint in_position_loc = glGetAttribLocation(water_program, "in_position");
	glEnableVertexAttribArray(in_position_loc);
	glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void *)0);
	gl_has_errors();

	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);

	glBindTexture(GL_TEXTURE_2D, off_screen_render_buffer_color);
	gl_has_errors();
	// Draw
	glDrawElements(
		GL_TRIANGLES, 3, GL_UNSIGNED_SHORT,
		nullptr); // one triangle = 3 vertices; nullptr indicates that there is
				  // no offset from the bound index buffer
	gl_has_errors();
}

// from simpleGl lecture 3
void RenderSystem::renderText(const mat3 &projection)
{

	// activate the shader program
	glUseProgram(m_font_shaderProgram);
	gl_has_errors();

	for (Entity entity : registry.texts.entities)
	{
		// if not moving
		if (!registry.motions.has(entity))
		{
			continue;
		}

		Motion &motion = registry.motions.get(entity);
		Text &text_component = registry.texts.get(entity);
		std::string text = text_component.info;
		float x = motion.position.x;
		float y = motion.position.y;
		float scale = motion.scale.x;

		GLint colorLocation = glGetUniformLocation(m_font_shaderProgram, "textColor");
		assert(colorLocation >= 0);
		glUniform3f(colorLocation, text_component.color.x, text_component.color.y, text_component.color.z);

		GLint transformLocation = glGetUniformLocation(m_font_shaderProgram, "transform");
		assert(transformLocation >= 0);
		glm::mat4 p =
			glm::mat4(1.0f); // not sure why but this works, dont try to pass in transformation matrix, won't work
		glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(p));
		glBindVertexArray(m_font_VAO);

		// iterate through all characters
		std::string::const_iterator c;
		for (c = text.begin(); c != text.end(); c++)
		{
			Character ch = m_ftCharacters[*c];

			float xpos = x + ch.Bearing.x * scale;
			float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

			float w = ch.Size.x * scale;
			float h = ch.Size.y * scale;
			// update VBO for each character
			float vertices[6][4] = {
				{xpos, ypos + h, 0.0f, 0.0f}, {xpos, ypos, 0.0f, 1.0f},		{xpos + w, ypos, 1.0f, 1.0f},

				{xpos, ypos + h, 0.0f, 0.0f}, {xpos + w, ypos, 1.0f, 1.0f}, {xpos + w, ypos + h, 1.0f, 0.0f}};

			// render glyph texture over quad
			glBindTexture(GL_TEXTURE_2D, ch.TextureID);
			// std::cout << "binding texture: " << ch.character << " = " << ch.TextureID << std::endl;

			// update content of VBO memory
			glBindBuffer(GL_ARRAY_BUFFER, m_font_VBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			// render quad
			glDrawArrays(GL_TRIANGLES, 0, 6);

			// now advance cursors for next glyph (note that advance is number of 1/64 pixels)
			x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
		}
		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

//takes game state to check current state
void RenderSystem::draw(GAME_STATE current_state, float elapsed_ms)
{
	static GAME_STATE previous_state = GAME_STATE::MAIN_MENU;
	static bool main_menu_initialized = false;

	// Getting size of window
	int w, h;
	glfwGetFramebufferSize(window, &w,
						   &h); // Note, this will be 2x the resolution given to glfwCreateWindow on retina displays

	// Handle initial setup for MAIN_MENU
	if (!main_menu_initialized && previous_state == GAME_STATE::MAIN_MENU)
	{
		initializeMenuEntities(GAME_STATE::MAIN_MENU);
		main_menu_initialized = true; // flag
	}

	// Handle state transitions
	if (current_state != previous_state)
	{
		// std::cout << "State changed from " << static_cast<int>(previous_state) << " to "
				  // << static_cast<int>(current_state) << std::endl;

		clearMenuEntities(previous_state);
		initializeMenuEntities(current_state);

		// Update the previous state
		previous_state = current_state;

		// to prevent multiple initializations...
		if (previous_state != GAME_STATE::MAIN_MENU)
			main_menu_initialized = false;
	}

	// Render menus if not in gameplay
	if (current_state != GAME_STATE::GAMEPLAY)
	{
		renderMenu(current_state, w, h);
		return;
	}
	
	// If in gameplay, proceed with normal rendering
	// First render to the custom framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
	gl_has_errors();
	// Clearing backbuffer
	glViewport(0, 0, w, h);
	glDepthRange(0.00001, 10);
	glClearColor(0.13f, 0.13f, 0.13f, 1.0);
	glClearDepth(10.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);
	gl_has_errors();

	// Set up projection and view matrices
	mat3 projection_2D = createProjectionMatrix();
	auto camera = registry.cameras.entities[0];
	mat3 view_2D = CameraSystem::createViewMatrix(camera);
	mat3 pv_matrix = projection_2D * view_2D;
	mat3 ortho_projection = createOrthographicProjection(w, h); // ortho projection for ui

	// Draw all textured meshes that have a position and size component
	for (Entity entity : registry.renderRequests.entities)
	{
		// Skip entities that don't have a motion component
		if (!registry.motions.has(entity))
			continue;

		int frame_current = 0;
		GLfloat frame_width = 0;

		// Handle animation if it exists
		AnimationSystem::applyAnimation(entity, elapsed_ms, frame_current, frame_width);

		// Use ortho projection for UI elements, otherwise use perspective view matrix
		if (registry.huds.has(entity))
		{
			drawTexturedMesh(entity, ortho_projection, frame_current, frame_width, elapsed_ms); //UI elements
		}
		else
		{
			drawTexturedMesh(entity, pv_matrix, frame_current, frame_width, elapsed_ms); //world-space elements
		}
	}

	//	glUseProgram(reload_program);

	//	GLint progress_uloc = glGetUniformLocation(reload_program, "progress");
	//	GLint position_uloc = glGetUniformLocation(reload_program, "cursorPosition");
	//	glUniform1f(progress_uloc, progress);
	//	glUniform2f(position_uloc, cursor_x, cursor_y);


	//	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffers[(GLuint)GEOMETRY_BUFFER_ID::RELOAD]);
	//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffers[(GLuint)GEOMETRY_BUFFER_ID::RELOAD]);
	//	glDrawElements(GL_TRIANGLES, , GL_UNSIGNED_SHORT, nullptr);
	//	gl_has_errors();

	//}
	// 
	// 

	// render text
	renderText(projection_2D);
	// Truely render to the screen
	drawToScreen();

	// Flicker-free display with a double buffer
	glfwSwapBuffers(window);
	gl_has_errors();
}


mat3 RenderSystem::createProjectionMatrix()
{
	// Fake projection matrix, scales with respect to window coordinates
	float left = 0.f;
	float top = 0.f;

	gl_has_errors();
	float right = (float)window_width_px;
	float bottom = (float)window_height_px;

	float sx = 2.f / (right - left);
	float sy = 2.f / (top - bottom);
	float tx = -(right + left) / (right - left);
	float ty = -(top + bottom) / (top - bottom);
	return {{sx, 0.f, 0.f}, {0.f, sy, 0.f}, {tx, ty, 1.f}};
}

//////////////////////////////HUD and MENU////////////////////////////////////

mat3 RenderSystem::createOrthographicProjection(float width, float height)
{
	float left = 0.0f;
	float right = width;
	float bottom = height;
	float top = 0.0f;

	// glm::ortho parameters: left, right, bottom, top
	return glm::ortho(left, right, bottom, top);
}


void RenderSystem::renderMenu(GAME_STATE current_state, int w, int h)
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, w, h);
	glClearColor(0.2f, 0.2f, 0.2f, 1.0); // for debug
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mat3 ortho_projection = createOrthographicProjection(w, h);

	if (cached_entities.find(current_state) != cached_entities.end())
	{
		for (Entity entity : cached_entities[current_state])
		{
			drawTexturedMesh(entity, ortho_projection, place_holder_int, place_holder_float, 0);
		}
	}
	else
	{
		std::cout << "No cached entities for state: " << static_cast<int>(current_state) << std::endl;
	}

	glfwSwapBuffers(window);
	gl_has_errors();
}

Entity RenderSystem::createMenu(TEXTURE_ASSET_ID texture_id, vec2 pos, vec2 scale)
{
	auto entity = Entity();

	Motion &motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.scale = scale;

	registry.renderRequests.insert(entity, {texture_id, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE});

	return entity;
}

Entity RenderSystem::createButton(TEXTURE_ASSET_ID texture_id, vec2 pos, vec2 scale)
{
	Entity entity = Entity();

	Motion &motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.scale = scale;

	Button &button = registry.buttons.emplace(entity);
	// std::cout << "Button created: " << entity << std::endl;

	registry.renderRequests.insert(entity, {texture_id, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE});

	return entity;
}

void RenderSystem::initializeMenuEntities(GAME_STATE state)
{
	if (state == GAME_STATE::MAIN_MENU)
	{

		Entity menu_entity = createMenu(TEXTURE_ASSET_ID::MAIN_MENU, {0, 0}, {window_width_px, window_height_px});
		cached_entities[state].push_back(menu_entity);
		// std::cout << "Main menu created: " << menu_entity << std::endl;
	}
	else if (state == GAME_STATE::PAUSED)
	{
		Entity menu_entity = createMenu(TEXTURE_ASSET_ID::PAUSE_MENU, {0, 0}, {window_width_px, window_height_px});
		cached_entities[state].push_back(menu_entity);
		// std::cout << "Pause menu created: " << menu_entity << std::endl;
		
	}
	else if (state == GAME_STATE::LEVEL_SELECTION)
	{
		Entity menu_entity = createMenu(TEXTURE_ASSET_ID::LEVEL_MENU, {0, 0}, {window_width_px, window_height_px});
		Entity level1_button = createButton(TEXTURE_ASSET_ID::LEVEL_BUTTON, {0, 0}, {200, 100});
		
		cached_entities[state].push_back(menu_entity);
		cached_entities[state].push_back(level1_button);
		// std::cout << "Level selection menu created: " << menu_entity << std::endl;
	}
}

void RenderSystem::clearMenuEntities(GAME_STATE state)
{
	if (cached_entities.find(state) != cached_entities.end())
	{
		for (Entity entity : cached_entities[state])
		{
			registry.remove_all_components_of(entity);
		}
		cached_entities[state].clear();
	}
}

