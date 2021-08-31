#include "Tema1.h"

#include <vector>
#include <iostream>
#include <cstdlib>
#include <ctime>

#include <Core/Engine.h>
#include "Transform2D.h"
#include "Object2D.h"

/*
	BIOLETE Alexandru-Eusebiu - Grupa 334CC

	Tema 1 - Bow and Arrow
*/

// Physics-related values
#define BOW_VERTICAL_MOVEMENT_SPEED				250

// Object-related values
#define BOW_SEMICIRCLE_RADIUS					100
#define BOW_VERTICAL_LIMIT_OFFSET				175
#define ARROW_TRAJECTORY_LINE_LENGTH			2000
#define ARROW_SIZE_HEIGHT						150
#define ARROW_SIZE_LENGTH						1
#define ARROW_TIP_SIDE_LENGTH					10
#define ARROW_MIN_SPEED							500
#define ARROW_MAX_SPEED							2000
#define BALLOON_COUNT							9
#define BALLOON_CIRCLE_RADIUS					75
#define BALLOON_VERTICAL_SIZE					0.675f
#define BALLOON_HORIZONTAL_SIZE					0.5f
#define BALLOON_TRIANGLE_SIDE_LENGTH			40
#define BALLOON_STRING_LINE_LENGTH				40 
#define BALLOON_HITBOX_SIZE						75
#define SHURIKEN_COUNT							3
#define SHURIKEN_TRIANGLE_SIDE_LENGTH			37.5f
#define SHURIKEN_HITBOX_SIZE					75

// Gameplay-related values
#define STARTING_LIFE_COUNT						3
#define SCORE_MULTIPLIER						1

// HUD-related values
#define HUD_BAR_X						150
#define HUD_BAR_Y						25
#define HUD_INDICATOR_RADIUS			15

using namespace std;

// Time-related variables
clock_t this_time = clock();
clock_t last_time = this_time;
GLfloat time_counter1 = 0, time_counter2 = 0, continuous_counter = 0;

// Object-related variables
Bow bow;
Arrow arrow;
Balloon balloon[BALLOON_COUNT];
Shuriken shuriken[SHURIKEN_COUNT];
vector<Balloon> balloons;

// Gameplay-related variables
Gameplay game;
Mouse mouse;

// Testing variables
Debug debug;

Tema1::Tema1()
{
}

Tema1::~Tema1()
{
}

