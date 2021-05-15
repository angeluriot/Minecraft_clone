#include "Wolf.h"

// Crée un loup

Wolf::Wolf() : Mob()
{
	shoulder_1_initial = glm::mat4(1.f);
	shoulder_2_initial = glm::mat4(1.f);
	hip_1_initial = glm::mat4(1.f);
	hip_2_initial = glm::mat4(1.f);
}

// Crée un loup à partir d'un autre

Wolf::Wolf(const Wolf& other)
{
	*this = other;
}

// Crée un loup à partir de ces paramètres

Wolf::Wolf(const glm::vec3& position) : Mob()
{
	y_offset = 3.f / 16.f - 0.5f;
	texture = &Texture::wolf;
	this->position = position;

	muzzle = Part(Mesh::cube, glm::vec2(0.f, 10.f), glm::vec3(3.f, 3.f, 4.f), {}, false);
	ear_1 = Part(Mesh::cube, glm::vec2(16.f, 14.f), glm::vec3(2.f, 2.f, 1.f), {}, false);
	ear_2 = Part(Mesh::cube, glm::vec2(16.f, 14.f), glm::vec3(2.f, 2.f, 1.f), {}, false);
	head = Part(Mesh::cube, glm::vec2(0.f, 0.f), glm::vec3(6.f, 6.f, 4.f), { &muzzle, &ear_1, &ear_2 }, false);

	topbody = Part(Mesh::cube, glm::vec2(21.f, 0.f), glm::vec3(8.f, 7.f, 6.f), {}, true);
	tail = Part(Mesh::cube, glm::vec2(9.f, 18.f), glm::vec3(2.f, 2.f, 8.f), {}, true);

	front_leg_1 = Part(Mesh::cube, glm::vec2(0.f, 18.f), glm::vec3(2.f, 8.f, 2.f), {}, false);
	front_leg_2 = Part(Mesh::cube, glm::vec2(0.f, 18.f), glm::vec3(2.f, 8.f, 2.f), {}, false);
	back_leg_1 = Part(Mesh::cube, glm::vec2(0.f, 18.f), glm::vec3(2.f, 8.f, 2.f), {}, false);
	back_leg_2 = Part(Mesh::cube, glm::vec2(0.f, 18.f), glm::vec3(2.f, 8.f, 2.f), {}, false);

	neck = Part(Mesh::null, glm::vec2(), glm::vec3(), { &head }, false);
	tailbase = Part(Mesh::null, glm::vec2(), glm::vec3(), { &tail }, false);
	shoulder_1 = Part(Mesh::null, glm::vec2(), glm::vec3(), { &front_leg_1 }, false);
	shoulder_2 = Part(Mesh::null, glm::vec2(), glm::vec3(), { &front_leg_2 }, false);
	hip_1 = Part(Mesh::null, glm::vec2(), glm::vec3(), { &back_leg_1 }, false);
	hip_2 = Part(Mesh::null, glm::vec2(), glm::vec3(), { &back_leg_2 }, false);

	body = Part(Mesh::cube, glm::vec2(18.f, 14.f), glm::vec3(6.f, 6.f, 9.f), { &topbody, &neck, &tailbase, &shoulder_1, &shoulder_2, &hip_1, &hip_2 }, true);

	// Positions

	muzzle.propagated_matrix = glm::translate(muzzle.propagated_matrix, glm::vec3(0.f, -1.5f / 16.f, -1.f / 8.f - 1.5f / 16.f));
	muzzle.local_matrix = glm::scale(muzzle.local_matrix, glm::vec3(3.f / 16.f, 3.f / 16.f, 3.f / 16.f));
	ear_1.propagated_matrix = glm::translate(ear_1.propagated_matrix, glm::vec3(-1.f / 8.f, 1.f / 4.f, 1.f / 32.f));
	ear_1.local_matrix = glm::scale(ear_1.local_matrix, glm::vec3(1.f / 8.f, 1.f / 8.f, 1.f / 16.f));
	ear_2.propagated_matrix = glm::translate(ear_2.propagated_matrix, glm::vec3(1.f / 8.f, 1.f / 4.f, 1.f / 32.f));
	ear_2.local_matrix = glm::scale(ear_2.local_matrix, glm::vec3(1.f / 8.f, 1.f / 8.f, 1.f / 16.f));

	head.propagated_matrix = glm::translate(head.propagated_matrix, glm::vec3(0.f, 0.f, -1.f / 8.f));
	head.local_matrix = glm::scale(head.local_matrix, glm::vec3(3.f / 8.f, 3.f / 8.f, 1.f / 4.f));

	topbody.propagated_matrix = glm::translate(topbody.propagated_matrix, glm::vec3(0.f, 0.f, -7.6f / 16.f));
	topbody.local_matrix = glm::scale(topbody.local_matrix, glm::vec3(1.f / 2.f, 7.f / 16.f, 3.f / 8.f));

	tail.propagated_matrix = glm::translate(tail.propagated_matrix, glm::vec3(0.f, 0.f, 3.f / 16.f));
	tail.local_matrix = glm::scale(tail.local_matrix, glm::vec3(1.f / 8.f, 1.f / 8.f, 7.f / 16.f));
	front_leg_1.propagated_matrix = glm::translate(front_leg_1.propagated_matrix, glm::vec3(0.f, -1.f / 4.f, 0.f));
	front_leg_1.local_matrix = glm::scale(front_leg_1.local_matrix, glm::vec3(1.f / 8.f, 1.f / 2.f, 1.f / 8.f));
	front_leg_2.propagated_matrix = glm::translate(front_leg_2.propagated_matrix, glm::vec3(0.f, -1.f / 4.f, 0.f));
	front_leg_2.local_matrix = glm::scale(front_leg_2.local_matrix, glm::vec3(1.f / 8.f, 1.f / 2.f, 1.f / 8.f));
	back_leg_1.propagated_matrix = glm::translate(back_leg_1.propagated_matrix, glm::vec3(0.f, -1.f / 4.f, 0.f));
	back_leg_1.local_matrix = glm::scale(back_leg_1.local_matrix, glm::vec3(1.f / 8.f, 1.f / 2.f, 1.f / 8.f));
	back_leg_2.propagated_matrix = glm::translate(back_leg_2.propagated_matrix, glm::vec3(0.f, -1.f / 4.f, 0.f));
	back_leg_2.local_matrix = glm::scale(back_leg_2.local_matrix, glm::vec3(1.f / 8.f, 1.f / 2.f, 1.f / 8.f));

	neck.propagated_matrix = glm::translate(neck.propagated_matrix, glm::vec3(0.f, 0.f, -10.5f / 16.f));
	tailbase.propagated_matrix = glm::translate(tailbase.propagated_matrix, glm::vec3(0.f, 1.f / 16.f, 4.5f / 16.f));
	shoulder_1.propagated_matrix = glm::translate(shoulder_1.propagated_matrix, glm::vec3(-1.5f / 16.f, -3.f / 16.f, -9.5f / 16.f));
	shoulder_2.propagated_matrix = glm::translate(shoulder_2.propagated_matrix, glm::vec3(1.5f / 16.f, -3.f / 16.f, -9.5f / 16.f));
	hip_1.propagated_matrix = glm::translate(hip_1.propagated_matrix, glm::vec3(-1.5f / 16.f, -3.f / 16.f, 1.5f / 16.f));
	hip_2.propagated_matrix = glm::translate(hip_2.propagated_matrix, glm::vec3(1.5f / 16.f, -3.f / 16.f, 1.5f / 16.f));

	body.propagated_matrix = glm::translate(body.propagated_matrix, position);
	body.local_matrix = glm::scale(body.local_matrix, glm::vec3(3.f / 8.f, 3.f / 8.f, 9.f / 16.f));

	shoulder_1_initial = shoulder_1.propagated_matrix;
	shoulder_2_initial = shoulder_2.propagated_matrix;
	hip_1_initial = hip_1.propagated_matrix;
	hip_2_initial = hip_2.propagated_matrix;

	move(this->position);
}

