#include <raylib.h>
#include <raymath.h>
#include <stdio.h>
#include <time.h>

typedef struct IntPair {
    int width;
    int height;
} IntPair;

typedef struct Ball {
    Rectangle rec;
    Vector2 velocity;
} Ball;

static void DrawMiddleBarrier(IntPair screen) {
    static const int size = 10;
    static const int step = 20;
    const int posX = screen.width/2-size/2;

    for (int y = 0; y < screen.height; y += step) {
        DrawRectangle(posX, y, size, size, RAYWHITE);
    }
}

static void DrawPaddle(IntPair screen, Rectangle paddle) {
    DrawRectangleRec(paddle, RAYWHITE);
}

static void UpdatePaddle(IntPair screen, Rectangle* paddle, float speed) {
    paddle->y = Clamp(paddle->y + speed, 0.f, screen.height-paddle->height);
}

static Vector2 RandomStartAngle(float speed) {
    int angle = GetRandomValue(-45, 45);
    const int flip = GetRandomValue(0, 1);
    if (flip) angle += 180;
    const float startVelocity = speed;

    const Vector2 velocity = {
        startVelocity * cosf(angle*DEG2RAD),
        startVelocity * sinf(angle*DEG2RAD)
    };

    return velocity;
}

static Vector2 GetBounceVelocity(Ball ball, Rectangle paddle) {
    const float diff = (ball.rec.y+ball.rec.height/2.) - (paddle.y+paddle.height/2.f);
    const float speed = sqrtf(ball.velocity.x * ball.velocity.x + ball.velocity.y * ball.velocity.y);
    const float maxDy = fabsf(ball.rec.height/2.f + paddle.height/2.f);

    float angle;
    if (ball.velocity.x < 0.f) {
        angle = Remap(diff, -maxDy, maxDy, -45.f, 45.f);
    } else {
        angle = Remap(diff, maxDy, -maxDy, 135.f, 225.f);
    }

    Vector2 newVelocity = { speed * cosf(angle*DEG2RAD), speed * sinf(angle*DEG2RAD) };
    return newVelocity;
}

typedef enum GameScreen { WELCOME, GAMEPLAY, PAUSE } GameScreen;