void Tema1::Init()
{
	resolution = window->GetResolution();
	auto camera = GetSceneCamera();
	camera->SetOrthographic(0, (float)resolution.x, 0, (float)resolution.y, 0.01f, 400);
	camera->SetPosition(glm::vec3(0, 0, 50));
	camera->SetRotation(glm::vec3(0, 0, 0));
	camera->Update();
	GetCameraInput()->SetActive(false);

	// Cursor visibility
	ShowCursor(true);

	// Polygon mode
	polygonMode = GL_FILL;

	// Text renderer
	Text = new TextRenderer(resolution.x, resolution.y);
	Text->Load("Source/TextRenderer/Fonts/NovaMono-Regular.ttf", 24);

	glm::vec3 corner = glm::vec3(0.001, 0.001, 0);

	// Color values
	{
		color_white = glm::vec3(1, 1, 1);
		color_blue = glm::vec3(0, 0, 1);
		color_purple = glm::vec3(1, 0, 1);
		color_red = glm::vec3(1, 0, 0);
		color_yellow = glm::vec3(1, 1, 0);
		color_orange = glm::vec3(1, 0.5f, 0);
		color_green = glm::vec3(0, 1, 0);
		color_lightblue = glm::vec3(0, 0.5f, 1);
		color_gray = glm::vec3(0.5f, 0.5f, 0.5f);
		color_black = glm::vec3(0, 0, 0);
	}

	// Initialization of triggers for special platform effect activation
	{
		yellow_balloon_trigger1 = false;
		yellow_balloon_trigger2 = true;
		red_balloon_trigger1 = false;
		red_balloon_trigger2 = true;
		shuriken_trigger1 = false;
		shuriken_trigger2 = true;
		rotation_trigger1 = false;
		rotation_trigger2 = true;
		life_trigger1 = false;
		life_trigger2 = true;
	}

	// game initialization
	{
		game.lives = 3;
		game.score = 0;
		game.speed = 1.0f;
		game.maximum_speed = false;
		game.shot = false;
		game.can_shoot = true;
	}

	// HUD
	{
		// HUD bar frame mesh
		Mesh* staticBar = Object2D::CreateRectangle("staticBar", corner, HUD_BAR_X, HUD_BAR_Y, color_white, false);
		AddMeshToList(staticBar);
		// Power bar mesh
		Mesh* powerBar = Object2D::CreateRectangle("powerBar", corner, HUD_BAR_X, HUD_BAR_Y, color_lightblue, true);
		AddMeshToList(powerBar);
		// Power bar overload mesh
		Mesh* powerBarOverload = Object2D::CreateRectangle("powerBarOverload", corner, HUD_BAR_X, HUD_BAR_Y, color_red, true);
		AddMeshToList(powerBarOverload);
		// Life indicator mesh
		Mesh* life = Object2D::CreateCircle("life", corner, HUD_INDICATOR_RADIUS, color_red, true);
		AddMeshToList(life);
	}

	// Bow
	{
		// Mesh
		Mesh* bowSemicircle = Object2D::CreateSemicircle("bowSemicircle", corner, BOW_SEMICIRCLE_RADIUS, glm::vec3(0.5f, 0.25f, 0));
		AddMeshToList(bowSemicircle);

		// Initialization
		{
			bow.position.x = resolution.x / 12;
			bow.position.y = resolution.y / 2;
			bow.rotation = 3 * PI / 2;
		}
	}

	// Arrow
	{
		// Arrow trajectory line mesh
		Mesh* bowDirectionLine = Object2D::CreateLine("arrowTrajectoryLine", corner, ARROW_TRAJECTORY_LINE_LENGTH, glm::vec3(0.125f, 0.125f, 0.25f));
		AddMeshToList(bowDirectionLine);
		// Arrow shaft mesh
		Mesh* arrowRectangle = Object2D::CreateRectangle("arrowRectangle", corner, ARROW_SIZE_HEIGHT, ARROW_SIZE_LENGTH, glm::vec3(0.5f, 0.25f, 0));
		AddMeshToList(arrowRectangle);
		// Arrow tip mesh
		Mesh* arrowTriangle = Object2D::CreateTriangle("arrowTriangle", corner, ARROW_TIP_SIDE_LENGTH, glm::vec3(0.5f, 0.5f, 0.5f), 1);
		AddMeshToList(arrowTriangle);

		// Initialization
		{
			arrow.size.x = 150;
			arrow.size.y = 1;
			arrow.position.x = -300;
			arrow.outOfMap = false;
		}
	}


	// Balloons
	{
		// Red balloon mesh
		Mesh* redBalloonCircle = Object2D::CreateCircle("redBalloonCircle", corner, BALLOON_CIRCLE_RADIUS, color_red, 1);
		AddMeshToList(redBalloonCircle);
		// Red balloon excess mesh
		Mesh* redBalloonTriangle = Object2D::CreateTriangle("redBalloonTriangle", corner, BALLOON_TRIANGLE_SIDE_LENGTH, color_red, 1);
		AddMeshToList(redBalloonTriangle);
		// Yellow balloon mesh
		Mesh* yellowBalloonCircle = Object2D::CreateCircle("yellowBalloonCircle", corner, BALLOON_CIRCLE_RADIUS, color_yellow, 1);
		AddMeshToList(yellowBalloonCircle);
		// Yellow balloon excess mesh
		Mesh* yellowBalloonTriangle = Object2D::CreateTriangle("yellowBalloonTriangle", corner, BALLOON_TRIANGLE_SIDE_LENGTH, color_yellow, 1);
		AddMeshToList(yellowBalloonTriangle);
		// Balloon string parts mesh
		Mesh* balloonStringLine = Object2D::CreateLine("balloonStringLine", corner, BALLOON_STRING_LINE_LENGTH, color_white);
		AddMeshToList(balloonStringLine);

		// Initialization
		for (int i = 0; i < BALLOON_COUNT; i++) {
			balloon[i].circleRadius = BALLOON_CIRCLE_RADIUS;
			balloon[i].triangleSideLength = BALLOON_TRIANGLE_SIDE_LENGTH;
			balloon[i].stringLineLength = BALLOON_STRING_LINE_LENGTH;

			srand(time(NULL) + i);
			balloon[i].position.x = resolution.x / 2 + 100 * (rand() % 6);
			balloon[i].position.y = -100 * (rand() % 6) * i;
		}
	}

	// Shurikens
	{
		// Mesh
		Mesh* shurikenTriangle = Object2D::CreateTriangle("shurikenTriangle", corner, SHURIKEN_TRIANGLE_SIDE_LENGTH, glm::vec3(0.75f, 0.75f, 0.75f), 1);
		AddMeshToList(shurikenTriangle);

		// Initialization
		for (int i = 0; i < SHURIKEN_COUNT; i++) {
			shuriken[i].triangleSideLength = SHURIKEN_TRIANGLE_SIDE_LENGTH;

			srand(time(NULL) + i);
			shuriken[i].position.x = resolution.x + 100 * (rand() % 6);
			shuriken[i].position.y = 200 * (1 + rand() % 5);
		}
	}
}

