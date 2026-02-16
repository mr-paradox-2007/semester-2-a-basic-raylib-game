#include <iostream>
#include "raylib.h"
using namespace std;

int S_W = 1366;
int S_H = 768;
bool has_music = true;
bool has_sfx = true;
Sound trainSfx;
Sound enemyDieSfx[4];
Sound enemyShootSfx[4];
Sound playerShootSfx;
Sound playerDieSfx;


struct bg
{
    float x;
    float y;
    int h;
    int w;
    Texture2D txr;
};
struct hitbox
{
    float x;
    float y;
    Rectangle box;
};
struct player 
{
    float x;
    float y;
    float h;
    float w;
    float vx;
    float vy;
    float gravity;
    bool is_on_ground;
    bool facing_left;
    bool is_looking_down;
    bool is_shooting;
    float ground_level;
    float jump_force;
    Texture2D txr;
    Texture2D txr_idle_right;
    Texture2D txr_idle_left;
    Texture2D txr_walk_right;
    Texture2D txr_walk_left;
    Texture2D txr_jump_right;
    Texture2D txr_jump_left;
    Texture2D txr_gun_right;
    Texture2D txr_gun_left;
    Texture2D txr_gun_jump_right;
    Texture2D txr_gun_jump_left;
    hitbox hb;
};
struct obstacle 
{
    float x;
    float y;
    float h;
    float w;
    float start_x;
    float end_x;
    bool has_spawned;
    Texture2D txr;
    hitbox hb;
};
struct bullet 
{
    float x;
    float y;
    float vx;
    float vy;
    float width;
    float height;
    bool is_active;
    hitbox hb;
};
#define MAX_BULLETS 3
bullet bullets[MAX_BULLETS];
struct enemy_bullets
{
    float x;
    float y;
    float radius;
    float vy;
    float vx;
    float width;
    float height;
    bool is_active;
    int type;
    hitbox hb;
};
struct enemy 
{
    float x;
    float y;
    float h;
    float w;
    float spawn_pos1;
    float spawn_pos2;
    bool is_alive;
    void(*behavior)(enemy&);
    enemy_bullets bullet;
    hitbox hb;
    Texture2D txr;
};
void init_title(bg& bg) 
{
    bg.x = 0;
    bg.y = 0;
    bg.h = S_H;
    bg.w = S_W;
    bg.txr = LoadTexture("Assets/Title/Title_Screen.png");
    bg.txr.height = bg.h;
    bg.txr.width = bg.w;
}
bool is_button_clicked(Rectangle btn)
{
    return CheckCollisionPointRec(GetMousePosition(), btn) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}
