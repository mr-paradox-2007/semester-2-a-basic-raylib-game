#include <iostream>
#include "raylib.h"

// Platform-specific includes
#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

using namespace std;

// =============================================================================
// GAME STATE ENUM
// =============================================================================
enum GameScreen 
{
    TITLE,
    LEVEL_SELECT,
    LEVEL_1,
    PAUSE,
    GAME_OVER,
    UNDER_DEVELOPMENT
};

// =============================================================================
// GLOBAL CONFIGURATION
// =============================================================================
int S_W = 1920;
int S_H = 1080;
bool has_music = true;
bool has_sfx = true;

GameScreen currentScreen = TITLE;
GameScreen previousScreen = TITLE;

// =============================================================================
// STRUCTS
// =============================================================================
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

// =============================================================================
// GLOBAL RESOURCES (Loaded once in InitGame)
// =============================================================================
Sound trainSfx;
Sound enemyDieSfx[4];
Sound enemyShootSfx[4];
Sound playerShootSfx;
Sound playerDieSfx;

// Title Screen
bg title_bg;

// Level Select
bg lvl_select_bg;

// Level 1 Resources
bg clouds, clouds2, bridge, bridge2, mountains, mountains2, sky, train;
player plr;
obstacle sign1, sign2;
enemy enemy1, enemy2, enemy3, enemy4;

#define MAX_BULLETS 3
bullet bullets[MAX_BULLETS];

// Level 1 Game State
long long score = 0;
float sign_spawn_timer = 0.0f;
float sign_spawn_delay = 15.0f;
double respawnTimer1 = 0.0, respawnDelay1 = 5.0;
double respawnTimer2 = 0.0, respawnDelay2 = 5.0;
double respawnTimer3 = 0.0, respawnDelay3 = 5.0;
double respawnTimer4 = 0.0, respawnDelay4 = 5.0;
bool trainPlayed = false;

// UI Buttons
Rectangle btn_title_start = { 1480, 250, 410, 230 };
Rectangle btn_title_settings = { 1480, 530, 410, 230 };
Rectangle btn_title_quit = { 1480, 780, 410, 230 };

Rectangle btn_lvl1 = { 140, 75, 600, 350 };
Rectangle btn_lvl2 = { 1190, 680, 600, 350 };
Rectangle btn_lvl3 = { 1190, 75, 600, 350 };
Rectangle btn_quit_lvl_select = { 140, 680, 600, 350 };

// =============================================================================
// FORWARD DECLARATIONS
// =============================================================================
void enemy_1_control(enemy& enm);
void enemy_2_control(enemy& enm);
void enemy_3_control(enemy& enm);
void enemy_4_control(enemy& enm);

