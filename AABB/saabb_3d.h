#include "raylib.h"
#include "raymath.h"
#include <limits>
#include <algorithm>

void Init();
void Update();
void Render();
void DeInit();

Vector3 Vector3Up() { return { 0,1,0 }; }
Vector3 Vector3Down() { return { 0,-1,0 }; }
Vector3 Vector3Right() { return { 1,0,0 }; }
Vector3 Vector3Left() { return { -1,0,0 }; }
Vector3 Vector3Forward() { return { 0,0,1 }; }
Vector3 Vector3Backward() { return { 0,0,-1 }; }

struct Box
{
	Vector3 vel;
	BoundingBox bbox;
	Vector3 size;
};

float Sign(int a)
{
	return a >= 0 ? 1 : -1;
}

float SweptAABB(Box b1, Box b2, float& normalx, float& normaly, float& normalz)
{
	float xInvEntry, yInvEntry, zInvEntry;
	float xInvExit, yInvExit, zInvExit;

	// find the distance between the objects on the near and far sides for both x, y and z
	if (b1.vel.x > 0.0f)
	{
		xInvEntry = b2.bbox.min.x - (b1.bbox.min.x + b1.size.x);
		xInvExit = (b2.bbox.min.x + b2.size.x) - b1.bbox.min.x;
	}
	else
	{
		xInvEntry = (b2.bbox.min.x + b2.size.x) - b1.bbox.min.x;
		xInvExit = b2.bbox.min.x - (b1.bbox.min.x + b1.size.x);
	}

	if (b1.vel.y > 0.0f)
	{
		yInvEntry = b2.bbox.min.y - (b1.bbox.min.y + b1.size.y);
		yInvExit = (b2.bbox.min.y + b2.size.y) - b1.bbox.min.y;
	}
	else
	{
		yInvEntry = (b2.bbox.min.y + b2.size.y) - b1.bbox.min.y;
		yInvExit = b2.bbox.min.y - (b1.bbox.min.y + b1.size.y);
	}

	if (b1.vel.z > 0.0f)
	{
		zInvEntry = b2.bbox.min.z - (b1.bbox.min.z + b1.size.z);
		zInvExit = (b2.bbox.min.z + b2.size.z) - b1.bbox.min.z;
	}
	else
	{
		zInvEntry = (b2.bbox.min.z + b2.size.z) - b1.bbox.min.z;
		zInvExit = b2.bbox.min.z - (b1.bbox.min.z + b1.size.z);
	}

	// find time of collision and time of leaving for each axis (if statement is to prevent divide by zero) 
	float xEntry, yEntry, zEntry;
	float xExit, yExit, zExit;

	if (b1.vel.x == 0.0f)
	{
		xEntry = -std::numeric_limits<float>::infinity();
		xExit = std::numeric_limits<float>::infinity();
	}
	else
	{
		xEntry = xInvEntry / b1.vel.x;
		xExit = xInvExit / b1.vel.x;
	}

	if (b1.vel.y == 0.0f)
	{
		yEntry = -std::numeric_limits<float>::infinity();
		yExit = std::numeric_limits<float>::infinity();
	}
	else
	{
		yEntry = yInvEntry / b1.vel.y;
		yExit = yInvExit / b1.vel.y;
	}

	if (b1.vel.z == 0.0f)
	{
		zEntry = -std::numeric_limits<float>::infinity();
		zExit = std::numeric_limits<float>::infinity();
	}
	else
	{
		zEntry = zInvEntry / b1.vel.z;
		zExit = zInvExit / b1.vel.z;
	}

	// find the earliest/latest times of collisionfloat 
	float entryTime = std::max(xEntry, std::max(yEntry, zEntry));
	float exitTime = std::min(xExit, std::min(yExit, zExit));

	// if there was no collision
	if (entryTime > exitTime
		|| entryTime < 0
		|| xEntry > 1.0f
		|| yEntry > 1.0f
		|| zEntry > 1.0f)
	{
		normalx = 0.0f;
		normaly = 0.0f;
		normalz = 0.0f;
		return 1.0f;
	}

	// if there was a collision 
	else
	{
		// calculate normal of collided surface
		if (xEntry > yEntry) {
			if (xEntry > zEntry) {
				normalx = -Sign(b1.vel.x);
				normaly = 0.0f;
				normalz = 0.0f;
			}
			else {
				normalz = -Sign(b1.vel.z);
				normalx = 0.0f;
				normaly = 0.0f;
			}
		}
		else {
			if (yEntry > zEntry) {
				normaly = -Sign(b1.vel.y);
				normalx = 0.0f;
				normalz = 0.0f;
			}
			else {
				normalz = -Sign(b1.vel.z);
				normalx = 0.0f;
				normaly = 0.0f;
			}
		}
	}

	return entryTime;
}

BoundingBox GetSweptBroadphaseBox(Box b)
{
	Vector3 min, size;

	min.x = b.vel.x > 0 ? b.bbox.min.x : b.bbox.min.x + b.vel.x;
	min.y = b.vel.y > 0 ? b.bbox.min.y : b.bbox.min.y + b.vel.y;
	min.z = b.vel.z > 0 ? b.bbox.min.z : b.bbox.min.z + b.vel.z;

	size.x = b.vel.x > 0 ? b.vel.x + b.size.x : b.size.x - b.vel.x;
	size.y = b.vel.y > 0 ? b.vel.y + b.size.y : b.size.y - b.vel.y;
	size.z = b.vel.z > 0 ? b.vel.z + b.size.z : b.size.z - b.vel.z;

	BoundingBox broadphasebox;
	broadphasebox.min = min;
	broadphasebox.max = Vector3Add(min, size);

	return broadphasebox;
}

