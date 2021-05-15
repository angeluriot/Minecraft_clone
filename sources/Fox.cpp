#include "Fox.h"

// Crée un renard

Fox::Fox() : Mob()
{
	shoulder_1_initial = glm::mat4(1.f);
	shoulder_2_initial = glm::mat4(1.f);
	hips_1_initial = glm::mat4(1.f);
	hips_2_initial = glm::mat4(1.f);
	head_initial = glm::mat4(1.f);
	tail_initial = glm::mat4(1.f);
}

// Crée un renard à partir d'un autre

Fox::Fox(const Fox& other)
{
	*this = other;
}

// Crée un renard à partir de ces paramètres

Fox::Fox(const glm::vec3& position) : Mob()
{
	y_offset = 0.f / 16.f - 0.5f;
	texture = &Texture::fox;
	this->position = position;

	muzzle = Part(Mesh::cube, glm::vec2(28.f, 0.f), glm::vec3(4.f, 2.f, 3.f), {}, false);
	ear_1 = Part(Mesh::cube, glm::vec2(34.f, 5.f), glm::vec3(2.f, 2.f, 1.f), {}, false);
	ear_2 = Part(Mesh::cube, glm::vec2(28.f, 5.f), glm::vec3(2.f, 2.f, 1.f), {}, false);
	head = Part(Mesh::cube, glm::vec2(0.f, 0.f), glm::vec3(8.f, 6.f, 6.f), { &muzzle, &ear_1, &ear_2 }, false);

	tail = Part(Mesh::cube, glm::vec2(0.f, 31.f), glm::vec3(5.f, 4.f, 9.f), {}, false);

	leg = Part(Mesh::cube, glm::vec2(28.f, 8.f), glm::vec3(2.f, 4.f, 2.f), {}, false);
	
	neck = Part(Mesh::null, glm::vec2(), glm::vec3(), { &head }, false);
	tailbase = Part(Mesh::null, glm::vec2(), glm::vec3(), { &tail }, false);
	shoulder_1 = Part(Mesh::null, glm::vec2(), glm::vec3(), { &leg }, false);
	shoulder_2 = Part(Mesh::null, glm::vec2(), glm::vec3(), { &leg }, false);
	hips_1 = Part(Mesh::null, glm::vec2(), glm::vec3(), { &leg }, false);
	hips_2 = Part(Mesh::null, glm::vec2(), glm::vec3(), { &leg }, false);

	body = Part(Mesh::cube, glm::vec2(0.f, 14.f), glm::vec3(6.f, 6.f, 11.f), { &neck, &tailbase, &shoulder_1, &shoulder_2, &hips_1, &hips_2 }, false);

	// Positions

	muzzle.propagated_matrix = glm::translate(muzzle.propagated_matrix, glm::vec3(0.f, -2.f / 16.f, -4.5f / 16.f));
	ear_1.propagated_matrix = glm::translate(ear_1.propagated_matrix, glm::vec3(-3.f / 16.f, 4.f / 16.f, -1.5f / 16.f));
	ear_2.propagated_matrix = glm::translate(ear_2.propagated_matrix, glm::vec3(3.f / 16.f, 4.f / 16.f, -1.5f / 16.f));

	head.propagated_matrix = glm::translate(head.propagated_matrix, glm::vec3(0.f, 0.f, -3.f / 16.f));

	tail.propagated_matrix = glm::translate(tail.propagated_matrix, glm::vec3(0.f, 0.f, 4.5f / 16.f));
	leg.propagated_matrix = glm::translate(leg.propagated_matrix, glm::vec3(0.f, -2.5f / 16.f, 0.f));
	
	neck.propagated_matrix = glm::translate(neck.propagated_matrix, glm::vec3(0.f, -1.f / 16.f, -5.5f / 16.f));
	tailbase.propagated_matrix = glm::translate(tailbase.propagated_matrix, glm::vec3(0.f, 0.f, 5.5f / 16.f));
	shoulder_1.propagated_matrix = glm::translate(shoulder_1.propagated_matrix, glm::vec3(-2.f / 16.f, -3.f / 16.f, -4.5f / 16.f));
	shoulder_2.propagated_matrix = glm::translate(shoulder_2.propagated_matrix, glm::vec3(2.f / 16.f, -3.f / 16.f, -4.5f / 16.f));
	hips_1.propagated_matrix = glm::translate(hips_1.propagated_matrix, glm::vec3(-2.f / 16.f, -3.f / 16.f, 4.5f / 16.f));
	hips_2.propagated_matrix = glm::translate(hips_2.propagated_matrix, glm::vec3(2.f / 16.f, -3.f / 16.f, 4.5f / 16.f));

	body.propagated_matrix = glm::translate(body.propagated_matrix, position);

	// Tailles

	muzzle.local_matrix = glm::scale(muzzle.local_matrix, glm::vec3(4.f / 16.f, 2.f / 16.f, 3.f / 16.f));
	ear_1.local_matrix = glm::scale(ear_1.local_matrix, glm::vec3(2.f / 16.f, 2.f / 16.f, 1.f / 16.f));
	ear_2.local_matrix = glm::scale(ear_2.local_matrix, glm::vec3(2.f / 16.f, 2.f / 16.f, 1.f / 16.f));

	head.local_matrix = glm::scale(head.local_matrix, glm::vec3(8.f / 16.f, 6.f / 16.f, 6.f / 16.f));

	tail.local_matrix = glm::scale(tail.local_matrix, glm::vec3(5.f / 16.f, 4.f / 16.f, 9.f / 16.f));
	leg.local_matrix = glm::scale(leg.local_matrix, glm::vec3(2.f / 16.f, 5.f / 16.f, 2.f / 16.f));

	body.local_matrix = glm::scale(body.local_matrix, glm::vec3(6.f / 16.f, 6.f / 16.f, 11.f / 16.f));

	shoulder_1_initial = shoulder_1.propagated_matrix;
	shoulder_2_initial = shoulder_2.propagated_matrix;
	hips_1_initial = hips_1.propagated_matrix;
	hips_2_initial = hips_2.propagated_matrix;
	head_initial = head.propagated_matrix;
	tail_initial = tailbase.propagated_matrix;

	move(this->position);
}