void Tema1::drawHud()
{
	// Power bar
	if (game.lives >= 0) {
		modelMatrix = glm::mat3(1);

		modelMatrix *= Transform2D::Translate(bow.position.x - 25, bow.position.y - 150);

		RenderMesh2D(meshes["staticBar"], shaders["VertexColor"], modelMatrix);

		if (!arrow.outOfMap && arrow.speed > ARROW_MIN_SPEED && arrow.speed < ARROW_MAX_SPEED) {
			modelMatrix = glm::mat3(1);

			modelMatrix *= Transform2D::Translate(bow.position.x - 25, bow.position.y - 150);
			modelMatrix *= Transform2D::Scale((arrow.speed - ARROW_MIN_SPEED) * 0.000665, 0.99f);

			RenderMesh2D(meshes["powerBar"], shaders["VertexColor"], modelMatrix);
		}
		else if (!arrow.outOfMap && arrow.speed == ARROW_MAX_SPEED) {
			modelMatrix = glm::mat3(1);

			modelMatrix *= Transform2D::Translate(bow.position.x - 25, bow.position.y - 150);
			modelMatrix *= Transform2D::Scale(1, 0.99f);

			RenderMesh2D(meshes["powerBarOverload"], shaders["VertexColor"], modelMatrix);
		}
	}

	// Life count indicators
	{
		for (int i = 0; i < game.lives; i++) {
			modelMatrix = glm::mat3(1);

			modelMatrix *= Transform2D::Translate(resolution.x - 150 + i * 40, resolution.y - 50);

			RenderMesh2D(meshes["life"], shaders["VertexColor"], modelMatrix);
		}
	}

	// Text
	{
		if (game.lives >= 0) {
			Text->RenderTextNumber("Score: ", game.score, resolution.x / 30, resolution.y / 20, 1.0f, color_white);

			if (!game.shot && !game.can_shoot)
				Text->RenderText("Reloading...", resolution.x / 2.2f, resolution.y / 20, 1.0f, color_white);
			if (game.maximum_speed)
				Text->RenderText("Maximum speed reached!", resolution.x / 2.5f, resolution.y - 50, 1.0f, color_red);
		}
		else {
			Text->RenderTextNumber("Final score: ", game.score, resolution.x / 2.5f, resolution.y / 20, 1.5f, color_white);
			Text->RenderText("GAME OVER!", resolution.x / 2.25f, resolution.y / 2.25f, 1.25f, color_white);
			Text->RenderText("Press ENTER to start a new game.", resolution.x / 2.85f, resolution.y / 2, 1.0f, color_white);
		}
	}
}

