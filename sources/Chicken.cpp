#include "Chicken.h"

// Crée une poule

Chicken::Chicken() : Mob()
{
	beak_bottom_initial = glm::mat4(1.f);
	shoulder_1_initial = glm::mat4(1.f);
	shoulder_2_initial = glm::mat4(1.f);
	hips_1_initial = glm::mat4(1.f);
	hips_2_initial = glm::mat4(1.f);
}

// Crée une poule à partir d'une autre

Chicken::Chicken(const Chicken& other)
{
	*this = other;
}

// Crée une poule à partir de sa position

Chicken::Chicken(const glm::vec3& position) : Mob()
{
	y_offset = -0.1f / 16.f - 0.5f;
	texture = &Texture::chicken;
	this->position = position;

	palm = Part(Mesh::cube, glm::vec2(20.f, 6.f), glm::vec3(3.f, 1.f, 2.f), {}, false);
	leg = Part(Mesh::cube, glm::vec2(26.f, 0.f), glm::vec3(1.f, 4.f, 1.f), { &palm }, false);

	wattle = Part(Mesh::cube, glm::vec2(14.f, 5.f), glm::vec3(2.f, 3.f, 1.f), {}, false);
	beak_bottom = Part(Mesh::null, glm::vec2(), glm::vec3(), { &wattle }, false);
	beak = Part(Mesh::cube, glm::vec2(14.f, 0.f), glm::vec3(4.f, 2.f, 2.f), { &beak_bottom }, false);
	head = Part(Mesh::cube, glm::vec2(0.f, 0.f), glm::vec3(4.f, 6.f, 3.f), { &beak }, false);

	hips_1 = Part(Mesh::null, glm::vec2(), glm::vec3(), { &leg }, false);
	hips_2 = Part(Mesh::null, glm::vec2(), glm::vec3(), { &leg }, false);

	wing_1 = Part(Mesh::cube, glm::vec2(24.f, 13.f), glm::vec3(1.f, 4.f, 6.f), {}, false);
	wing_2 = Part(Mesh::cube, glm::vec2(24.f, 13.f), glm::vec3(1.f, 4.f, 6.f), {}, false);

	shoulder_1 = Part(Mesh::null, glm::vec2(), glm::vec3(), { &wing_1 }, false);
	shoulder_2 = Part(Mesh::null, glm::vec2(), glm::vec3(), { &wing_2 }, false);
	neck = Part(Mesh::null, glm::vec2(), glm::vec3(), { &head }, false);
	body = Part(Mesh::cube, glm::vec2(0.f, 9.f), glm::vec3(6.f, 6.f, 8.f), { &neck, &hips_1, &hips_2, &shoulder_1, &shoulder_2 }, true);

	// Positions

	shoulder_1.propagated_matrix = glm::translate(shoulder_1.propagated_matrix, glm::vec3(-3.f / 16.f, 3.f / 16.f, 0.f));
	shoulder_2.propagated_matrix = glm::translate(shoulder_2.propagated_matrix, glm::vec3(3.f / 16.f, 3.f / 16.f, 0.f));

	wing_1.propagated_matrix = glm::translate(wing_1.propagated_matrix, glm::vec3(-0.5f / 16.f, -2.f / 16.f, 0.f));
	wing_2.propagated_matrix = glm::translate(wing_2.propagated_matrix, glm::vec3(0.5f / 16.f, -2.f / 16.f, 0.f));

	hips_1.propagated_matrix = glm::translate(hips_1.propagated_matrix, glm::vec3(-1.5f / 16.f, -3.f / 16.f, 0.f));
	hips_2.propagated_matrix = glm::translate(hips_2.propagated_matrix, glm::vec3(1.5f / 16.f, -3.f / 16.f, 0.f));

	leg.propagated_matrix = glm::translate(leg.propagated_matrix, glm::vec3(0.f, -2.f / 16.f, 0.f));
	palm.propagated_matrix = glm::translate(palm.propagated_matrix, glm::vec3(0.f, -1.5f / 16.f, -1.5f / 16.f));

	wattle.propagated_matrix = glm::translate(wattle.propagated_matrix, glm::vec3(0.f, -0.5f / 16.f, 0.f));
	beak_bottom.propagated_matrix = glm::translate(beak_bottom.propagated_matrix, glm::vec3(0.f, -1.f / 16.f, 0.5f / 16.f));
	beak.propagated_matrix = glm::translate(beak.propagated_matrix, glm::vec3(0.f, 0.f, -2.5f / 16.f));
	head.propagated_matrix = glm::translate(head.propagated_matrix, glm::vec3(0.f, 3.f / 16.f, 0.f));
	neck.propagated_matrix = glm::translate(neck.propagated_matrix, glm::vec3(0.f, 1.f / 16.f, -4.f / 16.f));

	// Tailles
	
	wing_1.local_matrix = glm::scale(wing_1.local_matrix, glm::vec3(1.f / 16.f, 4.f / 16.f, 6.f / 16.f));
	wing_2.local_matrix = glm::scale(wing_2.local_matrix, glm::vec3(1.f / 16.f, 4.f / 16.f, 6.f / 16.f));

	wattle.local_matrix = glm::scale(wattle.local_matrix, glm::vec3(2.f / 16.f, 3.f / 16.f, 1.f / 16.f));
	beak.local_matrix = glm::scale(beak.local_matrix, glm::vec3(4.f / 16.f, 2.f / 16.f, 2.f / 16.f));
	head.local_matrix = glm::scale(head.local_matrix, glm::vec3(4.f / 16.f, 6.f / 16.f, 3.f / 16.f));
	body.local_matrix = glm::scale(body.local_matrix, glm::vec3(6.f / 16.f, 6.f / 16.f, 8.f / 16.f));

	leg.local_matrix = glm::scale(leg.local_matrix, glm::vec3(1.f / 16.f, 4.f / 16.f, 1.f / 16.f));
	palm.local_matrix = glm::scale(palm.local_matrix, glm::vec3(3.f / 16.f, 1.f / 16.f, 2.f / 16.f));

	beak_bottom_initial = beak_bottom.propagated_matrix;
	shoulder_1_initial = shoulder_1.propagated_matrix;
	shoulder_2_initial = shoulder_2.propagated_matrix;
	hips_1_initial = hips_1.propagated_matrix;
	hips_2_initial = hips_2.propagated_matrix;
	body_initial = body.propagated_matrix;
	neck_initial = neck.propagated_matrix;

	move(this->position);
}

