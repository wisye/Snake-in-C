#include <windows.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#define WIDTH 800
#define HEIGHT 600
#define QUEUE_SIZE 100
#define SNAKE_MAX_LENGTH 650

typedef struct{
    int x;
    int y;
} Point;

typedef struct{
    Point position;
} Food;

typedef enum {
    UP,
    DOWN,
    LEFT,
    RIGHT
} Direction;

typedef struct{
    Point body[SNAKE_MAX_LENGTH];
    int length;
    Direction direction;
} Snake;

Direction queue[QUEUE_SIZE];
int head = 0;
int tail = 0;

void enqueue(Direction direction) {
    queue[tail] = direction;
    tail = (tail + 1) % QUEUE_SIZE;
}

Direction dequeue() {
    Direction direction = queue[head];
    head = (head + 1) % QUEUE_SIZE;
    return direction;
}

bool isQueueEmpty() {
    return head == tail;
}

Food food;
Snake snake = {
    .body = {{100, 100}},
    .length = 1,
    .direction = RIGHT,
};

void spawnFood() {
    bool overlap;
    do {
        overlap = false;
        food.position.x = (rand() % ((WIDTH - 200) / 20)) * 20;
        food.position.y = (rand() % ((HEIGHT - 200) / 20)) * 20;

        for (int i = 0; i < snake.length; i++) {
            if (snake.body[i].x == food.position.x && snake.body[i].y == food.position.y) {
                overlap = true;
                break;
            }
        }
    } while (overlap);
}

void moveSnake() {
    if (!isQueueEmpty()) {
        snake.direction = dequeue();
    }

    Point newHead = snake.body[0];

    switch (snake.direction) {
        case UP:
            newHead.y -= 20;
            if (newHead.y < 0)
                newHead.y = HEIGHT - 20;
            break;
        case DOWN:
            newHead.y += 20;
            if (newHead.y >= HEIGHT)
                newHead.y = 0;
            break;
        case LEFT:
            newHead.x -= 20;
            if (newHead.x < 0)
                newHead.x = WIDTH - 20;
            break;
        case RIGHT:
            newHead.x += 20;
            if (newHead.x >= WIDTH)
                newHead.x = 0;
            break;
    }

    for (int i = snake.length - 1; i > 0; i--) {
        snake.body[i] = snake.body[i - 1];
    }

    snake.body[0] = newHead;

    if (snake.body[0].x == food.position.x && snake.body[0].y == food.position.y) {
        snake.length++;
        spawnFood();
    }

    for (int i = 1; i < snake.length; i++) {
        if (snake.body[i].x == newHead.x && snake.body[i].y == newHead.y) {
            PostQuitMessage(0);
            return;
        }
    }
}

LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
    switch(msg){
        case WM_CREATE:
            SetTimer(hwnd, 1, 200, NULL);
            break;

        case WM_TIMER:
            moveSnake();
            InvalidateRect(hwnd, NULL, TRUE);
            break;

        case WM_KEYDOWN:
            switch (wParam) {
                case VK_UP:
                    enqueue(UP);
                    break;
                case VK_DOWN:
                    enqueue(DOWN);
                    break;
                case VK_LEFT:
                    enqueue(LEFT);
                    break;
                case VK_RIGHT:
                    enqueue(RIGHT);
                    break;
            }
            break;

        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW+1));

            HBRUSH blackBrush = CreateSolidBrush(RGB(0, 0, 0));
            for (int i = 0; i < snake.length; i++) {
                RECT rectSnake;
                rectSnake.left = snake.body[i].x;
                rectSnake.top = snake.body[i].y;
                rectSnake.right = rectSnake.left + 15;
                rectSnake.bottom = rectSnake.top + 15;
                FillRect(hdc, &rectSnake, blackBrush);
            }

            RECT rectFood;
            rectFood.left = food.position.x;
            rectFood.top = food.position.y;
            rectFood.right = rectFood.left + 15;
            rectFood.bottom = rectFood.top + 15;
            HBRUSH redBrush = CreateSolidBrush(RGB(255, 0, 0));
            FillRect(hdc, &rectFood, redBrush);

            DeleteObject(blackBrush);
            DeleteObject(redBrush);

            EndPaint(hwnd, &ps);
            break;
        }

        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR args, int ncmdshow){
    WNDCLASSW wc = {0};
    wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hInstance = hInst;
    wc.lpszClassName = L"Snake";
    wc.lpfnWndProc = WindowProcedure;
    RegisterClassW(&wc);
    CreateWindowW(L"Snake", L"Snake", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 200, 200, WIDTH, HEIGHT, NULL, NULL, NULL, NULL);
    srand(time(NULL));
    spawnFood();
    MSG msg = {0};
    while(GetMessage(&msg, NULL, 0, 0)){
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}