// =============================================================================
// UTILITY FUNCTIONS
// =============================================================================
bool is_button_clicked(Rectangle btn)
{
    return CheckCollisionPointRec(GetMousePosition(), btn) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
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

// =============================================================================
// RESOURCE LOADING (Called ONCE at startup)
// =============================================================================
void InitGame()
{
    // Load Audio
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

    // Load Title Screen
    title_bg.x = 0;
    title_bg.y = 0;
    title_bg.h = S_H;
    title_bg.w = S_W;
    title_bg.txr = LoadTexture("Assets/Title/Title_Screen.png");
    title_bg.txr.height = title_bg.h;
    title_bg.txr.width = title_bg.w;

    // Load Level Select
    lvl_select_bg.x = 0;
    lvl_select_bg.y = 0;
    lvl_select_bg.h = S_H;
    lvl_select_bg.w = S_W;
    lvl_select_bg.txr = LoadTexture("Assets/Level_Select/Level_Select.png");
    lvl_select_bg.txr.height = lvl_select_bg.h;
    lvl_select_bg.txr.width = lvl_select_bg.w;

    // Load Level 1 Backgrounds
    clouds.txr = LoadTexture("Assets/Level_1/Backgrounds/Clouds.png");
    clouds2.txr = LoadTexture("Assets/Level_1/Backgrounds/Clouds.png");
    bridge.txr = LoadTexture("Assets/Level_1/Backgrounds/Bridge.png");
    bridge2.txr = LoadTexture("Assets/Level_1/Backgrounds/Bridge.png");
    mountains.txr = LoadTexture("Assets/Level_1/Backgrounds/Mountains.png");
    mountains2.txr = LoadTexture("Assets/Level_1/Backgrounds/Mountains.png");
    sky.txr = LoadTexture("Assets/Level_1/Backgrounds/Sky.png");
    train.txr = LoadTexture("Assets/Level_1/Backgrounds/Train.png");

    // Load Player Textures
    plr.txr_idle_right = LoadTexture("Assets/Level_1/Animations/Player_lvl_1_idle_right.png");
    plr.txr_idle_left = LoadTexture("Assets/Level_1/Animations/Player_lvl_1_idle_left.png");
    plr.txr_walk_right = LoadTexture("Assets/Level_1/Animations/Player_lvl_1_moving_right.png");
    plr.txr_walk_left = LoadTexture("Assets/Level_1/Animations/Player_lvl_1_moving_left.png");
    plr.txr_jump_right = LoadTexture("Assets/Level_1/Animations/Player_lvl_1_jump_right.png");
    plr.txr_jump_left = LoadTexture("Assets/Level_1/Animations/Player_lvl_1_jump_left.png");
    plr.txr_gun_left = LoadTexture("Assets/Level_1/Animations/Player_lvl_1_gun_left.png");
    plr.txr_gun_right = LoadTexture("Assets/Level_1/Animations/Player_lvl_1_gun_right.png");
    plr.txr_gun_jump_right = LoadTexture("Assets/Level_1/Animations/Player_lvl_1_gun_jump_right.png");
    plr.txr_gun_jump_left = LoadTexture("Assets/Level_1/Animations/Player_lvl_1_gun_jump_left.png");

    // Load Obstacles
    sign1.txr = LoadTexture("Assets/Level_1/Dangers/High_Sign.png");
    sign2.txr = LoadTexture("Assets/Level_1/Dangers/Low_Sign.png");

    // Load Enemies
    enemy1.txr = LoadTexture("Assets/Level_1/Dangers/Enemy_lvl_1_pistol.png");
    enemy2.txr = LoadTexture("Assets/Level_1/Dangers/Enemy_lvl_1_shotgun.png");
    enemy3.txr = LoadTexture("Assets/Level_1/Dangers/Enemy_lvl_1_canon.png");
    enemy4.txr = LoadTexture("Assets/Level_1/Dangers/Enemy_lvl_1_rifle.png");
}

// =============================================================================
// LEVEL 1 INITIALIZATION (Resets variables without reloading textures)
// =============================================================================
void ResetLevel1()
{
    // Reset backgrounds
    clouds.x = 0;
    clouds.y = -60;
    clouds.h = S_H;
    clouds.w = S_W;
    clouds.txr.height = clouds.h;
    clouds.txr.width = clouds.w;

    clouds2.x = -S_W;
    clouds2.y = -60;
    clouds2.h = S_H;
    clouds2.w = S_W;
    clouds2.txr.height = clouds2.h;
    clouds2.txr.width = clouds2.w;

    bridge.x = 0;
    bridge.y = 0;
    bridge.h = S_H;
    bridge.w = S_W;
    bridge.txr.height = bridge.h;
    bridge.txr.width = bridge.w;

    bridge2.x = -S_W;
    bridge2.y = 0;
    bridge2.h = S_H;
    bridge2.w = S_W;
    bridge2.txr.height = bridge2.h;
    bridge2.txr.width = bridge2.w;

    mountains.x = 0;
    mountains.y = 0;
    mountains.h = S_H;
    mountains.w = S_W;
    mountains.txr.height = mountains.h;
    mountains.txr.width = mountains.w;

    mountains2.x = -S_H;
    mountains2.y = 0;
    mountains2.h = S_H;
    mountains2.w = S_W;
    mountains2.txr.height = mountains2.h;
    mountains2.txr.width = mountains2.w;

    sky.x = 0;
    sky.y = 0;
    sky.h = S_H;
    sky.w = S_W;
    sky.txr.height = sky.h;
    sky.txr.width = sky.w;

    train.x = 0;
    train.y = -95;
    train.h = S_H;
    train.w = S_W;
    train.txr.height = train.h;
    train.txr.width = train.w;

    // Reset player
    plr.x = S_W / 2 - 500;
    plr.y = 0;
    plr.h = 200;
    plr.w = 200;
    plr.vx = 5;
    plr.vy = 10;
    plr.is_on_ground = false;
    plr.facing_left = false;
    plr.ground_level = 510;
    plr.gravity = 0.5f;
    plr.jump_force = 15;
    plr.txr = plr.txr_idle_right;
    plr.txr_idle_right.height = plr.h;
    plr.txr_idle_right.width = plr.w;
    plr.txr_idle_left.height = plr.h;
    plr.txr_idle_left.width = plr.w;
    plr.txr_walk_right.height = plr.h;
    plr.txr_walk_right.width = plr.w;
    plr.txr_walk_left.height = plr.h;
    plr.txr_walk_left.width = plr.w;
    plr.txr_jump_right.height = plr.h;
    plr.txr_jump_right.width = plr.w;
    plr.txr_jump_left.height = plr.h;
    plr.txr_jump_left.width = plr.w;
    plr.txr_gun_left.height = plr.h;
    plr.txr_gun_left.width = plr.w;
    plr.txr_gun_right.height = plr.h;
    plr.txr_gun_right.width = plr.w;
    plr.txr_gun_jump_right.height = plr.h;
    plr.txr_gun_jump_right.width = plr.w;
    plr.txr_gun_jump_left.height = plr.h;
    plr.txr_gun_jump_left.width = plr.w;
    plr.hb.x = plr.x;
    plr.hb.y = plr.y;
    plr.hb.box = Rectangle{ plr.x + plr.w / 4 + 20, plr.y + plr.h / 4, plr.w - plr.w / 2 - 40, plr.h - plr.h / 4 };

    // Reset obstacles
    sign1.start_x = -1500;
    sign1.end_x = S_W + 100;
    sign1.x = sign1.start_x;
    sign1.y = -180;
    sign1.h = S_H - 300;
    sign1.w = S_W - 300;
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
    sign2.txr.height = sign2.h;
    sign2.txr.width = sign2.w;
    sign2.hb.x = sign2.x;
    sign2.hb.y = sign2.y;
    sign2.hb.box = Rectangle{ sign2.x + 400, sign2.y + 600, sign2.w - 400, sign2.h - 500 };
    sign2.has_spawned = false;

    // Reset enemies
    enemy1.spawn_pos1 = 400;
    enemy1.spawn_pos2 = 650;
    enemy1.x = enemy1.spawn_pos2;
    enemy1.y = S_H / 2 - 60;
    enemy1.h = 200;
    enemy1.w = 200;
    enemy1.is_alive = true;
    enemy1.txr.height = enemy1.h;
    enemy1.txr.width = enemy1.w;
    enemy1.hb.x = enemy1.x;
    enemy1.hb.y = enemy1.y;
    enemy1.hb.box = Rectangle{ enemy1.x + 20, enemy1.y + 40, enemy1.w - 40, enemy1.h - 40 };
    enemy1.behavior = enemy_1_control;
    enemy1.bullet.is_active = false;

    enemy2.spawn_pos1 = 970;
    enemy2.spawn_pos2 = 1270;
    enemy2.x = enemy2.spawn_pos2;
    enemy2.y = S_H / 2 - 60;
    enemy2.h = 200;
    enemy2.w = 200;
    enemy2.is_alive = true;
    enemy2.txr.height = enemy2.h;
    enemy2.txr.width = enemy2.w;
    enemy2.hb.x = enemy2.x;
    enemy2.hb.y = enemy2.y;
    enemy2.hb.box = Rectangle{ enemy2.x + 20, enemy2.y + 40, enemy2.w - 40, enemy2.h - 40 };
    enemy2.behavior = enemy_2_control;
    enemy2.bullet.is_active = false;

    enemy3.spawn_pos1 = 1550;
    enemy3.spawn_pos2 = 1550;
    enemy3.x = enemy3.spawn_pos2;
    enemy3.y = S_H / 2 - 60;
    enemy3.h = 200;
    enemy3.w = 200;
    enemy3.is_alive = true;
    enemy3.txr.height = enemy3.h;
    enemy3.txr.width = enemy3.w;
    enemy3.hb.x = enemy3.x;
    enemy3.hb.y = enemy3.y;
    enemy3.hb.box = Rectangle{ enemy3.x + 20, enemy3.y + 40, enemy3.w - 40, enemy3.h - 40 };
    enemy3.behavior = enemy_3_control;
    enemy3.bullet.is_active = false;

    enemy4.spawn_pos1 = S_W - 130;
    enemy4.spawn_pos2 = S_W - 130;
    enemy4.x = enemy4.spawn_pos1;
    enemy4.y = 311;
    enemy4.h = 200;
    enemy4.w = 200;
    enemy4.is_alive = true;
    enemy4.txr.height = enemy4.h;
    enemy4.txr.width = enemy4.w;
    enemy4.hb.x = enemy4.x;
    enemy4.hb.y = enemy4.y;
    enemy4.hb.box = Rectangle{ enemy4.x + 20, enemy4.y + 40, enemy4.w - 40, enemy4.h - 40 };
    enemy4.behavior = enemy_4_control;
    enemy4.bullet.is_active = false;

    // Reset bullets
    for (int i = 0; i < MAX_BULLETS; i++)
    {
        bullets[i].is_active = false;
    }

    // Reset game state
    score = 0;
    sign_spawn_timer = 0.0f;
    sign_spawn_delay = (float)GetRandomValue(15, 30);
    respawnTimer1 = GetTime();
    respawnDelay1 = GetRandomValue(5, 10);
    respawnTimer2 = GetTime();
    respawnDelay2 = GetRandomValue(5, 10);
    respawnTimer3 = GetTime();
    respawnDelay3 = GetRandomValue(5, 10);
    respawnTimer4 = GetTime();
    respawnDelay4 = GetRandomValue(5, 10);
    trainPlayed = false;
}

// =============================================================================
// BULLET FUNCTIONS
// =============================================================================
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

// =============================================================================
// ENEMY BEHAVIORS
// =============================================================================
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
            cooldown1 = (float)GetRandomValue(1, 3);
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
            cooldown2 = (float)GetRandomValue(1, 3);
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
            cooldown3 = (float)GetRandomValue(1, 3);
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
            cooldown4 = (float)GetRandomValue(1, 3);
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
            DrawCircle((int)enm.bullet.x, (int)enm.bullet.y, 8, RED);
            break;
        case 2:
            DrawRectangle((int)enm.bullet.x, (int)enm.bullet.y, (int)enm.bullet.width, (int)enm.bullet.height, RED);
            break;
        case 3:
            DrawCircle((int)enm.bullet.x, (int)enm.bullet.y, 20, RED);
            break;
        case 4:
            DrawRectangle((int)enm.bullet.x, (int)enm.bullet.y, (int)enm.bullet.width, (int)enm.bullet.height, RED);
            break;
        }
    }
}