void MoveBoundingBox(BoundingBox& box, Vector3 d)
{
	box.min = Vector3Add(box.min, d);
	box.max = Vector3Add(box.max, d);
}

Box player;
Box obstacle;
BoundingBox broadphasebox;
const float playerSpeed = 5;

Camera cam;

int main(void)
{
	Init();

	while (!WindowShouldClose())
	{
		Update();
		Render();
	}

	DeInit();

	return 0;
}

void Init()
{
	// window
	const int screenWidth = 1280;
	const int screenHeight = 720;
	InitWindow(screenWidth, screenHeight, "Swept AABB Tests");
	SetTargetFPS(60);

	cam.position = { 0.0f, 5.0f, -10.0f };
	cam.target = { 0.0f, 0.0f, 0.0f };
	cam.up = { 0.0f, 1.0f, 0.0f };
	cam.fovy = 60.0f;
	cam.projection = CAMERA_PERSPECTIVE;

	player.bbox = { {0,0,0}, {1,1,1} };
	player.vel = { 0,0,0 };
	player.size = Vector3Subtract(player.bbox.max, player.bbox.min);

	obstacle.bbox = { {-2,0,2}, {2,2,3} };
	obstacle.vel = { 0,0,0 };
	obstacle.size = Vector3Subtract(obstacle.bbox.max, obstacle.bbox.min);
}

void Update()
{
	// player controls
	player.vel.x = -(static_cast<float>(IsKeyDown(KEY_D)) - static_cast<float>(IsKeyDown(KEY_A))) * playerSpeed * GetFrameTime();
	player.vel.y = (static_cast<float>(IsKeyDown(KEY_SPACE)) - static_cast<float>(IsKeyDown(KEY_LEFT_SHIFT))) * playerSpeed * GetFrameTime();
	player.vel.z = (static_cast<float>(IsKeyDown(KEY_W)) - static_cast<float>(IsKeyDown(KEY_S))) * playerSpeed * GetFrameTime();

	// swept aabb collisions
	broadphasebox = GetSweptBroadphaseBox(player);
	if (CheckCollisionBoxes(broadphasebox, obstacle.bbox))
	{
		float normalx, normaly, normalz;
		float collisiontime = SweptAABB(player, obstacle, normalx, normaly, normalz);
		MoveBoundingBox(player.bbox, Vector3Scale(player.vel, collisiontime));
		if (collisiontime < 1.0f)
		{
			Vector3 vx{}, vy{}, vz{};
			if (normalx == 1)
			{
				vx = Vector3Zero();
				vy = Vector3Up();
				vz = Vector3Forward();
			}
			else if (normalx == -1)
			{
				vx = Vector3Zero();
				vy = Vector3Down();
				vz = Vector3Backward();
			}
			else if (normaly == 1)
			{
				vx = Vector3Right();
				vy = Vector3Zero();
				vz = Vector3Forward();
			}
			else if (normaly == -1)
			{
				vx = Vector3Left();
				vy = Vector3Zero();
				vz = Vector3Backward();
			}
			else if (normalz == 1)
			{
				vx = Vector3Right();
				vy = Vector3Up();
				vz = Vector3Zero();
			}
			else if (normalz == -1)
			{
				vx = Vector3Left();
				vy = Vector3Down();
				vz = Vector3Zero();
			}

			float remainingtime = 1.0f - collisiontime;
			if (player.vel.x != 0)
			{
				float dotprod = Vector3DotProduct(player.vel, vx) * remainingtime;
				MoveBoundingBox(player.bbox, Vector3Scale(vx, dotprod));
			}
			if (player.vel.y != 0)
			{
				float dotprod = Vector3DotProduct(player.vel, vy) * remainingtime;
				MoveBoundingBox(player.bbox, Vector3Scale(vy, dotprod));
			}
			if (player.vel.z != 0)
			{
				float dotprod = Vector3DotProduct(player.vel, vz) * remainingtime;
				MoveBoundingBox(player.bbox, Vector3Scale(vz, dotprod));
			}
		}
	}
	else
	{
		MoveBoundingBox(player.bbox, player.vel);
	}
}

void Render()
{
	BeginDrawing();

	ClearBackground(WHITE);

	BeginMode3D(cam);

	DrawGrid(10, 1);

	// obstacle
	DrawCubeV(Vector3Lerp(obstacle.bbox.min, obstacle.bbox.max, 0.5f), obstacle.size, GRAY);
	DrawBoundingBox(obstacle.bbox, GOLD);

	// player
	DrawCubeV(Vector3Lerp(player.bbox.min, player.bbox.max, 0.5f), player.size, DARKBLUE);
	DrawBoundingBox(player.bbox, RED);

	// player broadphase
	DrawBoundingBox(broadphasebox, GREEN);

	EndMode3D();

	EndDrawing();
}

void DeInit()
{
	CloseWindow();
}