// Opérateur égal

Wolf& Wolf::operator=(const Wolf& other)
{
	Mob::operator=(other);

	muzzle = other.muzzle;
	ear_1 = other.ear_1;
	ear_2 = other.ear_2;
	head = other.head;
	topbody = other.topbody;
	tail = other.tail;
	front_leg_1 = other.front_leg_1;
	front_leg_2 = other.front_leg_2;
	back_leg_1 = other.back_leg_1;
	back_leg_2 = other.back_leg_2;
	tailbase = other.tailbase;
	shoulder_1 = other.shoulder_1;
	shoulder_2 = other.shoulder_2;
	hip_1 = other.hip_1;
	hip_2 = other.hip_2;
	shoulder_1_initial = other.shoulder_1_initial;
	shoulder_2_initial = other.shoulder_2_initial;
	hip_1_initial = other.hip_1_initial;
	hip_2_initial = other.hip_2_initial;

	shoulder_1.children = { &front_leg_1 };
	shoulder_2.children = { &front_leg_2 };
	hip_1.children = { &back_leg_1 };
	hip_2.children = { &back_leg_2 };
	head.children = { &muzzle, &ear_1, &ear_2 };
	neck.children = { &head };
	tailbase.children = { &tail };
	body.children = { &topbody, &neck, &tailbase, &shoulder_1, &shoulder_2, &hip_1, &hip_2 };

	return *this;
}

// Anime le loup

void Wolf::animate(float time)
{
	shoulder_1.propagated_matrix = glm::rotate(shoulder_1_initial, sin(time), glm::vec3(1.f, 0.f, 0.f));
	shoulder_2.propagated_matrix = glm::rotate(shoulder_2_initial, -sin(time), glm::vec3(1.f, 0.f, 0.f));
	hip_1.propagated_matrix = glm::rotate(hip_1_initial, -sin(time), glm::vec3(1.f, 0.f, 0.f));
	hip_2.propagated_matrix = glm::rotate(hip_2_initial, sin(time), glm::vec3(1.f, 0.f, 0.f));
}