void Tema1::FrameStart()
{
	// Clears the color buffer (using the previously set color) and depth buffer
	glClearColor(0.0675f, 0.0675f, 0.125f, 1);
	// glClearColor(0, 0.25f, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::ivec2 resolution = window->GetResolution();
	// sets the screen area where to draw
	glViewport(0, 0, resolution.x, resolution.y);
}

void Tema1::Update(float deltaTimeSeconds)
{
	glLineWidth(3);
	glPointSize(5);
	glPolygonMode(GL_FRONT_AND_BACK, polygonMode);

	if (game.lives >= 0) {
		// Real-time related tasks
		// https://stackoverflow.com/questions/10807681/loop-every-10-second
		{
			this_time = clock();
			time_counter1 += (GLdouble)(this_time - last_time);
			time_counter2 += (GLdouble)(this_time - last_time);
			continuous_counter += (GLdouble)(this_time - last_time);
			last_time = this_time;

			if (time_counter1 > (GLdouble)(1 * CLOCKS_PER_SEC)) {
				yellow_balloon_trigger2 = true;
				red_balloon_trigger2 = true;
				shuriken_trigger2 = true;
				rotation_trigger2 = true;
				life_trigger2 = true;

				time_counter1 -= (GLdouble)(1 * CLOCKS_PER_SEC);
			}

			if (time_counter2 > (GLdouble)(3 * CLOCKS_PER_SEC)) {
				game.can_shoot = true;

				time_counter2 -= (GLdouble)(3 * CLOCKS_PER_SEC);
			}

			if (continuous_counter > (GLdouble)(30 * CLOCKS_PER_SEC)) {
				game.maximum_speed = true;

				continuous_counter -= (GLdouble)(30 * CLOCKS_PER_SEC);
			}
		}

		if (!game.maximum_speed) {
			game.speed += 0.0000001f * continuous_counter;
		}

		// Starting radius: 4.7125f
		// Targeting area (1 rad): [4.2125f, 5.2125f]

		// Upper limit: Starting radius + 0.5
		// Lower limit: Starting radius - 0.5f

		// (tan^(-1))(Y/base)
		// https://stackoverflow.com/questions/3344717/opengl-how-to-calculate-the-angle-where-my-mouse-is-pointing-at-in-the-3d-space
		if (bow.rotation > 4 && bow.rotation < 5.5f) {
			if (debug.mouseRange && bow.rotation > 4.0001f && bow.rotation < 5.4999f) {
				printf("[FOV] In correct range. Ready to shoot!\n");
				debug.mouseRange = false;
			}
			// Unstable method when there is no mouse movement
			bow.rotation -= 0.125f * deltaTimeSeconds * mouse.delta.y;
			/* Alternative method which is not working properly while moving the model vertically simultaneously
			// and is also glitching when reaching the FOV limits.

			 bow.rotation = PI + pow(tan(mouse.position.y / bow.position.y), -1);
			*/
		// Upper limit reset point: Upper limit - 0.0001f
		} 
		else if (bow.rotation > 5.5f && rotation_trigger2) {
			printf("[FOV] Reached upper limit!\n");
		
			debug.mouseRange = true;
			bow.rotation = 5.4999f;
		// Lower limit reset point: Lower limit + 0.0001f
		}
		else if (bow.rotation < 4 && rotation_trigger2) {
			printf("[FOV] Reached lower limit!\n");

			debug.mouseRange = true;
			bow.rotation = 4.0001f;
		}

		glm::ivec2 resolution = window->GetResolution();

		// Bow
		{
			modelMatrix = glm::mat3(1);

			modelMatrix *= Transform2D::Translate(bow.position.x, bow.position.y);
			modelMatrix *= Transform2D::Rotate(bow.rotation);

			RenderMesh2D(meshes["bowSemicircle"], shaders["VertexColor"], modelMatrix);

			/* Bow trajectory line
				modelMatrix *= Transform2D::Translate(0, -200);
				modelMatrix *= Transform2D::Rotate(1.57f);

				RenderMesh2D(meshes["arrowTrajectoryLine"], shaders["VertexColor"], modelMatrix);
			*/
		}

		if (game.shot && arrow.speed < ARROW_MAX_SPEED) {
			arrow.position.x = bow.position.x;
			arrow.position.y = bow.position.y;
			arrow.rotation = bow.rotation;
			arrow.speed += 10;
			debug.arrowSpeed = true;
		}
		else {
			// https://gamedev.stackexchange.com/questions/86364/calculating-projectile-movement
			if (debug.arrowSpeed) {
				printf("[ARROW] Power: %f\n", arrow.speed);

				debug.arrowSpeed = false;
			}

			arrow.position.x += deltaTimeSeconds * arrow.speed * -sin(arrow.rotation);
			arrow.position.y += deltaTimeSeconds * arrow.speed * cos(arrow.rotation);
		}

		if (arrow.position.x > resolution.x) {
			arrow.outOfMap = true;
		}
		else {
			arrow.outOfMap = false;
		}

		// Arrow
		{
			modelMatrix = glm::mat3(1);

			modelMatrix *= Transform2D::Translate(arrow.position.x, arrow.position.y);
			modelMatrix *= Transform2D::Rotate(PI / 2 + arrow.rotation);
			RenderMesh2D(meshes["arrowRectangle"], shaders["VertexColor"], modelMatrix);

			modelMatrix *= Transform2D::Translate(150, -5);
			modelMatrix *= Transform2D::Rotate(PI / 4);
			RenderMesh2D(meshes["arrowTriangle"], shaders["VertexColor"], modelMatrix);
		}

		// Yellow balloons
		for (int i = 0; i < BALLOON_COUNT / 2; i++)
		{
			// Generation algorithm
			balloon[i].position.y += deltaTimeSeconds * 100.0f * game.speed;

			if (balloon[i].position.y > resolution.y + 100) {
				balloon[i].size = 1.0f;
				balloon[i].shot = false;
				srand(time(NULL) + i);
				balloon[i].position.x = resolution.x / 2 + (rand() % 6) * 100;
				balloon[i].position.y = -(rand() % 1000) * i;
			}

			// Model
			{
				modelMatrix = glm::mat3(1);

				modelMatrix *= Transform2D::Translate(balloon[i].position.x, balloon[i].position.y);

				if (intersects(arrow, balloon[i]) && yellow_balloon_trigger2) {
					balloon[i].shot = true;
					yellow_balloon_trigger1 = true;
				}

				if (balloon[i].shot) {
					if (balloon[i].size > 0)
						balloon[i].size -= 0.1f;

					modelMatrix *= Transform2D::Scale(balloon[i].size, 0);
				}

				if (!balloon[i].shot && yellow_balloon_trigger1 && game.score >= SCORE_MULTIPLIER) {
					game.score -= SCORE_MULTIPLIER;

					printf("Score: %d\n", game.score);

					yellow_balloon_trigger1 = false;
					yellow_balloon_trigger2 = false;
				}

				modelMatrix *= Transform2D::Scale(BALLOON_HORIZONTAL_SIZE, BALLOON_VERTICAL_SIZE);
				RenderMesh2D(meshes["yellowBalloonCircle"], shaders["VertexColor"], modelMatrix);

				modelMatrix *= Transform2D::Translate(10, -87.5f);
				modelMatrix *= Transform2D::Scale(0.5f, 0.5f);
				modelMatrix *= Transform2D::Rotate(2.375f);
				RenderMesh2D(meshes["yellowBalloonTriangle"], shaders["VertexColor"], modelMatrix);

				modelMatrix *= Transform2D::Translate(20, 10);
				modelMatrix *= Transform2D::Rotate(1.35f);
				RenderMesh2D(meshes["balloonStringLine"], shaders["VertexColor"], modelMatrix);

				modelMatrix *= Transform2D::Translate(40, 0);
				modelMatrix *= Transform2D::Rotate(2.25f);
				RenderMesh2D(meshes["balloonStringLine"], shaders["VertexColor"], modelMatrix);

				modelMatrix *= Transform2D::Translate(40, 0);
				modelMatrix *= Transform2D::Rotate(4.05f);
				RenderMesh2D(meshes["balloonStringLine"], shaders["VertexColor"], modelMatrix);

				modelMatrix *= Transform2D::Translate(40, 0);
				modelMatrix *= Transform2D::Rotate(8.5f);
				RenderMesh2D(meshes["balloonStringLine"], shaders["VertexColor"], modelMatrix);
			}
		}

		// Red balloon
		for (int i = BALLOON_COUNT / 2; i < BALLOON_COUNT; i++)
		{
			// Generation algorithm
			balloon[i].position.y += deltaTimeSeconds * 100.0f * game.speed;

			if (balloon[i].position.y > resolution.y + 100) {
				balloon[i].size = 1.0f;
				balloon[i].shot = false;
				srand(time(NULL) + i);
				balloon[i].position.x = resolution.x / 2 + (rand() % 6) * 100;
				balloon[i].position.y = -(rand() % 6) * i * 100;
			}

			// Model
			{
				modelMatrix = glm::mat3(1);

				modelMatrix *= Transform2D::Translate(balloon[i].position.x, balloon[i].position.y);

				if (intersects(arrow, balloon[i]) && red_balloon_trigger2 && !balloon[i].shot) {
					balloon[i].shot = true;
					red_balloon_trigger1 = true;
				}

				if (balloon[i].shot) {
					if (balloon[i].size > 0)
						balloon[i].size -= 0.1f;

					modelMatrix *= Transform2D::Scale(balloon[i].size, 0);
				}

				if (!balloon[i].shot && red_balloon_trigger1) {
					game.score += SCORE_MULTIPLIER;

					printf("Score: %d\n", game.score);

					red_balloon_trigger1 = false;
					red_balloon_trigger2 = false;
				}

				modelMatrix *= Transform2D::Scale(BALLOON_HORIZONTAL_SIZE, BALLOON_VERTICAL_SIZE);
				RenderMesh2D(meshes["redBalloonCircle"], shaders["VertexColor"], modelMatrix);

				modelMatrix *= Transform2D::Translate(10, -87.5f);
				modelMatrix *= Transform2D::Scale(0.5f, 0.5f);
				modelMatrix *= Transform2D::Rotate(2.375f);
				RenderMesh2D(meshes["redBalloonTriangle"], shaders["VertexColor"], modelMatrix);

				modelMatrix *= Transform2D::Translate(20, 10);
				modelMatrix *= Transform2D::Rotate(1.35f);
				RenderMesh2D(meshes["balloonStringLine"], shaders["VertexColor"], modelMatrix);

				modelMatrix *= Transform2D::Translate(40, 0);
				modelMatrix *= Transform2D::Rotate(2.25f);
				RenderMesh2D(meshes["balloonStringLine"], shaders["VertexColor"], modelMatrix);

				modelMatrix *= Transform2D::Translate(40, 0);
				modelMatrix *= Transform2D::Rotate(4.05f);
				RenderMesh2D(meshes["balloonStringLine"], shaders["VertexColor"], modelMatrix);

				modelMatrix *= Transform2D::Translate(40, 0);
				modelMatrix *= Transform2D::Rotate(8.5f);
				RenderMesh2D(meshes["balloonStringLine"], shaders["VertexColor"], modelMatrix);
			}
		}

		// Shuriken
		for (int i = 0; i < SHURIKEN_COUNT; i++)
		{
			// Generation algorithm
			shuriken[i].position.x -= deltaTimeSeconds * 200.0f * game.speed;
			shuriken[i].rotation += deltaTimeSeconds * 10.0f * game.speed;

			if (shuriken[i].position.x < 0) {
				shuriken[i].size = 1.0f;
				shuriken[i].shot = false;
				srand(time(NULL) + i);
				shuriken[i].position.x = resolution.x + 100 * rand() % 6;
				shuriken[i].position.y = 200 * (1 + rand() % 5);
			}

			if (intersects(shuriken[i], bow) && life_trigger2 && !shuriken[i].shot) {
				shuriken[i].shot = true;
				life_trigger1 = true;
			}

			if (!shuriken[i].shot && life_trigger1 && shuriken[i].position.x <= resolution.x) {
				if (game.score >= SCORE_MULTIPLIER)
					game.score -= SCORE_MULTIPLIER;
			
				game.lives--;

				printf("Score: %d\n", game.score);

				life_trigger1 = false;
				life_trigger2 = false;
			}

			// Model
			{
				modelMatrix = glm::mat3(1);

				modelMatrix *= Transform2D::Translate(shuriken[i].position.x, shuriken[i].position.y);

				if (intersects(arrow, shuriken[i]) && shuriken_trigger2) {
					shuriken[i].shot = true;
					shuriken_trigger1 = true;
				}

				if (shuriken[i].shot) {
					if (shuriken[i].size > 0)
						shuriken[i].size -= 0.1f;

					modelMatrix *= Transform2D::Scale(shuriken[i].size, shuriken[i].size);
				}

				if (shuriken_trigger1) {
					game.score += SCORE_MULTIPLIER;

					printf("Score: %d\n", game.score);

					shuriken_trigger1 = false;
					shuriken_trigger2 = false;
				}

				modelMatrix *= Transform2D::Rotate(shuriken[i].rotation);
				RenderMesh2D(meshes["shurikenTriangle"], shaders["VertexColor"], modelMatrix);

				modelMatrix *= Transform2D::Rotate(PI / 2);
				RenderMesh2D(meshes["shurikenTriangle"], shaders["VertexColor"], modelMatrix);

				modelMatrix *= Transform2D::Rotate(PI / 2);
				RenderMesh2D(meshes["shurikenTriangle"], shaders["VertexColor"], modelMatrix);

				modelMatrix *= Transform2D::Rotate(PI / 2);
				RenderMesh2D(meshes["shurikenTriangle"], shaders["VertexColor"], modelMatrix);
			}
		}
	}
}