// Opérateur égal

Fox& Fox::operator=(const Fox& other)
{
	Mob::operator=(other);

	muzzle = other.muzzle;
	ear_1 = other.ear_1;
	ear_2 = other.ear_2;
	head = other.head;
	tail = other.tail;
	leg = other.leg;
	tailbase = other.tailbase;
	shoulder_1 = other.shoulder_1;
	shoulder_2 = other.shoulder_2;
	hips_1 = other.hips_1;
	hips_2 = other.hips_2;
	shoulder_1_initial = other.shoulder_1_initial;
	shoulder_2_initial = other.shoulder_2_initial;
	hips_1_initial = other.hips_1_initial;
	hips_2_initial = other.hips_2_initial;
	head_initial = other.head_initial;
	tail_initial = other.tail_initial;

	shoulder_1.children = { &leg };
	shoulder_2.children = { &leg };
	hips_1.children = { &leg };
	hips_2.children = { &leg };
	head.children = { &muzzle, &ear_1, &ear_2 };
	neck.children = { &head };
	tailbase.children = { &tail };
	body.children = { &neck, &tailbase, &shoulder_1, &shoulder_2, &hips_1, &hips_2 };

	return *this;
}

// Anime le renard

void Fox::animate(float time)
{
	shoulder_1.propagated_matrix = glm::rotate(shoulder_1_initial, sin(2.f * time) / 3.f, glm::vec3(1.f, 0.f, 0.f));
	shoulder_2.propagated_matrix = glm::rotate(shoulder_2_initial, -sin(2.f * time) / 3.f, glm::vec3(1.f, 0.f, 0.f));
	hips_1.propagated_matrix = glm::rotate(hips_1_initial, -sin(2.f * time) / 3.f, glm::vec3(1.f, 0.f, 0.f));
	hips_2.propagated_matrix = glm::rotate(hips_2_initial, sin(2.f * time) / 3.f, glm::vec3(1.f, 0.f, 0.f));
	tailbase.propagated_matrix = glm::rotate(tail_initial, sin(2.f * time) / 4.f, glm::vec3(0.f, 1.f, 0.f));

	body.propagated_matrix = glm::translate(body_initial, sin(time * 3.f) / 100.f * glm::vec3(0.f, 1.f, 0.5f));
	body.propagated_matrix = glm::rotate(body.propagated_matrix, sin(2.f * time) / 20, glm::vec3(0.f, 0.f, 1.f));
	head.propagated_matrix = glm::translate(head_initial, sin(time * 2.f) / 80.f * glm::vec3(0.f, 1.f, 0.f));
	head.propagated_matrix = glm::rotate(head.propagated_matrix, -sin(2.f * time) / 20.f, glm::vec3(0.f, 0.f, 1.f));
}