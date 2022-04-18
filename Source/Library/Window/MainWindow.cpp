#include "Window/MainWindow.h"

namespace library
{
    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   MainWindow::Initialize
      Summary:  Initializes main window
      Args:     HINSTANCE hInstance
                  Handle to the instance
                INT nCmdShow
                    Is a flag that says whether the main application window
                    will be minimized, maximized, or shown normally
                PCWSTR pszWindowName
                    The window name
      Returns:  HRESULT
                  Status code
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/

    HRESULT MainWindow::Initialize(_In_ HINSTANCE hInstance, _In_ INT nCmdShow, 
        _In_ PCWSTR pszWindowName) {
        m_pszWindowName = pszWindowName;
        return initialize(hInstance, nCmdShow, pszWindowName, WS_OVERLAPPEDWINDOW, 0, 0, 800, 600, nullptr, nullptr);
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   MainWindow::GetWindowClassName
      Summary:  Returns the name of the window class
      Returns:  PCWSTR
                  Name of the window class
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/

    PCWSTR MainWindow::GetWindowClassName() const {
        return m_pszWindowName;
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   MainWindow::HandleMessage
      Summary:  Handles the messages
      Args:     UINT uMessage
                  Message code
                WPARAM wParam
                    Additional data the pertains to the message
                LPARAM lParam
                    Additional data the pertains to the message
      Returns:  LRESULT
                  Integer value that your program returns to Windows
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/

    LRESULT MainWindow::HandleMessage(_In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam) {

        PAINTSTRUCT ps;
        HDC hdc;

        switch (uMsg) {
        case WM_CLOSE: {
            if (MessageBox(m_hWnd, L"Really Quit?", PSZ_COURSE_TITLE, MB_OKCANCEL) == IDOK) {

                HMENU hMenu = GetMenu(m_hWnd);

                if (hMenu) {
                    DestroyMenu(hMenu);
                }

                DestroyWindow(m_hWnd);
                UnregisterClass(GetWindowClassName(), m_hInstance);
            }
            break;
        }
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        case WM_INPUT: {
            UINT uDataSize = 0u;
            GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, nullptr, &uDataSize, sizeof(RAWINPUTHEADER));

            if (uDataSize > 0) {
                std::unique_ptr<BYTE[]> rawData = std::make_unique<BYTE[]>(uDataSize);

                if (GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, rawData.get(), &uDataSize,
                    sizeof(RAWINPUTHEADER))) {

                    RAWINPUT* raw = reinterpret_cast<RAWINPUT*>(rawData.get());

                    if (raw->header.dwType == RIM_TYPEMOUSE) {
                        m_mouseRelativeMovement.X = raw->data.mouse.lLastX;
                        m_mouseRelativeMovement.Y = raw->data.mouse.lLastY;

                        WCHAR szDebugMessage[256];
                        swprintf_s(szDebugMessage, L"Mouse %ld %ld\m", m_mouseRelativeMovement.X, m_mouseRelativeMovement.Y);
                        OutputDebugString(szDebugMessage);

                        RECT rc;
                        RECT rc2;
                        POINT p1;
                        POINT p2;

                        GetWindowRect(m_hWnd, &rc2);
                        GetClientRect(m_hWnd, &rc);

                        p1.x = rc.left;
                        p1.y = rc.top;
                        p2.x = rc.right;
                        p2.y = rc.bottom;

                        ClientToScreen(m_hWnd, &p1);
                        ClientToScreen(m_hWnd, &p2);

                        rc.left = p1.x;
                        rc.top = rc2.top;
                        rc.right = p2.x;
                        rc.bottom = p2.y;

                        ClipCursor(&rc);
                    }
                }
            }

            return DefWindowProc(m_hWnd, uMsg, wParam, lParam);
        }
        case WM_KEYDOWN:
            switch (wParam) {
            case VK_SPACE:
                m_directions.bUp = TRUE;
            case VK_SHIFT :
                m_directions.bDown = TRUE;
            }
        case WM_KEYUP:
            switch (wParam) {
            case VK_SPACE:
                m_directions.bUp = FALSE;
            case VK_SHIFT:
                m_directions.bDown = FALSE;
            }
        case WM_PAINT:
            hdc = BeginPaint(m_hWnd, &ps);
            FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
            EndPaint(m_hWnd, &ps);
            break;
        default:
            return DefWindowProc(m_hWnd, uMsg, wParam, lParam);
        }

        return TRUE;
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   MainWindow::GetDirections
      Summary:  Returns the keyboard direction input
      Returns:  const DirectionsInput&
                  Keyboard direction input
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    const DirectionsInput& MainWindow::GetDirections() const {
        return m_directions;
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   MainWindow::GetMouseRelativeMovement
      Summary:  Returns the mouse relative movement
      Returns:  const MouseRelativeMovement&
                  Mouse relative movement
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    const MouseRelativeMovement& MainWindow::GetMouseRelativeMovement() const {
        return m_mouseRelativeMovement;
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   MainWindow::ResetMouseMovement
      Summary:  Reset the mouse relative movement to zero
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    void MainWindow::ResetMouseMovement() {
        m_mouseRelativeMovement.X = 0;
        m_mouseRelativeMovement.Y = 0;
    }
}