void Tema1::FrameEnd()
{
	drawHud();
	// DrawCoordinatSystem();
}

void Tema1::OnInputUpdate(float deltaTime, int mods)
{
	if (window->KeyHold(GLFW_KEY_W) && bow.position.y < resolution.y - BOW_VERTICAL_LIMIT_OFFSET)
	{
		bow.position.y += BOW_VERTICAL_MOVEMENT_SPEED * deltaTime;
	}

	if (window->KeyHold(GLFW_KEY_S) && bow.position.y > BOW_VERTICAL_LIMIT_OFFSET)
	{
		bow.position.y -= BOW_VERTICAL_MOVEMENT_SPEED * deltaTime;
	}
}

void Tema1::OnKeyPress(int key, int mods)
{
	// add key press event
	if (key == GLFW_KEY_ENTER && game.lives <= 0) {
		Tema1::Init();
	}
}

void Tema1::OnKeyRelease(int key, int mods)
{
	// add key release event
}

void Tema1::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
	mouse.position.y = mouseY;
	mouse.delta.y = deltaY;
}

void Tema1::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button press event
	if (button && game.can_shoot) {
		printf("[MOUSE] Button pressed.\n");

		arrow.speed = ARROW_MIN_SPEED;
		arrow.position.x = 200;
		game.shot = true;
		game.can_shoot = false;
	}
}