// Opérateur égal

Chicken& Chicken::operator=(const Chicken& other)
{
	Mob::operator=(other);

	head = other.head;
	shoulder_1 = other.shoulder_1;
	shoulder_2 = other.shoulder_2;
	hips_1 = other.hips_1;
	hips_2 = other.hips_2;
	leg = other.leg;
	palm = other.palm;
	wing_1 = other.wing_1;
	wing_2= other.wing_2;
	beak = other.beak;
	beak_bottom = other.beak_bottom;
	wattle = other.wattle;
	beak_bottom_initial = other.beak_bottom_initial;
	shoulder_1_initial = other.shoulder_1_initial;
	shoulder_2_initial = other.shoulder_2_initial;
	hips_1_initial = other.hips_1_initial;
	hips_2_initial = other.hips_2_initial;

	leg.children = { &palm };
	beak_bottom.children = { &wattle };
	beak.children = { &beak_bottom };
	head.children = { &beak };
	hips_1.children = { &leg };
	hips_2.children = { &leg };
	shoulder_1.children = { &wing_1 };
	shoulder_2.children = { &wing_2 };
	neck.children = { &head };
	body.children = { &neck, &hips_1, &hips_2, &shoulder_1, &shoulder_2 };

	return *this;
}

// Anime la poule

void Chicken::animate(float time)
{
	beak_bottom.propagated_matrix = glm::rotate(beak_bottom_initial, -sin(time) / 8.f, glm::vec3(0.f, 0.f, 1.f));
	shoulder_1.propagated_matrix = glm::rotate(shoulder_1_initial, -(pi / 4.f + sin(2.f * time)) / 2.f, glm::vec3(0.f, 0.f, 1.f));
	shoulder_2.propagated_matrix = glm::rotate(shoulder_2_initial, (pi / 4.f + sin(2.f * time)) / 2.f, glm::vec3(0.f, 0.f, 1.f));

	hips_1.propagated_matrix = glm::rotate(hips_1_initial, -sin(2.f * time), glm::vec3(1.f, 0.f, 0.f));
	hips_2.propagated_matrix = glm::rotate(hips_2_initial, sin(2.f * time), glm::vec3(1.f, 0.f, 0.f));

	body.propagated_matrix = glm::translate(body_initial, sin(time * 2.f * 18.f / 10.f) / 80.f * glm::vec3(0.f, 1.f, 0.5f));
	neck.propagated_matrix = glm::translate(neck_initial, sin(time) / 20.f * glm::vec3(0.f, 0.f, 0.5f) + 1.f / 20.f * glm::vec3(0.f, 0.f, -0.25f));
	neck.propagated_matrix = glm::rotate(neck_initial, -sin(time * 18.f / 10.f) / 8.f, glm::vec3(0.f, 0.f, 1.f));
	beak_bottom.propagated_matrix = glm::rotate(beak_bottom_initial, -sin(time * 18.f / 10.f) / 8.f, glm::vec3(0.f, 0.f, 1.f));
}
