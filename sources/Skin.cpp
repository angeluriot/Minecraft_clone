#include "Skin.h"
#include "Texture.h"
#include "Block.h"
#include "Game.h"

// Crée une partie du corps

Skin::Part::Part()
{
	propagated_matrix = glm::mat4(1.f);
	local_matrix = glm::mat4(1.f);
	children.clear();
}

// Crée une partie du corps à partir d'une autre

Skin::Part::Part(const Part& other)
{
	*this = other;
}

// Crée une partie du corps à partir de ces paramètres

Skin::Part::Part(Mesh mesh, const glm::vec2& texture_pos, const glm::vec3& size, const std::vector<Part*>& children)
{
	mesh.texcoords = init_texcoords(texture_pos / texture_size, size / texture_size);
	object.send_data(Shader::entity, mesh);
	propagated_matrix = glm::mat4(1.f);
	local_matrix = glm::mat4(1.f);
	this->children = children;
}

// Opérateur égal

Skin::Part& Skin::Part::operator=(const Part& other)
{
	object = other.object;
	propagated_matrix = other.propagated_matrix;
	local_matrix = other.local_matrix;
	children = other.children;

	return *this;
}

// Affiche la partie du corps

void Skin::Part::draw(const Camera& camera, const std::vector<const Light*>& lights, const Plane& clipping_plane) const
{
	glm::mat4 model = Game::matrices.top() * propagated_matrix * local_matrix;
	Game::matrices.push(Game::matrices.top() * propagated_matrix);

	object.bind();

	ColorRGB water_color = ColorRGB(0.f, 0.f, 0.f);

	for (uint16_t i = 0; i < std::min((int)lights.size(), (int)nb_max_lights); i++)
		water_color += ColorRGB(Material::water.get_color()) * ColorRGB(lights[i]->get_color()) * lights[i]->get_intensity();

	object.send_uniform("u_model", model);
	object.send_uniform("u_inverted_model", glm::transpose(glm::inverse(glm::mat3(model))));
	object.send_uniform("u_mvp", camera.get_matrix() * model);
	object.send_uniform("u_camera", camera.get_position());
	object.send_uniform("u_ambient", Material::entity.get_ambient());
	object.send_uniform("u_diffuse", Material::entity.get_diffuse());
	object.send_uniform("u_specular", Material::entity.get_specular());
	object.send_uniform("u_shininess", Material::entity.get_shininess());
	object.send_uniform("u_fake_cam", (int)Game::fake_cam);
	object.send_uniform("u_water_level", water_level);
	object.send_uniform("u_water_color", water_color);
	object.send_uniform("u_clipping_plane", clipping_plane);
	object.send_texture("u_texture", 0);

	std::vector<int> light_types;
	std::vector<glm::vec3> light_vectors;
	std::vector<ColorRGB> light_colors;
	std::vector<float> light_intensities;

	for (uint16_t i = 0; i < std::min((int)lights.size(), (int)nb_max_lights); i++)
	{
		light_types.push_back((int)lights[i]->get_type());
		light_vectors.push_back(lights[i]->get_vector());
		light_colors.push_back(ColorRGB(lights[i]->get_color()));
		light_intensities.push_back(lights[i]->get_intensity());
	}

	object.send_uniform("u_light_types", light_types);
	object.send_uniform("u_light_vectors", light_vectors);
	object.send_uniform("u_light_colors", light_colors);
	object.send_uniform("u_light_intensities", light_intensities);
	object.send_uniform("u_nb_lights", std::min((int)lights.size(), (int)nb_max_lights));

	object.draw();

	VertexBuffer::unbind();

	for (auto child : children)
		child->draw(camera, lights, clipping_plane);

	Game::matrices.pop();
}

// Donne les coordonnées de textures à partir de la position et de la taille sur l'image

