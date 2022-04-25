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
                
                
                
                pszWindowName
                    The window name
      Returns:  HRESULT
                  Status code
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/

    HRESULT MainWindow::Initialize(_In_ HINSTANCE hInstance, _In_ INT nCmdShow, 
        _In_ PCWSTR pszWindowName) {

        HRESULT hr = S_OK;
        m_pszWindowName = pszWindowName;
        RECT rc = { 0, 0, 800, 600 };

        hr = initialize(hInstance, nCmdShow, pszWindowName, WS_OVERLAPPEDWINDOW, 
            CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr);

        if (FAILED(hr))
        {
            return hr;
        }

        GetClientRect(m_hWnd, &rc);

        POINT p1 = { rc.left, rc.top };
        POINT p2 = { rc.right, rc.bottom };

        ClientToScreen(m_hWnd, &p1);
        ClientToScreen(m_hWnd, &p2);

        rc.left = p1.x;
        rc.top = p1.y;
        rc.right = p2.x;
        rc.bottom = p2.y;

        ClipCursor(&rc);

        return S_OK;
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

        RAWINPUTDEVICE rawInputDev[1];
        rawInputDev[0].usUsagePage = 0x01;
        rawInputDev[0].usUsage = 0x02;
        rawInputDev[0].dwFlags = RIDEV_INPUTSINK;
        rawInputDev[0].hwndTarget = m_hWnd;

        if (!RegisterRawInputDevices(rawInputDev, 1, sizeof(rawInputDev[0])))
        {
            return E_FAIL;
        }

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
        case WM_INPUT:
        {
            UINT dwSize = sizeof(RAWINPUT);
            RAWINPUT raw;

            GetRawInputData((HRAWINPUT)lParam, RID_INPUT, &raw, &dwSize, sizeof(RAWINPUTHEADER));

            if (raw.header.dwType == RIM_TYPEMOUSE)
            {
                m_mouseRelativeMovement.X = raw.data.mouse.lLastX;
                m_mouseRelativeMovement.Y = raw.data.mouse.lLastY;
            }
            return 0;
        }
        case WM_KEYDOWN:
            switch (wParam)
            {
            case VK_SPACE:
                m_directions.bUp = true;
                return 0;
            case VK_SHIFT:
                m_directions.bDown = true;
                return 0;
            case 0x57: // w
                m_directions.bFront = true;
                return 0;
            case 0x41: // a
                m_directions.bLeft = true;
                return 0;
            case 0x53: // s
                m_directions.bBack = true;
                return 0;
            case 0x44: // d
                m_directions.bRight = true;
                return 0;
            default:
                return 0;
            }
        case WM_KEYUP:
            switch (wParam)
            {
            case VK_SPACE:
                m_directions.bUp = false;
                return 0;
            case VK_SHIFT:
                m_directions.bDown = false;
                return 0;
            case 0x57:
                m_directions.bFront = false;
                return 0;
            case 0x41:
                m_directions.bLeft = false;
                return 0;
            case 0x53:
                m_directions.bBack = false;
                return 0;
            case 0x44:
                m_directions.bRight = false;
                return 0;
            default:
                return 0;
            }
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