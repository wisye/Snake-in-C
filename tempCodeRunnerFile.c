        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW+1));

            // Draw a black pixel representing the snake
            SetPixel(hdc, 10, 10, RGB(0, 0, 0));  // RGB(0, 0, 0) is black

            EndPaint(hwnd, &ps);
            break;
        }