std::vector<float> Skin::Part::init_texcoords(const glm::vec2& texture_pos, const glm::vec3& size)
{
	std::vector<float> texcoords = Mesh::cube.texcoords;
	std::array<glm::vec2, 6> pos_1;
	std::array<glm::vec2, 6> pos_2;

	pos_1[3] = texture_pos + glm::vec2(0, size.z);
	pos_1[0] = pos_1[3] + glm::vec2(size.z, 0);
	pos_1[2] = pos_1[0] + glm::vec2(size.x, 0);
	pos_1[1] = pos_1[2] + glm::vec2(size.z, 0);

	pos_1[4] = texture_pos + glm::vec2(size.z, 0);
	pos_1[5] = pos_1[4] + glm::vec2(size.x, 0);

	pos_2[3] = pos_1[3] + glm::vec2(size.z, size.y);
	pos_2[0] = pos_2[3] + glm::vec2(size.x, 0);
	pos_2[2] = pos_2[0] + glm::vec2(size.z, 0);
	pos_2[1] = pos_2[2] + glm::vec2(size.x, 0);

	pos_2[4] = pos_1[4] + glm::vec2(size.x, size.z);
	pos_2[5] = pos_2[4] + glm::vec2(size.x, 0);

	for (uint8_t i = 0; i < Cube::nb_faces; i++)
		for (uint8_t j = 0; j < 2 * Cube::Face::nb_points; j += 2)
		{
			texcoords[i * 2 * Cube::Face::nb_points + j] = texcoords[i * 2 * Cube::Face::nb_points + j] * (pos_2[i].x - pos_1[i].x) + pos_1[i].x;
			texcoords[i * 2 * Cube::Face::nb_points + j + 1] = texcoords[i * 2 * Cube::Face::nb_points + j + 1] * (pos_2[i].y - pos_1[i].y) + pos_1[i].y;
		}

	return texcoords;
}

// Crée un model 3D du joueur

Skin::Skin()
{
	walking = false;
	running = false;
	forward = false;
	left = false;
	right = false;
	back = false;
	animation_i = 0;
	walk_strength = 0.f;
	shoulder_1_initial = glm::mat4(0.f);
	shoulder_2_initial = glm::mat4(0.f);
	hips_1_initial = glm::mat4(0.f);
	hips_2_initial = glm::mat4(0.f);
	neck_initial = glm::mat4(0.f);
	body_angle = 0.f;
	head_angle = 0.f;
}

// Crée un model 3D du joueur à partir d'un autre

Skin::Skin(const Skin& other)
{
	*this = other;
}

// Crée un model 3D du joueur à partir de ces paramètres