void Tema1::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button release event
	if (button) {
		printf("[MOUSE] Button released.\n");

		game.shot = 0;
	}
}

void Tema1::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}

// bool Tema1::CheckCollision(Arrow &arrow, Shuriken &shuriken)
// {
// 	bool collisionX = arrow.position.x + arrow.size.x >= shuriken.position.x && shuriken.position.x + shuriken.triangleSideLength >= arrow.position.x;
// 	bool collisionY = arrow.position.y + arrow.size.y >= shuriken.position.y && shuriken.position.x + shuriken.triangleSideLength >= arrow.position.y;

// 	return collisionX && collisionY;
// }

GLboolean Tema1::intersects(Shuriken shuriken, Bow bow)
{
	// get center point circle first
	glm::vec2 center(shuriken.position + shuriken.triangleSideLength);
	// calculate AABB info (center, half-extents)
	glm::vec2 aabb_half_extents(BOW_SEMICIRCLE_RADIUS, BOW_SEMICIRCLE_RADIUS);
	glm::vec2 aabb_center(
		bow.position.x + aabb_half_extents.x,
		bow.position.y + aabb_half_extents.y
	);
	// get difference                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       
	// vector between both centers
	glm::vec2 difference = center - aabb_center;
	glm::vec2 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);
	// add clamped value to AABB_center and we get the value of box closest to circle
	glm::vec2 closest = aabb_center + clamped;
	// retrieve vector between center circle and closest point AABB and check if length <= radius
	difference = closest - center;
	return glm::length(difference) < shuriken.triangleSideLength;
}