bool pause()
{
    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(BLACK);
        DrawText("GAME PAUSED", 150, 20, 100, WHITE);
        DrawText("Press Enter to continue", 200, S_H / 2, 50, DARKGREEN);
        DrawText("Press Q to quit", 200, S_H / 2 + 50, 50, MAROON);
        EndDrawing();
        if (IsKeyPressed(KEY_ENTER)) 
            return false;
        if (IsKeyPressed(KEY_Q))
            return true;
    }
    return true;
}
void init_lvl_1(bg& clouds, bg& clouds2, bg& bridge, bg& bridge2, bg& mountains, bg& mountains2, bg& sky, bg& train, player& plr, obstacle& sign1, obstacle& sign2) 
{
    clouds.x = 0;
    clouds.y = -60;
    clouds.h = S_H;
    clouds.w = S_W;
    clouds.txr = LoadTexture("Assets/Level_1/Backgrounds/Clouds.png");
    clouds.txr.height = clouds.h;
    clouds.txr.width = clouds.w;

    clouds2.x = -S_W;
    clouds2.y = -60;
    clouds2.h = S_H;
    clouds2.w = S_W;
    clouds2.txr = LoadTexture("Assets/Level_1/Backgrounds/Clouds.png");
    clouds2.txr.height = clouds.h;
    clouds2.txr.width = clouds.w;

    bridge.x = 0;
    bridge.y = 0;
    bridge.h = S_H;
    bridge.w = S_W;
    bridge.txr = LoadTexture("Assets/Level_1/Backgrounds/Bridge.png");
    bridge.txr.height = bridge.h;
    bridge.txr.width = bridge.w;

    bridge2.x = -S_W;
    bridge2.y = 0;
    bridge2.h = S_H;
    bridge2.w = S_W;
    bridge2.txr = LoadTexture("Assets/Level_1/Backgrounds/Bridge.png");
    bridge2.txr.height = bridge.h;
    bridge2.txr.width = bridge.w;

    mountains.x = 0;
    mountains.y = 0;
    mountains.h = S_H;
    mountains.w = S_W;
    mountains.txr = LoadTexture("Assets/Level_1/Backgrounds/Mountains.png");
    mountains.txr.height = mountains.h;
    mountains.txr.width = mountains.w;

    mountains2.x = -S_H;
    mountains2.y = 0;
    mountains2.h = S_H;
    mountains2.w = S_W;
    mountains2.txr = LoadTexture("Assets/Level_1/Backgrounds/Mountains.png");
    mountains2.txr.height = mountains.h;
    mountains2.txr.width = mountains.w;

    sky.x = 0;
    sky.y = 0;
    sky.h = S_H;
    sky.w = S_W;
    sky.txr = LoadTexture("Assets/Level_1/Backgrounds/Sky.png");
    sky.txr.height = sky.h;
    sky.txr.width = sky.w;

    train.x = 0;
    train.y = -95;
    train.h = S_H;
    train.w = S_W;
    train.txr = LoadTexture("Assets/Level_1/Backgrounds/Train.png");
    train.txr.height = train.h;
    train.txr.width = train.w;

    plr.x = S_W / 2 - 500;
    plr.y = 0;
    plr.h = 200;
    plr.w = 200;
    plr.vx = 5;
    plr.vy = 10;
    plr.is_on_ground = false;
    plr.facing_left = false;
    plr.ground_level = 510;
    plr.gravity = 0.5;
    plr.jump_force = 15;
    plr.txr_idle_right = LoadTexture("Assets/Level_1/Animations/Player_lvl_1_idle_right.png");
    plr.txr_idle_right.height = plr.h;
    plr.txr_idle_right.width = plr.w;
    plr.txr = plr.txr_idle_right;
    plr.txr_idle_left = LoadTexture("Assets/Level_1/Animations/Player_lvl_1_idle_left.png");
    plr.txr_idle_left.height = plr.h;
    plr.txr_idle_left.width = plr.w;
    plr.txr_walk_right = LoadTexture("Assets/Level_1/Animations/Player_lvl_1_moving_right.png");
    plr.txr_walk_right.height = plr.h;
    plr.txr_walk_right.width = plr.w;
    plr.txr_walk_left = LoadTexture("Assets/Level_1/Animations/Player_lvl_1_moving_left.png");
    plr.txr_walk_left.height = plr.h;
    plr.txr_walk_left.width = plr.w;
    plr.txr_jump_right = LoadTexture("Assets/Level_1/Animations/Player_lvl_1_jump_right.png");
    plr.txr_jump_right.height = plr.h;
    plr.txr_jump_right.width = plr.w;
    plr.txr_jump_left = LoadTexture("Assets/Level_1/Animations/Player_lvl_1_jump_left.png");
    plr.txr_jump_left.height = plr.h;
    plr.txr_jump_left.width = plr.w;
    plr.txr_gun_left = LoadTexture("Assets/Level_1/Animations/Player_lvl_1_gun_left.png");
    plr.txr_gun_left.height = plr.h;
    plr.txr_gun_left.width = plr.w;
    plr.txr_gun_right = LoadTexture("Assets/Level_1/Animations/Player_lvl_1_gun_right.png");
    plr.txr_gun_right.height = plr.h;
    plr.txr_gun_right.width = plr.w;
    plr.txr_gun_jump_right = LoadTexture("Assets/Level_1/Animations/Player_lvl_1_gun_jump_right.png");
    plr.txr_gun_jump_right.height = plr.h;
    plr.txr_gun_jump_right.width = plr.w;
    plr.txr_gun_jump_left = LoadTexture("Assets/Level_1/Animations/Player_lvl_1_gun_jump_left.png");
    plr.txr_gun_jump_left.height = plr.h;
    plr.txr_gun_jump_left.width = plr.w;
    plr.hb.x = plr.x;
    plr.hb.y = plr.y;
    plr.hb.box = Rectangle{ plr.x + plr.w / 4 + 20, plr.y + plr.h / 4, plr.w - plr.w / 2 - 40, plr.h - plr.h / 4 };

    sign1.start_x = -1500;
    sign1.end_x = S_W + 100;
    sign1.x = sign1.start_x;
    sign1.y = -180;
    sign1.h = S_H - 300;
    sign1.w = S_W - 300;
    sign1.txr = LoadTexture("Assets/Level_1/Dangers/High_Sign.png");
    sign1.txr.height = sign1.h;
    sign1.txr.width = sign1.w;
    sign1.hb.x = sign1.x;
    sign1.hb.y = sign1.y;
    sign1.hb.box = Rectangle{ sign1.x + 400, sign1.y + 600, sign1.w - 400, sign1.h - 500 };
    sign1.has_spawned = false;

    sign2.start_x = -1500;
    sign2.end_x = S_W + 100;
    sign2.x = sign2.start_x;
    sign2.y = 300;
    sign2.h = S_H - 300;
    sign2.w = S_W - 300;
    sign2.txr = LoadTexture("Assets/Level_1/Dangers/Low_Sign.png");
    sign2.txr.height = sign2.h;
    sign2.txr.width = sign2.w;
    sign2.hb.x = sign2.x;
    sign2.hb.y = sign2.y;
    sign2.hb.box = Rectangle{ sign2.x + 400, sign2.y + 600, sign2.w - 400, sign2.h - 500 };
    sign2.has_spawned = false;
}
void move_lvl_1(bg& clouds, bg& clouds2, bg& bridge, bg& bridge2, bg& mountains, bg& mountains2)
{
    clouds.x += 0.5;
    clouds2.x += 0.5;
    bridge.x += 40;
    bridge2.x += 40;
    mountains.x += 0.15;
    mountains2.x += 0.15;
    if (clouds.x >= S_W)
        clouds.x = -S_W;
    if (clouds2.x >= S_W)
        clouds2.x = -S_W;
    if (bridge.x >= S_W)
        bridge.x = -S_W;
    if (bridge2.x >= S_W)
        bridge2.x = -S_W;
    if (mountains.x >= S_W)
        mountains.x = -S_W;
    if (mountains2.x >= S_W)
        mountains2.x = -S_W;
}
void gravity(player& plr) 
{
    if (!plr.is_on_ground) plr.vy += plr.gravity;
    plr.y += plr.vy;
    if (plr.y + plr.h >= plr.ground_level) 
    {
        plr.y = plr.ground_level - plr.h;
        plr.vy = 0;
        plr.is_on_ground = true;
    }
}
void init_bullet(bullet& b, float startX, float startY, float velocityX, float velocityY)
{
    b.x = startX;
    b.y = startY;
    b.vx = velocityX;
    b.vy = velocityY;
    b.width = 15;   
    b.height = 15;
    b.is_active = true;
    b.hb.x = b.x;
    b.hb.y = b.y;
    b.hb.box = Rectangle{ b.x, b.y, b.width, b.height };
}
Rectangle get_hitbox(const enemy_bullets& eb)
{
    Rectangle rect = { 0 };
    switch (eb.type)
    {
    case 1:
        rect.x = eb.x - 4;
        rect.y = eb.y - 4;
        rect.width = 8;
        rect.height = 8;
        break;
    case 2:
        rect.x = eb.x;
        rect.y = eb.y;
        rect.width = eb.width * 0.8f;
        rect.height = eb.height * 0.8f;
        break;
    case 3:
        rect.x = eb.x - 12;
        rect.y = eb.y - 12;
        rect.width = 24;
        rect.height = 24;
        break;
    case 4:
        rect.x = eb.x;
        rect.y = eb.y;
        rect.width = eb.width * 0.8f;
        rect.height = eb.height * 0.8f;
        break;
    default:
        rect.x = eb.x;
        rect.y = eb.y;
        rect.width = eb.width;
        rect.height = eb.height;
        break;
    }
    return rect;
}
void controller_player_lvl_1(player& plr)
{
    plr.is_looking_down = IsKeyDown(KEY_S);
    static int bullet_index = 0;
    if (IsKeyDown(KEY_SPACE))
    {
        plr.is_shooting = true;
        if (!bullets[bullet_index].is_active)
        {
            float bullet_velocity_x = plr.facing_left ? -20 : 20;
            float bullet_velocity_y;
            if (plr.is_looking_down)
            {
                bullet_velocity_x = 0;
                bullet_velocity_y = 20;
            }
            else
            {
                bullet_velocity_y = 0;
            }
            if (!plr.is_looking_down)
            {
                init_bullet(bullets[bullet_index],
                    plr.x + (plr.facing_left ? -10 : plr.w),
                    plr.y + 100,
                    bullet_velocity_x, bullet_velocity_y);
            }
            else
            {
                init_bullet(bullets[bullet_index],
                    plr.x + (plr.facing_left ? 25 : 80),
                    plr.y + 125,
                    bullet_velocity_x, bullet_velocity_y);
            }
            if (has_sfx)
            {
                PlaySound(playerShootSfx);
            }
            bullet_index = (bullet_index + 1) % MAX_BULLETS;
        }
    }


    if (IsKeyDown(KEY_A) && !IsKeyDown(KEY_D)) 
    {
        plr.x -= plr.vx;
        plr.txr = plr.txr_walk_left;
        plr.facing_left = true;
    }
    if (IsKeyDown(KEY_D) && !IsKeyDown(KEY_A)) 
    {
        plr.x += plr.vx + 7;
        plr.txr = plr.txr_walk_right;
        plr.facing_left = false;
    }
    if (!IsKeyDown(KEY_A) && !IsKeyDown(KEY_D) && !plr.facing_left) 
    {
        if (IsKeyDown(KEY_S))
            plr.txr = plr.txr_gun_jump_right;
        else
            plr.txr = plr.txr_idle_right;
        plr.facing_left = false;
    }
    if (!IsKeyDown(KEY_A) && !IsKeyDown(KEY_D) && plr.facing_left)
    {
        if (IsKeyDown(KEY_S))
            plr.txr = plr.txr_gun_jump_left;
        else
            plr.txr = plr.txr_idle_left;
        plr.facing_left = true;
    }
    if (IsKeyPressed(KEY_W) && plr.is_on_ground) 
    {
        plr.vy = -plr.jump_force;
        plr.is_on_ground = false;
    }
    if (!plr.is_on_ground) 
    {
        if (plr.facing_left) 
        {
            if (IsKeyDown(KEY_S))
                plr.txr = plr.txr_gun_jump_left;
            else
                plr.txr = plr.txr_jump_left;
        }
        else 
        {
            if (IsKeyDown(KEY_S))
                plr.txr = plr.txr_gun_jump_right;
            else
                plr.txr = plr.txr_jump_right;
        }
    }
    if (IsKeyDown(KEY_SPACE) && plr.is_on_ground && !IsKeyDown(KEY_S)) 
    {
        if (plr.facing_left)
            plr.txr = plr.txr_gun_left;
        else
            plr.txr = plr.txr_gun_right;
    }
    if (plr.x > S_W - plr.w + 70) plr.x = S_W - plr.w + 69;
    if (plr.x < 45) plr.x = 46;
    plr.hb.x = plr.x;
    plr.hb.y = plr.y;
    plr.hb.box = Rectangle{ plr.x + plr.w / 4 + 20, plr.y + plr.h / 4, plr.hb.box.width, plr.hb.box.height };
}
void update_bullet(bullet& b)
{
    if (b.is_active)
    {
        b.x += b.vx;
        b.y += b.vy;
        b.hb.x = b.x;
        b.hb.y = b.y;
        b.hb.box.x = b.x;
        b.hb.box.y = b.y;
        if (b.x < 0 || b.x > S_W || b.y < 0 || b.y > S_H)
            b.is_active = false;
    }
}
void enemy_1_control(enemy& enm)
{
    static float cooldown1 = 0.0f;
    if (cooldown1 > 0)
    {
        cooldown1 -= GetFrameTime();
    }
    if (!enm.bullet.is_active && cooldown1 <= 0.0f)
    {
        if (GetRandomValue(0, 100) < 10)
        {
            enm.bullet.is_active = true;
            enm.bullet.x = enm.x + enm.w / 2;
            enm.bullet.y = enm.y;
            enm.bullet.vy = -12.0f;
            enm.bullet.type = 1;
            cooldown1 = GetRandomValue(1, 3);
            if (has_sfx)
            {
                PlaySound(enemyShootSfx[0]);
            }
        }
    }
    if (enm.bullet.is_active)
    {
        enm.bullet.y += enm.bullet.vy;
        if (enm.bullet.y < 0)
        {
            enm.bullet.is_active = false;
        }
    }
}
void enemy_2_control(enemy& enm)
{
    static float cooldown2 = 0.0f;
    if (cooldown2 > 0)
    {
        cooldown2 -= GetFrameTime();
    }
    if (!enm.bullet.is_active && cooldown2 <= 0.0f)
    {
        if (GetRandomValue(0, 100) < 10)
        {
            enm.bullet.is_active = true;
            enm.bullet.x = enm.x + enm.w / 2;
            enm.bullet.y = enm.y;
            enm.bullet.vy = -7.0f;
            enm.bullet.width = 40;
            enm.bullet.height = 40;
            enm.bullet.type = 2;
            cooldown2 = GetRandomValue(1, 3);
            if (has_sfx)
            {
                PlaySound(enemyShootSfx[1]);
            }
        }
    }
    if (enm.bullet.is_active)
    {
        enm.bullet.y += enm.bullet.vy;
        if (enm.bullet.y < 0)
        {
            enm.bullet.is_active = false;
        }
    }
}
void enemy_3_control(enemy& enm)
{
    static float cooldown3 = 0.0f;
    if (cooldown3 > 0)
    {
        cooldown3 -= GetFrameTime();
    }
    if (!enm.bullet.is_active && cooldown3 <= 0.0f)
    {
        if (GetRandomValue(0, 100) < 10)
        {
            enm.bullet.is_active = true;
            enm.bullet.x = enm.x + enm.w / 2;
            enm.bullet.y = enm.y;
            enm.bullet.vy = -10.0f;
            enm.bullet.width = 40;
            enm.bullet.height = 40;
            enm.bullet.type = 3;
            cooldown3 = GetRandomValue(1, 3);
            if (has_sfx)
            {
                PlaySound(enemyShootSfx[2]);
            }
        }
    }
    if (enm.bullet.is_active)
    {
        enm.bullet.y += enm.bullet.vy;
        if (enm.bullet.y < enm.y - 300)
        {
            enm.bullet.vy = 12.0f;
        }
        if (enm.bullet.y > S_H)
        {
            enm.bullet.is_active = false;
        }
    }
}
void enemy_4_control(enemy& enm)
{
    static float cooldown4 = 0.0f;
    if (cooldown4 > 0)
    {
        cooldown4 -= GetFrameTime();
    }
    if (!enm.bullet.is_active && cooldown4 <= 0.0f)
    {
        if (GetRandomValue(0, 100) < 10)
        {
            enm.bullet.is_active = true;
            enm.bullet.x = enm.x;
            enm.bullet.y = enm.y + enm.h / 2;
            enm.bullet.vx = -15.0f;
            enm.bullet.width = 40;
            enm.bullet.height = 20;
            enm.bullet.type = 4;
            cooldown4 = GetRandomValue(1, 3);
            if (has_sfx)
            {
                PlaySound(enemyShootSfx[3]);
            }
        }
    }
    if (enm.bullet.is_active)
    {
        enm.bullet.x += enm.bullet.vx;
        if (enm.bullet.x < 0)
        {
            enm.bullet.is_active = false;
        }
    }
}

