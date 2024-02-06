//#include "raylib.h"
//#include <limits>
//#include <algorithm>
//
//void Init();
//void Update();
//void Render();
//void DeInit();
//
//// describes an axis-aligned rectangle with a velocity 
//struct Box
//{
//	Vector2 pos;
//	Vector2 size;
//	Vector2 vel;
//};
//
//float SweptAABB(Box b1, Box b2, float& normalx, float& normaly)
//{
//	float xInvEntry, yInvEntry;
//	float xInvExit, yInvExit;
//
//	// find the distance between the objects on the near and far sides for both x and y 
//	if (b1.vel.x > 0.0f)
//	{
//		xInvEntry = b2.pos.x - (b1.pos.x + b1.size.x);
//		xInvExit = (b2.pos.x + b2.size.x) - b1.pos.x;
//	}
//	else
//	{
//		xInvEntry = (b2.pos.x + b2.size.x) - b1.pos.x;
//		xInvExit = b2.pos.x - (b1.pos.x + b1.size.x);
//	}
//
//	if (b1.vel.y > 0.0f)
//	{
//		yInvEntry = b2.pos.y - (b1.pos.y + b1.size.y);
//		yInvExit = (b2.pos.y + b2.size.y) - b1.pos.y;
//	}
//	else
//	{
//		yInvEntry = (b2.pos.y + b2.size.y) - b1.pos.y;
//		yInvExit = b2.pos.y - (b1.pos.y + b1.size.y);
//	}
//
//	// find time of collision and time of leaving for each axis (if statement is to prevent divide by zero) 
//	float xEntry, yEntry;
//	float xExit, yExit;
//
//	if (b1.vel.x == 0.0f)
//	{
//		xEntry = -std::numeric_limits<float>::infinity();
//		xExit = std::numeric_limits<float>::infinity();
//	}
//	else
//	{
//		xEntry = xInvEntry / b1.vel.x;
//		xExit = xInvExit / b1.vel.x;
//	}
//
//	if (b1.vel.y == 0.0f)
//	{
//		yEntry = -std::numeric_limits<float>::infinity();
//		yExit = std::numeric_limits<float>::infinity();
//	}
//	else
//	{
//		yEntry = yInvEntry / b1.vel.y;
//		yExit = yInvExit / b1.vel.y;
//	}
//
//	// find the earliest/latest times of collisionfloat 
//	float entryTime = std::max(xEntry, yEntry);
//	float exitTime = std::min(xExit, yExit);
//
//	// if there was no collision
//	if (entryTime > exitTime || xEntry < 0.0f && yEntry < 0.0f || xEntry > 1.0f || yEntry > 1.0f)
//	{
//		normalx = 0.0f;
//		normaly = 0.0f;
//		return 1.0f;
//	}
//
//	else // if there was a collision 
//	{
//		// calculate normal of collided surface
//		if (xEntry > yEntry)
//		{
//			if (xInvEntry < 0.0f)
//			{
//				normalx = 1.0f;
//				normaly = 0.0f;
//			}
//			else
//			{
//				normalx = -1.0f;
//				normaly = 0.0f;
//			}
//		}
//		else
//		{
//			if (yInvEntry < 0.0f)
//			{
//				normalx = 0.0f;
//				normaly = 1.0f;
//			}
//			else
//			{
//				normalx = 0.0f;
//				normaly = -1.0f;
//			}
//		}
//	}
//
//	return entryTime;
//}
////
//Box GetSweptBroadphaseBox(Box b)
//{
//	Box broadphasebox;
//
//	broadphasebox.pos.x = b.vel.x > 0 ? b.pos.x : b.pos.x + b.vel.x;
//	broadphasebox.size.x = b.vel.x > 0 ? b.vel.x + b.size.x : b.size.x - b.vel.x;
//
//	broadphasebox.pos.y = b.vel.y > 0 ? b.pos.y : b.pos.y + b.vel.y;
//	broadphasebox.size.y = b.vel.y > 0 ? b.vel.y + b.size.y : b.size.y - b.vel.y;
//
//	return broadphasebox;
//}
//
//Rectangle BoxToRectangle(Box b)
//{
//	return { b.pos.x, b.pos.y, b.size.x, b.size.y };
//}
//
////bool AABBCheck(Box b1, Box b2)
////{
////	return !(b1.x + b1.w < b2.x || b1.x > b2.x + b2.w || b1.y + b1.h < b2.y || b1.y > b2.y + b2.h);
////}
//
//Box player;
//Box obstacle;
//Box broadphasebox;
//const float playerSpeed = 200;
//
////temp
//float dotprod;
//
//int main(void)
//{
//	Init();
//
//	while (!WindowShouldClose())
//	{
//		Update();
//		Render();
//	}
//
//	DeInit();
//
//	return 0;
//}
//
//void Init()
//{
//	// window
//	const int screenWidth = 1280;
//	const int screenHeight = 720;
//	InitWindow(screenWidth, screenHeight, "Swept AABB Tests");
//	SetTargetFPS(60);
//
//	player = { 50, 50, 50, 50, 0, 0 };
//	obstacle = { 600, 100, 40, 500, 0, 0 };
//}
//
//void Update()
//{
//	// player controls
//	player.vel.x = (static_cast<float>(IsKeyDown(KEY_D)) - static_cast<float>(IsKeyDown(KEY_A))) * playerSpeed * GetFrameTime();
//	player.vel.y = (static_cast<float>(IsKeyDown(KEY_S)) - static_cast<float>(IsKeyDown(KEY_W))) * playerSpeed * GetFrameTime();
//
//	// swept aabb collisions
//	broadphasebox = GetSweptBroadphaseBox(player);
//	if (CheckCollisionRecs(BoxToRectangle(broadphasebox), BoxToRectangle(obstacle)))
//	{
//		float normalx, normaly;
//		float collisiontime = SweptAABB(player, obstacle, normalx, normaly);
//		player.pos.x += player.vel.x * collisiontime;
//		player.pos.y += player.vel.y * collisiontime;
//		if (collisiontime < 1.0f)
//		{
//			float remainingtime = 1.0f - collisiontime;
//			dotprod = (player.vel.x * normaly + player.vel.y * normalx) * remainingtime;
//			player.pos.x += dotprod * normaly;
//			player.pos.y += dotprod * normalx;
//		}
//	}
//	else
//	{
//		player.pos.x += player.vel.x;
//		player.pos.y += player.vel.y;
//	}
//}
//
//void Render()
//{
//	BeginDrawing();
//
//	ClearBackground(WHITE);
//	DrawRectangleRec(BoxToRectangle(obstacle), GRAY);
//	DrawRectangleRec(BoxToRectangle(player), DARKBLUE);
//	DrawRectangleLinesEx(BoxToRectangle(broadphasebox), 1, RED);
//
//	DrawText(TextFormat("Player Velocity: %f, %f", player.vel.x, player.vel.y), 30, 30, 20, BLACK);
//
//	EndDrawing();
//}
//
//void DeInit()
//{
//	CloseWindow();
//}