GLboolean Tema1::intersects(Arrow arrow, Shuriken shuriken)
{
	// get center point circle first
	glm::vec2 center(arrow.position + arrow.size.x);
	// calculate AABB info (center, half-extents)
	//glm::vec2 aabb_half_extents(shuriken.triangleSideLength / 2.0f, shuriken.triangleSideLength / 2.0f);
	glm::vec2 aabb_half_extents(SHURIKEN_HITBOX_SIZE, SHURIKEN_HITBOX_SIZE);
	glm::vec2 aabb_center(
		shuriken.position.x + aabb_half_extents.x,
		shuriken.position.y + aabb_half_extents.y
	);
	// get difference                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       
	// vector between both centers
	glm::vec2 difference = center - aabb_center;
	glm::vec2 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);
	// add clamped value to AABB_center and we get the value of box closest to circle
	glm::vec2 closest = aabb_center + clamped;
	// retrieve vector between center circle and closest point AABB and check if length <= radius
	difference = closest - center;
	return glm::length(difference) < arrow.size.x;
}


GLboolean Tema1::intersects(Arrow arrow, Balloon balloon)
{
	// get center point circle first
	glm::vec2 center(arrow.position + arrow.size.x);
	// calculate AABB info (center, half-extents)
	//glm::vec2 aabb_half_extents(BALLOON_HORIZONTAL_SIZE / 2.0f, BALLOON_VERTICAL_SIZE / 2.0f);
	glm::vec2 aabb_half_extents(BALLOON_HITBOX_SIZE, BALLOON_HITBOX_SIZE);
	glm::vec2 aabb_center(
		balloon.position.x + aabb_half_extents.x,
		balloon.position.y + aabb_half_extents.y
	);
	// get difference                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       
	// vector between both centers
	glm::vec2 difference = center - aabb_center;
	glm::vec2 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);
	// add clamped value to AABB_center and we get the value of box closest to circle
	glm::vec2 closest = aabb_center + clamped;
	// retrieve vector between center circle and closest point AABB and check if length <= radius
	difference = closest - center;
	return glm::length(difference) < arrow.size.x;
}