void draw_enemy_bullet(enemy& enm)
{
    if (enm.bullet.is_active)
    {
        switch (enm.bullet.type)
        {
        case 1:
            DrawCircle(enm.bullet.x, enm.bullet.y, 8, RED);
            break;
        case 2:
            DrawRectangle(enm.bullet.x, enm.bullet.y, enm.bullet.width, enm.bullet.height, RED);
            break;
        case 3:
            DrawCircle(enm.bullet.x, enm.bullet.y, 20, RED);
            break;
        case 4:
            DrawRectangle(enm.bullet.x, enm.bullet.y, enm.bullet.width, enm.bullet.height, RED);
            break;
        }
    }
}
void draw_enemies(enemy& e1, enemy& e2, enemy& e3, enemy& e4)
{
    if (e1.is_alive)
    {
        DrawTexture(e1.txr, e1.x, e1.y, WHITE); draw_enemy_bullet(e1);
    }
    if (e2.is_alive)
    {
        DrawTexture(e2.txr, e2.x, e2.y, WHITE); draw_enemy_bullet(e2);
    }
    if (e3.is_alive)
    {
        DrawTexture(e3.txr, e3.x, e3.y, WHITE); draw_enemy_bullet(e3);
    }
    if (e4.is_alive)
    {
        DrawTexture(e4.txr, e4.x, e4.y, WHITE); draw_enemy_bullet(e4);
    }
}
void control_enemies(enemy& e1, enemy& e2, enemy& e3, enemy& e4) 
{
    if (e1.is_alive)
        e1.behavior(e1);
    if (e2.is_alive)
        e2.behavior(e2);
    if (e3.is_alive)
        e3.behavior(e3);
    if (e4.is_alive) 
        e4.behavior(e4);
}
void init_lvl_1_enemies(enemy& enemy1, enemy& enemy2, enemy& enemy3, enemy& enemy4) 
{
    enemy1.spawn_pos1 = 400;
    enemy1.spawn_pos2 = 650;
    enemy1.x = enemy1.spawn_pos2;
    enemy1.y = S_H / 2 - 60;
    enemy1.h = 200;
    enemy1.w = 200;
    enemy1.is_alive = true;
    enemy1.hb.x = enemy1.x;
    enemy1.hb.y = enemy1.y;
    enemy1.hb.box = Rectangle{ enemy1.x + 20, enemy1.y + 40, enemy1.w - 40, enemy1.h - 40 };
    enemy1.txr = LoadTexture("Assets/Level_1/Dangers/Enemy_lvl_1_pistol.png");
    enemy1.txr.height = enemy1.h;
    enemy1.txr.width = enemy1.w;

    enemy2.spawn_pos1 = 970;
    enemy2.spawn_pos2 = 1270;
    enemy2.x = enemy2.spawn_pos2;
    enemy2.y = S_H / 2 - 60;
    enemy2.h = 200;
    enemy2.w = 200;
    enemy2.is_alive = true;
    enemy2.hb.x = enemy2.x;
    enemy2.hb.y = enemy2.y;
    enemy2.hb.box = Rectangle{ enemy2.x + 20, enemy2.y + 40, enemy2.w - 40, enemy2.h - 40 };
    enemy2.txr = LoadTexture("Assets/Level_1/Dangers/Enemy_lvl_1_shotgun.png");
    enemy2.txr.height = enemy2.h;
    enemy2.txr.width = enemy2.w;

    enemy3.spawn_pos1 = 1550;
    enemy3.spawn_pos2 = 1550;
    enemy3.x = enemy3.spawn_pos2;
    enemy3.y = S_H / 2 - 60;
    enemy3.h = 200;
    enemy3.w = 200;
    enemy3.is_alive = true;
    enemy3.hb.x = enemy3.x;
    enemy3.hb.y = enemy3.y;
    enemy3.hb.box = Rectangle{ enemy3.x + 20, enemy3.y + 40, enemy3.w - 40, enemy3.h - 40 };
    enemy3.txr = LoadTexture("Assets/Level_1/Dangers/Enemy_lvl_1_canon.png");
    enemy3.txr.height = enemy3.h;
    enemy3.txr.width = enemy3.w;

    enemy4.spawn_pos1 = S_W - 130;
    enemy4.spawn_pos2 = S_W - 130;
    enemy4.x = enemy4.spawn_pos1;
    enemy4.y = 311;
    enemy4.h = 200;
    enemy4.w = 200;
    enemy4.is_alive = true;
    enemy4.hb.x = enemy4.x;
    enemy4.hb.y = enemy4.y;
    enemy4.hb.box = Rectangle{ enemy4.x + 20, enemy4.y + 40, enemy4.w - 40, enemy4.h - 40 };
    enemy4.txr = LoadTexture("Assets/Level_1/Dangers/Enemy_lvl_1_rifle.png");
    enemy4.txr.height = enemy4.h;
    enemy4.txr.width = enemy4.w;

    enemy1.behavior = enemy_1_control;
    enemy2.behavior = enemy_2_control;
    enemy3.behavior = enemy_3_control;
    enemy4.behavior = enemy_4_control;
}
void move_obstacles_lvl_1(obstacle& sign1, obstacle& sign2, float& sign_spawn_timer, float& sign_spawn_delay)
{
    sign_spawn_timer += GetFrameTime();
    if (!sign1.has_spawned && !sign2.has_spawned && sign_spawn_timer >= sign_spawn_delay)
    {
        DrawText("WARNING: A sign is about to appear!", S_W / 2 - 500, S_H / 2 - 200, 50, RED);
        if (sign_spawn_timer >= sign_spawn_delay + 2.0) 
        {
            int sign_to_spawn = GetRandomValue(0, 1);
            if (sign_to_spawn == 0) 
                sign1.has_spawned = true;
            else 
                sign2.has_spawned = true;
            sign_spawn_timer = 0.0;
            sign_spawn_delay = GetRandomValue(15, 25);
        }
    }
    if (sign1.has_spawned)
    {
        sign1.x += 24;
        if (sign1.x >= sign1.end_x) 
        {
            sign1.x = sign1.start_x; 
            sign1.has_spawned = false; 
        }
        sign1.hb.x = sign1.x;
        sign1.hb.y = sign1.y;
        sign1.hb.box = Rectangle{ sign1.x + 600, sign1.y + 200, sign1.w - 1400, sign1.h - 520 };
    }
    if (sign2.has_spawned) 
    {
        sign2.x += 24;
        if (sign2.x >= sign2.end_x) 
        {
            sign2.x = sign2.start_x;
            sign2.has_spawned = false; 
        }
        sign2.hb.x = sign2.x;
        sign2.hb.y = sign2.y;
        sign2.hb.box = Rectangle{ sign2.x + 1000, sign2.y + 100, sign2.w - 1400, sign2.h - 600 };
    }
}
bool check_collisions_sign_lvl_1(player& plr, obstacle& sign1, obstacle& sign2) 
{
    if (CheckCollisionRecs(sign1.hb.box, plr.hb.box))
        return true;
    if (CheckCollisionRecs(sign2.hb.box, plr.hb.box)) 
        return true;
    return false;
}
void game_over_screen(int score)
{
    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(BLACK);
        DrawText("GAME OVER", S_W / 2 - 300, S_H / 2 - 100, 100, RED);
        DrawText(TextFormat("YOUR SCORE: %i",score), S_W / 2 - 200, S_H / 2, 50, GREEN);
        DrawText("PRESS Q TO RETURN", S_W / 2 - 200, S_H / 2 + 100, 50, WHITE);
        EndDrawing();
        if (IsKeyPressed(KEY_Q))
            return;
    }
}
void level_1()
{
    player plr;
    enemy enemy1, enemy2, enemy3, enemy4;
    bg clouds, clouds2, bridge, bridge2, mountains, mountains2, sky, train;
    obstacle sign1, sign2;
    long long score = 0;
    float sign_spawn_timer = 0.0;
    float sign_spawn_delay = GetRandomValue(15, 30);

    init_lvl_1(clouds, clouds2, bridge, bridge2, mountains, mountains2, sky, train, plr, sign1, sign2);
    init_lvl_1_enemies(enemy1, enemy2, enemy3, enemy4);

    double respawnTimer1 = GetTime(), respawnDelay1 = GetRandomValue(5, 10);
    double respawnTimer2 = GetTime(), respawnDelay2 = GetRandomValue(5, 10);
    double respawnTimer3 = GetTime(), respawnDelay3 = GetRandomValue(5, 10);
    double respawnTimer4 = GetTime(), respawnDelay4 = GetRandomValue(5, 10);

    static bool trainPlayed = false;
    if (!trainPlayed && has_sfx)
    {
        PlaySound(trainSfx);
        trainPlayed = true;
    }


    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(SKYBLUE);
        DrawTexture(sky.txr, sky.x, sky.y, WHITE);
        DrawTexture(mountains.txr, mountains.x, mountains.y, WHITE);
        DrawTexture(mountains2.txr, mountains2.x, mountains2.y, WHITE);
        DrawTexture(clouds.txr, clouds.x, clouds.y, WHITE);
        DrawTexture(clouds2.txr, clouds2.x, clouds2.y, WHITE);
        DrawTexture(sign1.txr, sign1.x, sign1.y, WHITE);
        DrawTexture(bridge.txr, bridge.x, bridge.y, WHITE);
        DrawTexture(bridge2.txr, bridge2.x, bridge2.y, WHITE);
        DrawTexture(train.txr, train.x, train.y, WHITE);
        DrawTexture(plr.txr, plr.x, plr.y, WHITE);
        DrawTexture(sign2.txr, sign2.x, sign2.y, WHITE);
        DrawText(TextFormat("Score: %i", score), 10, 10, 50, BLACK);

        controller_player_lvl_1(plr);
        gravity(plr);
        move_lvl_1(clouds, clouds2, bridge, bridge2, mountains, mountains2);
        move_obstacles_lvl_1(sign1, sign2, sign_spawn_timer, sign_spawn_delay);
        draw_enemies(enemy1, enemy2, enemy3, enemy4);
        control_enemies(enemy1, enemy2, enemy3, enemy4);

        for (int i = 0; i < MAX_BULLETS; i++)
        {
            if (bullets[i].is_active)
            {
                update_bullet(bullets[i]);
                DrawRectangle(bullets[i].x, bullets[i].y, bullets[i].width, bullets[i].height, BLACK);
            }
        }

        if (enemy1.is_alive) 
        {
            for (int i = 0; i < MAX_BULLETS; i++) 
            {
                if (bullets[i].is_active && CheckCollisionRecs(bullets[i].hb.box, enemy1.hb.box))
                {
                    bullets[i].is_active = false;
                    enemy1.is_alive = false;
                    score += 1000;
                    respawnTimer1 = GetTime();
                    respawnDelay1 = GetRandomValue(5, 10);
                }
            }
        }
        if (enemy2.is_alive) 
        {
            for (int i = 0; i < MAX_BULLETS; i++) 
            {
                if (bullets[i].is_active && CheckCollisionRecs(bullets[i].hb.box, enemy2.hb.box)) 
                {
                    bullets[i].is_active = false;
                    enemy2.is_alive = false;
                    score += 1000;
                    respawnTimer2 = GetTime();
                    respawnDelay2 = GetRandomValue(5, 10);
                }
            }
        }
        if (enemy3.is_alive)
        {
            for (int i = 0; i < MAX_BULLETS; i++)
            {
                if (bullets[i].is_active && CheckCollisionRecs(bullets[i].hb.box, enemy3.hb.box)) 
                {
                    bullets[i].is_active = false;
                    enemy3.is_alive = false;
                    score += 1000;
                    respawnTimer3 = GetTime();
                    respawnDelay3 = GetRandomValue(5, 10);
                }
            }
        }
        if (enemy4.is_alive)
        {
            for (int i = 0; i < MAX_BULLETS; i++) 
            {
                if (bullets[i].is_active && CheckCollisionRecs(bullets[i].hb.box, enemy4.hb.box)) 
                {
                    bullets[i].is_active = false;
                    enemy4.is_alive = false;
                    score += 1000;
                    respawnTimer4 = GetTime();
                    respawnDelay4 = GetRandomValue(5, 10);
                }
            }
        }

        for (int i = 0; i < MAX_BULLETS; i++)
        {
            if (bullets[i].is_active)
            {
                Rectangle playerBulletRect = { bullets[i].x, bullets[i].y, bullets[i].width, bullets[i].height };
                if (enemy1.bullet.is_active) 
                {
                    Rectangle eBulletRect = get_hitbox(enemy1.bullet);
                    if (CheckCollisionRecs(playerBulletRect, eBulletRect)) 
                    {
                        bullets[i].is_active = false;
                        enemy1.bullet.is_active = false;
                    }
                }
                if (enemy2.bullet.is_active)
                {
                    Rectangle eBulletRect = get_hitbox(enemy2.bullet);
                    if (CheckCollisionRecs(playerBulletRect, eBulletRect))
                    {
                        bullets[i].is_active = false;
                        enemy2.bullet.is_active = false;
                    }
                }
                if (enemy3.bullet.is_active) 
                {
                    Rectangle eBulletRect = get_hitbox(enemy3.bullet);
                    if (CheckCollisionRecs(playerBulletRect, eBulletRect)) 
                    {
                        bullets[i].is_active = false;
                        enemy3.bullet.is_active = false;
                    }
                }
                if (enemy4.bullet.is_active)
                {
                    Rectangle eBulletRect = get_hitbox(enemy4.bullet);
                    if (CheckCollisionRecs(playerBulletRect, eBulletRect)) 
                    {
                        bullets[i].is_active = false;
                        enemy4.bullet.is_active = false;
                    }
                }
            }
        }
        if (enemy1.bullet.is_active)
        {
            Rectangle eBulletRect = get_hitbox(enemy1.bullet);
            if (CheckCollisionRecs(eBulletRect, plr.hb.box))
            {
                if (has_sfx)
                {
                    PlaySound(playerDieSfx);
                }
                game_over_screen(score);
                return;
            }
        }
        if (enemy2.bullet.is_active)
        {
            Rectangle eBulletRect = get_hitbox(enemy2.bullet);
            if (CheckCollisionRecs(eBulletRect, plr.hb.box))
            {
                if (has_sfx)
                {
                    PlaySound(playerDieSfx);
                }
                game_over_screen(score);
                return;
            }
        }
        if (enemy3.bullet.is_active)
        {
            Rectangle eBulletRect = get_hitbox(enemy3.bullet);
            if (CheckCollisionRecs(eBulletRect, plr.hb.box))
            {
                if (has_sfx)
                {
                    PlaySound(playerDieSfx);
                }
                game_over_screen(score);
                return;
            }
        }
        if (enemy4.bullet.is_active)
        {
            Rectangle eBulletRect = get_hitbox(enemy4.bullet);
            if (CheckCollisionRecs(eBulletRect, plr.hb.box))
            {
                if (has_sfx)
                {
                    PlaySound(playerDieSfx);
                }
                game_over_screen(score);
                return;
            }
        }

        if (!enemy1.is_alive && (GetTime() - respawnTimer1 >= respawnDelay1))
        {
            enemy1.is_alive = true;
            int r = GetRandomValue(0, 1);
            if (r == 0)
                enemy1.x = enemy1.spawn_pos1;
            else
                enemy1.x = enemy1.spawn_pos2;
            enemy1.y = S_H / 2 - 60;
            enemy1.hb.box = Rectangle{ enemy1.x + 20, enemy1.y + 40, enemy1.w - 40, enemy1.h - 40 };
            enemy1.bullet.is_active = false;
        }

        if (!enemy2.is_alive && (GetTime() - respawnTimer2 >= respawnDelay2))
        {
            enemy2.is_alive = true;
            int r = GetRandomValue(0, 1);
            if (r == 0)
                enemy2.x = enemy2.spawn_pos1;
            else
                enemy2.x = enemy2.spawn_pos2;
            enemy2.y = S_H / 2 - 60;
            enemy2.hb.box = Rectangle{ enemy2.x + 20, enemy2.y + 40, enemy2.w - 40, enemy2.h - 40 };
            enemy2.bullet.is_active = false;
        }

        if (!enemy3.is_alive && (GetTime() - respawnTimer3 >= respawnDelay3)) 
        {
            enemy3.is_alive = true;      
            int r = GetRandomValue(0, 1);
            if (r == 0)
                enemy3.x = enemy3.spawn_pos1;
            else
                enemy3.x = enemy3.spawn_pos2;
            enemy3.y = S_H / 2 - 60;
            enemy3.hb.box = Rectangle{ enemy3.x + 20, enemy3.y + 40, enemy3.w - 40, enemy3.h - 40 };
            enemy3.bullet.is_active = false;
        }
        if (!enemy4.is_alive && (GetTime() - respawnTimer4 >= respawnDelay4))
        {
            enemy4.is_alive = true;
            enemy4.x = enemy4.spawn_pos1;
            enemy4.y = 311;
            enemy4.hb.box = Rectangle{ enemy4.x + 20, enemy4.y + 40, enemy4.w - 40, enemy4.h - 40 };
            enemy4.bullet.is_active = false;
        }

        EndDrawing();
        if (IsKeyPressed(KEY_P) && pause())
            return;
        score++; 
    }
}
void init_lvl_select(bg& lvl) 
{
    lvl.x = 0;
    lvl.y = 0;
    lvl.h = S_H;
    lvl.w = S_W;
    lvl.txr = LoadTexture("Assets/Level_Select/Level_Select.png");
    lvl.txr.height = S_H;
    lvl.txr.width = S_W;
}
void under_development() 
{
    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(BLACK);
        DrawText("THIS GAME IS CURRENTLY UNDERDEVELOPMENT", S_W / 2 - 600, S_H / 2 - 100, 50, WHITE);
        DrawText("PLEASE WAIT FOR FULL RELEASE", S_W / 2 - 370, S_H / 2 - 50, 50, WHITE);
        DrawText("IN THE MEANWHILE ENJOY LEVEL 1", S_W / 2 - 385, S_H / 2, 50, GREEN);
        DrawText("PRESS Q TO RETURN", S_W / 2 - 230, S_H / 2 + 100, 50, MAROON);
        EndDrawing();
        if (IsKeyPressed(KEY_Q)) return;
    }
}
void level_select() 
{
    bg lvl;
    Rectangle lvl1 = { 140,75,600,350 };
    Rectangle quit = { 140,680,600,350 };
    Rectangle lvl3 = { 1190,75,600,350 };
    Rectangle lvl2 = { 1190,680,600,350 };
    init_lvl_select(lvl);
    while (!WindowShouldClose()) 
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawRectangle(quit.x, quit.y, quit.width, quit.height, BLUE);
        DrawRectangle(lvl1.x, lvl1.y, lvl1.width, lvl1.height, RED);
        DrawRectangle(lvl2.x, lvl2.y, lvl2.width, lvl2.height, RED);
        DrawRectangle(lvl3.x, lvl3.y, lvl3.width, lvl3.height, RED);
        DrawTexture(lvl.txr, lvl.x, lvl.y, WHITE);
        DrawText("LEVEL SELECT", S_W / 2 - 400, S_H / 2 - 50, 100, WHITE);
        DrawText("LEVEL 1", lvl1.x + 70, lvl1.y + 100, 120, LIGHTGRAY);
        DrawText("LEVEL 3", lvl2.x + 50, lvl2.y + 100, 120, LIGHTGRAY);
        DrawText("LEVEL 2", lvl3.x + 50, lvl3.y + 100, 120, LIGHTGRAY);
        DrawText("QUIT", quit.x + 150, quit.y + 100, 120, BLACK);
        EndDrawing();
        if (is_button_clicked(lvl1)) { level_1(); }
        if (is_button_clicked(lvl2)) { under_development(); }
        if (is_button_clicked(lvl3)) { under_development(); }
        if (is_button_clicked(quit)) { return; }
    }
}
void title_screen() 
{
    bg title_screen;
    Rectangle start = { 1480,250,410,230 };
    Rectangle settings = { 1480,530,410,230 };
    Rectangle quit = { 1480,780,410,230 };
    init_title(title_screen);
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawRectangle(settings.x, settings.y, settings.width, settings.height, PINK);
        DrawRectangle(quit.x, quit.y, quit.width, quit.height, BLUE);
        DrawRectangle(start.x, start.y, start.width, start.height, RED);
        DrawTexture(title_screen.txr, title_screen.x, title_screen.y, WHITE);
        EndDrawing();
        if (is_button_clicked(start)) { level_select(); }
        else if (is_button_clicked(settings)) { return; }
        else if (is_button_clicked(quit)) { return; }
    }
}
int main() 
{
    S_W = 1920;
    S_H = 1080;
    
    InitWindow(S_W, S_H, "DARK REFLECTION");
    InitAudioDevice();
    
    trainSfx = LoadSound("Assets/Sounds/Train_Sfx.mp3");
    enemyDieSfx[0] = LoadSound("Assets/Sounds/Enemy1_Die_Sfx.mp3");
    enemyDieSfx[1] = LoadSound("Assets/Sounds/Enemy2_Die_Sfx.mp3");
    enemyDieSfx[2] = LoadSound("Assets/Sounds/Enemy3_Die_Sfx.mp3");
    enemyDieSfx[3] = LoadSound("Assets/Sounds/Enemy4_Die_Sfx.mp3");
    enemyShootSfx[0] = LoadSound("Assets/Sounds/Enemy1_Shooting_Sfx.mp3");
    enemyShootSfx[1] = LoadSound("Assets/Sounds/Enemy2_Shooting_Sfx.mp3");
    enemyShootSfx[2] = LoadSound("Assets/Sounds/Enemy3_Shooting_Sfx.mp3");
    enemyShootSfx[3] = LoadSound("Assets/Sounds/Enemy4_Shooting_Sfx.mp3");
    playerShootSfx = LoadSound("Assets/Sounds/Player_Shooting_Sfx.mp3");
    playerDieSfx = LoadSound("Assets/Sounds/Player_Die_Sfx.mp3");
    SetTargetFPS(60);
    title_screen();
    CloseWindow();
    return(0);
}
