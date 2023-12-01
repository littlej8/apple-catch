#include "raylib.h"
#include "bucket.h"
#include "apple.h"
#include "__stack_chk_fail.h"

#define SCREEN_WIDTH 650
#define SCREEN_HEIGHT 800

#define IMAGE_WIDTH 256
#define IMAGE_HEIGHT 256
#define IMAGE_RECT CLITERAL(Rectangle){0, 0, IMAGE_WIDTH, IMAGE_HEIGHT}

#define MAX_APPLES 16
#define MIN_FALLSPEED 150
#define MAX_FALLSPEED 250
#define TIME_BETWEEN_APPLES 1.0f
#define MAX_LIVES 3

void Init(void);
void Update(void);

void DrawStartScreen(void);

void DrawHUD(void);
void DrawBasket(void);
void DrawApples(void);

Vector2 GetBasketPos(void);

void UpdateApples(void);
void SetApple(void);
void UnsetApple(int idx);

Texture2D _basket_image;
Texture2D _apple_image;

struct Apple {
    Vector2 position;
    int fall_speed;

    int active;
};
struct Apple _apples[MAX_APPLES] = {0};

enum GameState {
    ACTIVE,
    END
};
enum GameState _state = END;

float _apple_timer;
float _game_time;
int _score;
int _lives = MAX_LIVES;

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Apple Catch");
    SetTargetFPS(60);

    Init();

    while (!WindowShouldClose())
    {
        Update();
    }

    CloseWindow();

    return 0;
}

void Init(void)
{
    Image basket = {0};
    basket.format = BUCKET_FORMAT;
    basket.height = BUCKET_HEIGHT;
    basket.width = BUCKET_WIDTH;
    basket.data = BUCKET_DATA;
    basket.mipmaps = 1;
    _basket_image = LoadTextureFromImage(basket);

    Image apple = {0};
    apple.format = APPLE_FORMAT;
    apple.height = APPLE_HEIGHT;
    apple.width = APPLE_WIDTH;
    apple.data = APPLE_DATA;
    apple.mipmaps = 1;
    _apple_image = LoadTextureFromImage(apple);
    
    _apple_timer = TIME_BETWEEN_APPLES;
}

void Update(void)
{   
    if (_state == END)
    {   
        if (GetKeyPressed() != KEY_NULL)
        {
            _state = ACTIVE;
            _score = 0;
            _game_time = 0;
            _lives = MAX_LIVES;

            for (int i = 0; i < MAX_APPLES; i++)
            {
                UnsetApple(i);
            }
        }

        BeginDrawing();
            ClearBackground(RAYWHITE);

            DrawStartScreen();
        EndDrawing();
        return;
    }

    _apple_timer -= GetFrameTime();
    if (_apple_timer <= 0)
    {
        _apple_timer = TIME_BETWEEN_APPLES;
        SetApple();
    }

    _game_time += GetFrameTime();

    UpdateApples();

    if (_lives <= 0)
    {
        _state = END;
    }

    BeginDrawing();

        ClearBackground(RAYWHITE);

        DrawBasket();
        DrawApples();
        DrawHUD();

    EndDrawing();
}

void DrawStartScreen(void)
{
    const char *title = "Apple Catch";
    const char *desc = "Press Any Key To Start";

    DrawText(title, SCREEN_WIDTH/2 - MeasureText(title, 72)/2, SCREEN_HEIGHT/3, 72, DARKGRAY);
    DrawText(desc, SCREEN_WIDTH/2 - MeasureText(desc, 48)/2, SCREEN_HEIGHT/2, 48, DARKGRAY);
}

void DrawHUD(void)
{
    DrawText(TextFormat("Score: %d", _score), 10, 0, 48, DARKGRAY);

    int minutes = (int)_game_time / 60;
    int seconds = (int)_game_time % 60;
    const char *fmt = (seconds < 10) ? "%d:0%d" : "%d:%d";
    DrawText(TextFormat(fmt, minutes, seconds), 20, 50, 24, DARKGRAY);

    DrawText(TextFormat("Lives: %d", _lives), 10, 75, 48, RED);
}

void DrawBasket(void)
{
    DrawTextureEx(_basket_image, GetBasketPos(), 0, 1.5, WHITE);
}

void DrawApples(void)
{
    for (int i = 0; i < MAX_APPLES; i++)
    {
        if (_apples[i].active != 1)
        {
            continue;
        }

        Vector2 pos = {_apples[i].position.x - IMAGE_WIDTH/4, _apples[i].position.y - IMAGE_HEIGHT/4};
        DrawTextureEx(_apple_image, pos, 0, 0.5, WHITE);
    }
}

Vector2 GetBasketPos(void)
{
    return (Vector2){GetMousePosition().x - IMAGE_WIDTH * 1.5 / 2, SCREEN_HEIGHT - IMAGE_HEIGHT*1.25};
}

void UpdateApples(void)
{
    for (int i = 0; i < MAX_APPLES; i++)
    {
        if (_apples[i].active != 1)
        {
            continue;
        }
        _apples[i].position.y += _apples[i].fall_speed * GetFrameTime();

        if (_apples[i].position.y > SCREEN_HEIGHT + IMAGE_HEIGHT/2)
        {
            UnsetApple(i);
            _lives--;
        }

        if (_apples[i].position.y >= GetBasketPos().y + IMAGE_HEIGHT/3 &&
            _apples[i].position.y <= GetBasketPos().y + IMAGE_HEIGHT &&
            _apples[i].position.x >= GetBasketPos().x + IMAGE_WIDTH*0.2 &&
            _apples[i].position.x <= GetBasketPos().x + IMAGE_WIDTH*1.2)
        {
            UnsetApple(i);
            _score++;
        }
    }
}

void SetApple(void)
{
    for (int i = 0; i < MAX_APPLES; i++)
    {
        if (_apples[i].active == 0)
        {
            _apples[i].active = 1;
            _apples[i].position = (Vector2){GetRandomValue(0 + IMAGE_WIDTH/4, SCREEN_WIDTH - IMAGE_WIDTH/2), 0 - IMAGE_HEIGHT};
            _apples[i].fall_speed = GetRandomValue(MIN_FALLSPEED, MAX_FALLSPEED);

            break;
        }
    }
}

void UnsetApple(int idx)
{
    _apples[idx].active = 0;
}