int main(void) {
    const IntPair screen = { 800, 500 };
    const int scoreFontSize = 50;
    const Vector2 origin = { screen.width/2.f, screen.height/2.f };

    // Initialization
    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT);
    InitWindow(screen.width, screen.height, "Pong");
    SetWindowState(FLAG_WINDOW_UNDECORATED);

    Shader crtShader = LoadShader(NULL, "resources/shaders/shader.frag");
    RenderTexture2D target = LoadRenderTexture(screen.width, screen.height);

    SetTargetFPS(60);
    SetRandomSeed(time(NULL));

    const float paddleSpeed = 10.f;
    const Vector2 paddleSize = { 10.f, 60.f };
    const float paddlePadding = 20.f;

    Rectangle rightPaddle = {
        screen.width-paddlePadding-paddleSize.x,
        origin.y-paddleSize.y/2.f,
        paddleSize.x,
        paddleSize.y
    };

    Rectangle leftPaddle = {
        paddlePadding,
        origin.y-paddleSize.y/2.f,
        paddleSize.x,
        paddleSize.y
    };

    const Rectangle initBallRec = {
        origin.x-5.f,
        origin.y-5.f,
        10.f, 10.f
    };

    const float initBallSpeed = 5.f;
    Ball ball = { initBallRec, RandomStartAngle(initBallSpeed) };

    bool paused = false;
    unsigned int rightScore = 0;
    unsigned int leftScore = 0;
    unsigned long frameCounter = 0;

    // Main game loop
    while (!WindowShouldClose()) {
        // Update --------------------------------------------------------------
        if (IsKeyPressed(KEY_P)) {
            paused = !paused;
        }

        if (IsKeyDown(KEY_UP))
            UpdatePaddle(screen, &rightPaddle, -paddleSpeed);
        if (IsKeyDown(KEY_DOWN))
            UpdatePaddle(screen, &rightPaddle, paddleSpeed);
        if (IsKeyDown(KEY_W))
            UpdatePaddle(screen, &leftPaddle, -paddleSpeed);
        if (IsKeyDown(KEY_S))
            UpdatePaddle(screen, &leftPaddle, paddleSpeed);

        // Reset
        if (IsKeyReleased(KEY_R)) {
            ball = (Ball){ initBallRec, RandomStartAngle(5.f) };
            rightScore = 0;
            leftScore = 0;
        }

        if (!paused) {
            Vector2 newBallPos = {
                ball.velocity.x + ball.rec.x,
                ball.velocity.y + ball.rec.y
            };

            if ((newBallPos.y < 0.f || (newBallPos.y > screen.height - ball.rec.height))) {
                ball.velocity.y *= -1.f;
                newBallPos.y = ball.velocity.y + ball.rec.y;
            }

            // Update points
            if (newBallPos.x + ball.rec.width < 0.f) {
                rightScore++;
                ball.velocity = RandomStartAngle(initBallSpeed);
                newBallPos = (Vector2){ initBallRec.x, initBallRec.y };
            } else if (newBallPos.x > screen.width) {
                leftScore++;
                ball.velocity = RandomStartAngle(initBallSpeed);
                newBallPos = (Vector2){ initBallRec.x, initBallRec.y };
            }

            if (CheckCollisionRecs(rightPaddle, ball.rec)) {
                ball.velocity = GetBounceVelocity(ball, rightPaddle);
                ball.rec.x = rightPaddle.x - ball.rec.width;
                newBallPos = Vector2Add(ball.velocity, (Vector2){ ball.rec.x, ball.rec.y });
            } else if (CheckCollisionRecs(leftPaddle, ball.rec)) {
                ball.velocity = GetBounceVelocity(ball, leftPaddle);
                ball.rec.x = leftPaddle.x + leftPaddle.width;
                newBallPos = Vector2Add(ball.velocity, (Vector2){ ball.rec.x, ball.rec.y });
            }

            ball.rec.x = newBallPos.x;
            ball.rec.y = newBallPos.y;
            frameCounter++;
            if (frameCounter % 60 == 0) {
                // Gradually increase difficulty by increasing speed by 1% every second
                ball.velocity = Vector2Scale(ball.velocity, 1.01f);
            }
        }

        // Draw ----------------------------------------------------------------
        BeginTextureMode(target);
            // Background
            ClearBackground(BLACK);
            DrawMiddleBarrier(screen);

            // Paddles
            DrawPaddle(screen, rightPaddle);
            DrawPaddle(screen, leftPaddle);

            // Scores
            char rightScoreStr[10] = {0};
            char leftScoreStr[10] = {0};
            snprintf(rightScoreStr, 9, "%u", rightScore);
            snprintf(leftScoreStr, 9, "%u", leftScore);
            int leftScoreW = MeasureText(leftScoreStr, scoreFontSize);

            DrawText(rightScoreStr, screen.width/2+40, 30, scoreFontSize, RAYWHITE);
            DrawText(leftScoreStr, screen.width/2-40-leftScoreW, 30, scoreFontSize, RAYWHITE);

            // Ball
            DrawRectangleRec(ball.rec, RAYWHITE);

            if (paused) {
                const int fontSize = 40;
                const int width = MeasureText("PAUSE", fontSize);
                const unsigned char intensity =
                    floorf(Remap((sin(GetTime()*2.f)+1.f)*0.5f, 0.f, 1.f, 0.f, 245.f));

                Color recColor = BLACK;
                recColor.a = intensity;
                DrawRectangle(screen.width/2-width/2-5, screen.height/2-fontSize/2-5, width+10, fontSize+10, recColor);

                Color textColor = (Color){ intensity, intensity, intensity, 255 };
                DrawText("PAUSE", screen.width/2-width/2, screen.height/2-fontSize/2, fontSize, textColor);
            }
        EndTextureMode();

        BeginDrawing();
            ClearBackground(BLACK);

            BeginShaderMode(crtShader);
                DrawTextureRec(
                    target.texture,
                    (Rectangle){
                        0, 0,
                        (float)target.texture.width,
                        (float)-target.texture.height
                    },
                    (Vector2){ 0, 0 },
                    WHITE
                );
            EndShaderMode();
        EndDrawing();
    }

    UnloadShader(crtShader);
    CloseWindow();

    return 0;
}