// =============================================================================
// PLAYER FUNCTIONS
// =============================================================================
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

// =============================================================================
// LEVEL 1 UPDATE FUNCTIONS
// =============================================================================
void move_lvl_1_backgrounds()
{
    clouds.x += 0.5f;
    clouds2.x += 0.5f;
    bridge.x += 40;
    bridge2.x += 40;
    mountains.x += 0.15f;
    mountains2.x += 0.15f;
    
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

void move_obstacles_lvl_1()
{
    sign_spawn_timer += GetFrameTime();
    if (!sign1.has_spawned && !sign2.has_spawned && sign_spawn_timer >= sign_spawn_delay)
    {
        if (sign_spawn_timer >= sign_spawn_delay + 2.0f) 
        {
            int sign_to_spawn = GetRandomValue(0, 1);
            if (sign_to_spawn == 0) 
                sign1.has_spawned = true;
            else 
                sign2.has_spawned = true;
            sign_spawn_timer = 0.0f;
            sign_spawn_delay = (float)GetRandomValue(15, 25);
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

void control_enemies() 
{
    if (enemy1.is_alive)
        enemy1.behavior(enemy1);
    if (enemy2.is_alive)
        enemy2.behavior(enemy2);
    if (enemy3.is_alive)
        enemy3.behavior(enemy3);
    if (enemy4.is_alive) 
        enemy4.behavior(enemy4);
}

void handle_enemy_respawn()
{
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
}

void handle_collisions()
{
    // Check sign collisions
    if (CheckCollisionRecs(sign1.hb.box, plr.hb.box) || CheckCollisionRecs(sign2.hb.box, plr.hb.box))
    {
        if (has_sfx)
        {
            PlaySound(playerDieSfx);
        }
        currentScreen = GAME_OVER;
        return;
    }

    // Check bullet vs enemy collisions
    for (int i = 0; i < MAX_BULLETS; i++)
    {
        if (!bullets[i].is_active) continue;

        if (enemy1.is_alive && CheckCollisionRecs(bullets[i].hb.box, enemy1.hb.box))
        {
            bullets[i].is_active = false;
            enemy1.is_alive = false;
            score += 1000;
            respawnTimer1 = GetTime();
            respawnDelay1 = GetRandomValue(5, 10);
        }
        
        if (enemy2.is_alive && CheckCollisionRecs(bullets[i].hb.box, enemy2.hb.box))
        {
            bullets[i].is_active = false;
            enemy2.is_alive = false;
            score += 1000;
            respawnTimer2 = GetTime();
            respawnDelay2 = GetRandomValue(5, 10);
        }
        
        if (enemy3.is_alive && CheckCollisionRecs(bullets[i].hb.box, enemy3.hb.box))
        {
            bullets[i].is_active = false;
            enemy3.is_alive = false;
            score += 1000;
            respawnTimer3 = GetTime();
            respawnDelay3 = GetRandomValue(5, 10);
        }
        
        if (enemy4.is_alive && CheckCollisionRecs(bullets[i].hb.box, enemy4.hb.box))
        {
            bullets[i].is_active = false;
            enemy4.is_alive = false;
            score += 1000;
            respawnTimer4 = GetTime();
            respawnDelay4 = GetRandomValue(5, 10);
        }
    }

    // Check player bullet vs enemy bullet collisions
    for (int i = 0; i < MAX_BULLETS; i++)
    {
        if (!bullets[i].is_active) continue;
        
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

    // Check enemy bullets vs player
    if (enemy1.bullet.is_active)
    {
        Rectangle eBulletRect = get_hitbox(enemy1.bullet);
        if (CheckCollisionRecs(eBulletRect, plr.hb.box))
        {
            if (has_sfx)
            {
                PlaySound(playerDieSfx);
            }
            currentScreen = GAME_OVER;
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
            currentScreen = GAME_OVER;
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
            currentScreen = GAME_OVER;
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
            currentScreen = GAME_OVER;
            return;
        }
    }
}

// =============================================================================
// SCREEN UPDATE FUNCTIONS
// =============================================================================
void UpdateTitle()
{
    if (is_button_clicked(btn_title_start))
    {
        currentScreen = LEVEL_SELECT;
    }
    else if (is_button_clicked(btn_title_settings))
    {
        // Future: Settings screen
    }
    else if (is_button_clicked(btn_title_quit))
    {
        #ifdef __EMSCRIPTEN__
        // Can't close window on web, just return to title
        #else
        CloseWindow();
        #endif
    }
}

void UpdateLevelSelect()
{
    if (is_button_clicked(btn_lvl1))
    {
        ResetLevel1();
        currentScreen = LEVEL_1;
    }
    else if (is_button_clicked(btn_lvl2) || is_button_clicked(btn_lvl3))
    {
        currentScreen = UNDER_DEVELOPMENT;
    }
    else if (is_button_clicked(btn_quit_lvl_select))
    {
        currentScreen = TITLE;
    }
}

void UpdateLevel1()
{
    // Play train sound once
    if (!trainPlayed && has_sfx)
    {
        PlaySound(trainSfx);
        trainPlayed = true;
    }

    // Handle pause
    if (IsKeyPressed(KEY_P))
    {
        previousScreen = LEVEL_1;
        currentScreen = PAUSE;
        return;
    }

    // Update game logic
    controller_player_lvl_1(plr);
    gravity(plr);
    move_lvl_1_backgrounds();
    move_obstacles_lvl_1();
    control_enemies();
    handle_enemy_respawn();

    // Update bullets
    for (int i = 0; i < MAX_BULLETS; i++)
    {
        if (bullets[i].is_active)
        {
            update_bullet(bullets[i]);
        }
    }

    // Handle collisions (may change screen to GAME_OVER)
    handle_collisions();

    // Increment score
    score++;
}

void UpdatePause()
{
    if (IsKeyPressed(KEY_ENTER))
    {
        currentScreen = previousScreen;
    }
    else if (IsKeyPressed(KEY_Q))
    {
        currentScreen = TITLE;
    }
}

void UpdateGameOver()
{
    if (IsKeyPressed(KEY_Q))
    {
        currentScreen = TITLE;
    }
}

void UpdateUnderDevelopment()
{
    if (IsKeyPressed(KEY_Q))
    {
        currentScreen = LEVEL_SELECT;
    }
}

// =============================================================================
// SCREEN DRAW FUNCTIONS
// =============================================================================
void DrawTitle()
{
    ClearBackground(RAYWHITE);
    DrawRectangle((int)btn_title_settings.x, (int)btn_title_settings.y, (int)btn_title_settings.width, (int)btn_title_settings.height, PINK);
    DrawRectangle((int)btn_title_quit.x, (int)btn_title_quit.y, (int)btn_title_quit.width, (int)btn_title_quit.height, BLUE);
    DrawRectangle((int)btn_title_start.x, (int)btn_title_start.y, (int)btn_title_start.width, (int)btn_title_start.height, RED);
    DrawTexture(title_bg.txr, (int)title_bg.x, (int)title_bg.y, WHITE);
}

void DrawLevelSelect()
{
    ClearBackground(RAYWHITE);
    DrawRectangle((int)btn_quit_lvl_select.x, (int)btn_quit_lvl_select.y, (int)btn_quit_lvl_select.width, (int)btn_quit_lvl_select.height, BLUE);
    DrawRectangle((int)btn_lvl1.x, (int)btn_lvl1.y, (int)btn_lvl1.width, (int)btn_lvl1.height, RED);
    DrawRectangle((int)btn_lvl2.x, (int)btn_lvl2.y, (int)btn_lvl2.width, (int)btn_lvl2.height, RED);
    DrawRectangle((int)btn_lvl3.x, (int)btn_lvl3.y, (int)btn_lvl3.width, (int)btn_lvl3.height, RED);
    DrawTexture(lvl_select_bg.txr, (int)lvl_select_bg.x, (int)lvl_select_bg.y, WHITE);
    DrawText("LEVEL SELECT", S_W / 2 - 400, S_H / 2 - 50, 100, WHITE);
    DrawText("LEVEL 1", (int)btn_lvl1.x + 70, (int)btn_lvl1.y + 100, 120, LIGHTGRAY);
    DrawText("LEVEL 3", (int)btn_lvl2.x + 50, (int)btn_lvl2.y + 100, 120, LIGHTGRAY);
    DrawText("LEVEL 2", (int)btn_lvl3.x + 50, (int)btn_lvl3.y + 100, 120, LIGHTGRAY);
    DrawText("QUIT", (int)btn_quit_lvl_select.x + 150, (int)btn_quit_lvl_select.y + 100, 120, BLACK);
}

void DrawLevel1()
{
    ClearBackground(SKYBLUE);
    DrawTexture(sky.txr, (int)sky.x, (int)sky.y, WHITE);
    DrawTexture(mountains.txr, (int)mountains.x, (int)mountains.y, WHITE);
    DrawTexture(mountains2.txr, (int)mountains2.x, (int)mountains2.y, WHITE);
    DrawTexture(clouds.txr, (int)clouds.x, (int)clouds.y, WHITE);
    DrawTexture(clouds2.txr, (int)clouds2.x, (int)clouds2.y, WHITE);
    DrawTexture(sign1.txr, (int)sign1.x, (int)sign1.y, WHITE);
    DrawTexture(bridge.txr, (int)bridge.x, (int)bridge.y, WHITE);
    DrawTexture(bridge2.txr, (int)bridge2.x, (int)bridge2.y, WHITE);
    DrawTexture(train.txr, (int)train.x, (int)train.y, WHITE);
    DrawTexture(plr.txr, (int)plr.x, (int)plr.y, WHITE);
    DrawTexture(sign2.txr, (int)sign2.x, (int)sign2.y, WHITE);

    // Draw enemies
    if (enemy1.is_alive)
    {
        DrawTexture(enemy1.txr, (int)enemy1.x, (int)enemy1.y, WHITE);
        draw_enemy_bullet(enemy1);
    }
    if (enemy2.is_alive)
    {
        DrawTexture(enemy2.txr, (int)enemy2.x, (int)enemy2.y, WHITE);
        draw_enemy_bullet(enemy2);
    }
    if (enemy3.is_alive)
    {
        DrawTexture(enemy3.txr, (int)enemy3.x, (int)enemy3.y, WHITE);
        draw_enemy_bullet(enemy3);
    }
    if (enemy4.is_alive)
    {
        DrawTexture(enemy4.txr, (int)enemy4.x, (int)enemy4.y, WHITE);
        draw_enemy_bullet(enemy4);
    }

    // Draw bullets
    for (int i = 0; i < MAX_BULLETS; i++)
    {
        if (bullets[i].is_active)
        {
            DrawRectangle((int)bullets[i].x, (int)bullets[i].y, (int)bullets[i].width, (int)bullets[i].height, BLACK);
        }
    }

    // Draw warning for signs
    if (!sign1.has_spawned && !sign2.has_spawned && sign_spawn_timer >= sign_spawn_delay && sign_spawn_timer < sign_spawn_delay + 2.0f)
    {
        DrawText("WARNING: A sign is about to appear!", S_W / 2 - 500, S_H / 2 - 200, 50, RED);
    }

    // Draw score
    DrawText(TextFormat("Score: %lld", score), 10, 10, 50, BLACK);
}

void DrawPause()
{
    ClearBackground(BLACK);
    DrawText("GAME PAUSED", 150, 20, 100, WHITE);
    DrawText("Press Enter to continue", 200, S_H / 2, 50, DARKGREEN);
    DrawText("Press Q to quit", 200, S_H / 2 + 50, 50, MAROON);
}

void DrawGameOver()
{
    ClearBackground(BLACK);
    DrawText("GAME OVER", S_W / 2 - 300, S_H / 2 - 100, 100, RED);
    DrawText(TextFormat("YOUR SCORE: %lld", score), S_W / 2 - 200, S_H / 2, 50, GREEN);
    DrawText("PRESS Q TO RETURN", S_W / 2 - 200, S_H / 2 + 100, 50, WHITE);
}

void DrawUnderDevelopment()
{
    ClearBackground(BLACK);
    DrawText("THIS GAME IS CURRENTLY UNDERDEVELOPMENT", S_W / 2 - 600, S_H / 2 - 100, 50, WHITE);
    DrawText("PLEASE WAIT FOR FULL RELEASE", S_W / 2 - 370, S_H / 2 - 50, 50, WHITE);
    DrawText("IN THE MEANWHILE ENJOY LEVEL 1", S_W / 2 - 385, S_H / 2, 50, GREEN);
    DrawText("PRESS Q TO RETURN", S_W / 2 - 230, S_H / 2 + 100, 50, MAROON);
}

// =============================================================================
// MAIN UPDATE/DRAW LOOP
// =============================================================================
void UpdateDrawFrame()
{
    // Update
    switch (currentScreen)
    {
    case TITLE:
        UpdateTitle();
        break;
    case LEVEL_SELECT:
        UpdateLevelSelect();
        break;
    case LEVEL_1:
        UpdateLevel1();
        break;
    case PAUSE:
        UpdatePause();
        break;
    case GAME_OVER:
        UpdateGameOver();
        break;
    case UNDER_DEVELOPMENT:
        UpdateUnderDevelopment();
        break;
    }

    // Draw
    BeginDrawing();
    switch (currentScreen)
    {
    case TITLE:
        DrawTitle();
        break;
    case LEVEL_SELECT:
        DrawLevelSelect();
        break;
    case LEVEL_1:
        DrawLevel1();
        break;
    case PAUSE:
        DrawPause();
        break;
    case GAME_OVER:
        DrawGameOver();
        break;
    case UNDER_DEVELOPMENT:
        DrawUnderDevelopment();
        break;
    }
    EndDrawing();
}

// =============================================================================
// MAIN ENTRY POINT
// =============================================================================
int main() 
{
    S_W = 1920;
    S_H = 1080;
    
    InitWindow(S_W, S_H, "DARK REFLECTION");
    InitAudioDevice();
    
    // Load all resources once
    InitGame();
    
    SetTargetFPS(60);

    #ifdef __EMSCRIPTEN__
    // Web: Give control back to browser each frame
    emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
    #else
    // Desktop: Standard game loop
    while (!WindowShouldClose())
    {
        UpdateDrawFrame();
    }
    #endif

    CloseAudioDevice();
    CloseWindow();
    return 0;
}