Skin::Skin(const glm::vec3& position, const glm::vec3& direction)
{
	arm_1 = Part(Mesh::cube, glm::vec2(32, 48), glm::vec3(4, 12, 4), {});
	shoulder_1 = Part(Mesh::null, glm::vec2(), glm::vec3(), { &arm_1 });

	arm_2 = Part(Mesh::cube, glm::vec2(40, 16), glm::vec3(4, 12, 4), {});
	shoulder_2 = Part(Mesh::null, glm::vec2(), glm::vec3(), { &arm_2 });

	leg_1 = Part(Mesh::cube, glm::vec2(16, 48), glm::vec3(4, 12, 4), {});
	hips_1 = Part(Mesh::null, glm::vec2(), glm::vec3(), { &leg_1 });

	leg_2 = Part(Mesh::cube, glm::vec2(0, 16), glm::vec3(4, 12, 4), {});
	hips_2 = Part(Mesh::null, glm::vec2(), glm::vec3(), { &leg_2 });

	camera = Part(Mesh::null, glm::vec2(), glm::vec3(), {});
	head = Part(Mesh::cube, glm::vec2(0, 0), glm::vec3(8, 8, 8), { &camera });
	neck = Part(Mesh::null, glm::vec2(), glm::vec3(), { &head });

	body = Part(Mesh::cube, glm::vec2(16, 16), glm::vec3(8, 12, 4), { &shoulder_1, &shoulder_2, &hips_1, &hips_2, &neck });

	// Positions

	arm_1.propagated_matrix = glm::translate(arm_1.propagated_matrix, glm::vec3(0.f, -6.f / 16.f, 0.f));
	shoulder_1.propagated_matrix = glm::translate(shoulder_1.propagated_matrix, glm::vec3(-6.f / 16.f, 8.f / 16.f, 0.f));

	arm_2.propagated_matrix = glm::translate(arm_2.propagated_matrix, glm::vec3(0.f, -6.f / 16.f, 0.f));
	shoulder_2.propagated_matrix = glm::translate(shoulder_2.propagated_matrix, glm::vec3(6.f / 16.f, 8.f / 16.f, 0.f));

	leg_1.propagated_matrix = glm::translate(leg_1.propagated_matrix, glm::vec3(0.f, -6.f / 16.f, 0.f));
	hips_1.propagated_matrix = glm::translate(hips_1.propagated_matrix, glm::vec3(-2.f / 16.f, -4.f / 16.f, 0.f));

	leg_2.propagated_matrix = glm::translate(leg_2.propagated_matrix, glm::vec3(0.f, -6.f / 16.f, 0.f));
	hips_2.propagated_matrix = glm::translate(hips_2.propagated_matrix, glm::vec3(2.f / 16.f, -4.f / 16.f, 0.f));

	head.propagated_matrix = glm::translate(head.propagated_matrix, glm::vec3(0.f, 4.f / 16.f, 0.f));
	neck.propagated_matrix = glm::translate(neck.propagated_matrix, glm::vec3(0.f, 8.f / 16.f, 0.f));

	body.local_matrix = glm::translate(body.local_matrix, glm::vec3(0.f, 2.f / 16.f, 0.f));

	// Tailles

	leg_1.local_matrix = glm::scale(leg_1.local_matrix, glm::vec3(4.f / 16.f, 12.f / 16.f, 4.f / 16.f));
	leg_2.local_matrix = glm::scale(leg_2.local_matrix, glm::vec3(4.f / 16.f, 12.f / 16.f, 4.f / 16.f));

	arm_1.local_matrix = glm::scale(arm_1.local_matrix, glm::vec3(4.f / 16.f, 12.f / 16.f, 4.f / 16.f));
	arm_2.local_matrix = glm::scale(arm_2.local_matrix, glm::vec3(4.f / 16.f, 12.f / 16.f, 4.f / 16.f));

	head.local_matrix = glm::scale(head.local_matrix, glm::vec3(8.f / 16.f, 8.f / 16.f, 8.f / 16.f));

	body.local_matrix = glm::scale(body.local_matrix, glm::vec3(8.f / 16.f, 12.f / 16.f, 4.f / 16.f));

	shoulder_1_initial = shoulder_1.propagated_matrix;
	shoulder_2_initial = shoulder_2.propagated_matrix;
	hips_1_initial = hips_1.propagated_matrix;
	hips_2_initial = hips_2.propagated_matrix;
	neck_initial = neck.propagated_matrix;

	walking = false;
	running = false;
	forward = false;
	left = false;
	right = false;
	back = false;
	animation_i = 0;
	walk_strength = 0.;
	body_angle = 0.f;
	head_angle = 0.f;
}

// Opérateur égal

Skin& Skin::operator=(const Skin& other)
{
	arm_1 = other.arm_1;
	shoulder_1 = other.shoulder_1;
	arm_2 = other.arm_2;
	shoulder_2 = other.shoulder_2;
	leg_1 = other.leg_1;
	hips_1 = other.hips_1;
	leg_2 = other.leg_2;
	hips_2 = other.hips_2;
	camera = other.camera;
	head = other.head;
	neck = other.neck;
	body = other.body;
	walking = other.walking;
	running = other.running;
	forward = other.forward;
	left = other.left;
	right = other.right;
	back = other.back;
	animation_i = other.animation_i;
	walk_strength = other.walk_strength;
	shoulder_1_initial = other.shoulder_1_initial;
	shoulder_2_initial = other.shoulder_2_initial;
	hips_1_initial = other.hips_1_initial;
	hips_2_initial = other.hips_2_initial;
	neck_initial = other.neck_initial;
	body_angle = other.body_angle;
	head_angle = other.head_angle;

	shoulder_1.children = { &arm_1 };
	shoulder_2.children = { &arm_2 };
	hips_1.children = { &leg_1 };
	hips_2.children = { &leg_2 };
	head.children = { &camera };
	neck.children = { &head };
	body.children = { &shoulder_1, &shoulder_2, &hips_1, &hips_2, &neck };

	return *this;
}

