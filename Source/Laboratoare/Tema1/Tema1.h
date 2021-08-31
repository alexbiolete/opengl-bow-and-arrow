#pragma once

#include <Component/SimpleScene.h>
#include <TextRenderer/TextRenderer.h>

#define PI 3.14f

class Debug
{
public:
	GLboolean mouseRange, arrowSpeed;
};

class Gameplay
{
public:
	GLint lives, score;
	GLfloat speed;
	GLboolean shot, can_shoot, maximum_speed;
};

class Mouse
{
public:
	glm::vec2 position;
	glm::vec2 delta;
};

class Bow
{
public:
	glm::vec2 position;
	GLfloat rotation;
};

class Arrow
{
public:
	glm::vec2 size, position;
	GLfloat rotation, speed;
	GLboolean outOfMap;
};

class Balloon
{
public:
	GLboolean CheckCollision(Arrow& arrow);

	GLfloat size = 1.0f, circleRadius, triangleSideLength, stringLineLength;
	glm::vec2 position;
	GLboolean shot = false;
};

class Shuriken
{
public:
	GLboolean CheckCollision(Arrow& arrow);

	GLfloat size = 1.0f, triangleSideLength, rotation = PI / 20;
	glm::vec2 position;
	GLboolean shot = false;
};

class Tema1 : public SimpleScene
{
public:
	Tema1();
	~Tema1();

	void Init() override;

private:
	void FrameStart() override;
	void Update(float deltaTimeSeconds) override;
	void FrameEnd() override;

	void OnInputUpdate(float deltaTime, int mods) override;
	void OnKeyPress(int key, int mods) override;
	void OnKeyRelease(int key, int mods) override;
	void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
	void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
	void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
	void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;

	// Draw the hud
	void drawHud();

	// Window resolution
	glm::ivec2 resolution;

	// Draw primitives mode
	GLenum polygonMode;

	// The text renderer
	TextRenderer* Text;

protected:
	GLboolean intersects(Shuriken shuriken, Bow bow);
	GLboolean intersects(Arrow arrow, Shuriken shuriken);
	GLboolean intersects(Arrow arrow, Balloon balloon);

	glm::mat3 modelMatrix, visMatrix;

	glm::vec3 color_white, color_blue, color_purple, color_red, color_yellow, color_orange, color_green, color_lightblue, color_gray, color_black;
	GLboolean yellow_balloon_trigger1, yellow_balloon_trigger2, red_balloon_trigger1, red_balloon_trigger2, shuriken_trigger1, shuriken_trigger2;
	GLboolean rotation_trigger1, rotation_trigger2;
	GLboolean life_trigger1, life_trigger2;
};