// Déplace le model 3D

void Skin::move(const glm::vec3& position)
{
	body.propagated_matrix = glm::translate(glm::mat4(1.f), position);
	update_walk();
}

// Tourne le model 3D

void Skin::rotate(float angle)
{
	body_angle = angle;
	body.propagated_matrix = glm::rotate(body.propagated_matrix, body_angle, glm::vec3(0.f, 1.f, 0.f));
}

// Tourne la tête

void Skin::rotate_head(float yaw, float pitch)
{
	head_angle = -pi / 2.f - glm::radians(yaw);

	if (abs(head_angle - body_angle) > max_head_rotation)
	{
		if (head_angle > body_angle)
			body_angle += abs(head_angle - body_angle) - max_head_rotation;

		else
			body_angle -= abs(head_angle - body_angle) - max_head_rotation;
	}

	body.propagated_matrix = glm::rotate(body.propagated_matrix, body_angle, glm::vec3(0.f, 1.f, 0.f));

	neck.propagated_matrix = glm::rotate(neck_initial, head_angle - body_angle, glm::vec3(0.f, 1.f, 0.f));
	neck.propagated_matrix = glm::rotate(neck.propagated_matrix, glm::radians(pitch), glm::vec3(1.f, 0.f, 0.f));
}

// Met à jour l'animation de la marche (n'essayez pas de comprendre cette fonction)

void Skin::update_walk()
{
	if (walk_strength < strength_step)
		walk_strength = 0.f;

	if (walking && walk_strength < max_walk_strength)
		walk_strength += strength_step;

	if (!walking && walk_strength > 0.f)
		walk_strength -= strength_step;

	if ((forward || back) && !left && !right)
	{
		if (abs(body_angle - head_angle) < turn_speed)
			body_angle = head_angle;

		else if (body_angle > head_angle)
			body_angle -= turn_speed;

		else
			body_angle += turn_speed;
	}

	if ((forward && left && !right) || (back && !left && right))
	{
		if (abs(body_angle - (head_angle + max_head_rotation / 2.f)) < turn_speed)
			body_angle = head_angle + max_head_rotation / 2.f;

		else if (body_angle > head_angle + max_head_rotation / 2.f)
			body_angle -= turn_speed;

		else
			body_angle += turn_speed;
	}

	// Je vous avais prévenu...

	if ((forward && !left && right) || (back && left && !right))
	{
		if (abs(body_angle - (head_angle - max_head_rotation / 2.f)) < turn_speed)
			body_angle = head_angle - max_head_rotation / 2.f;

		else if (body_angle > head_angle - max_head_rotation / 2.f)
			body_angle -= turn_speed;

		else
			body_angle += turn_speed;
	}

	if (!forward && !back && left)
		body_angle += turn_speed;

	if (!forward && !back && right)
		body_angle -= turn_speed;

	shoulder_1.propagated_matrix = glm::rotate(shoulder_1_initial, -walk_strength * float(sin((animation_i / 8.f) * walk_anim_speed)), glm::vec3(1.f, 0.f, 0.f));
	shoulder_2.propagated_matrix = glm::rotate(shoulder_2_initial, walk_strength * float(sin((animation_i / 8.f) * walk_anim_speed)), glm::vec3(1.f, 0.f, 0.f));

	hips_1.propagated_matrix = glm::rotate(hips_1_initial, walk_strength * float(sin((animation_i / 8.f) * walk_anim_speed)), glm::vec3(1.f, 0.f, 0.f));
	hips_2.propagated_matrix = glm::rotate(hips_2_initial, -walk_strength * float(sin((animation_i / 8.f) * walk_anim_speed)), glm::vec3(1.f, 0.f, 0.f));

	animation_i += (running ? 10 : 8);
}

// Affiche le model 3D

void Skin::draw(const Camera& camera, const std::vector<const Light*>& lights, const Plane& clipping_plane) const
{
	Shader::entity.bind();
	Texture::player.bind(0);

	body.draw(camera, lights, clipping_plane);
		
	Texture::unbind();
	Shader::unbind();
}
