#include <iostream>
#include <string>
#include <vector>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/XKBlib.h>

using namespace std;

vector<int> keysPressed;
vector<string> modifierNames = { "Shift_L", "Control_L", "Alt", "Super_L" };

void printKeyCombo(XEvent comboEvent, Display *comboDisplay) {
	string modifierList = "";
	string lastModifier = "";
	try {
		XKeyEvent *keyEvent = reinterpret_cast<XKeyEvent*>(&comboEvent);
		char keyName[256];
		XLookupString(keyEvent, keyName, sizeof(keyName), nullptr, nullptr);
		keysPressed.push_back(keyEvent->keycode);
		bool shiftPressed = false;
		for (int i = 0; i < modifierNames.size(); i++) {
			if (keyEvent->state & (1 << i)) {
				modifierList += modifierNames[i] + " + ";
				if (modifierNames[i] == "Control_L") {
					lastModifier = "Control_L";
				} else if (modifierNames[i] == "Alt") {
					lastModifier = "Alt";
				}
			}
		}
		if (keyEvent->state & ShiftMask) {
			shiftPressed = true;
		}
		if (shiftPressed) {
			if (lastModifier == "Control_L") {
				cout << "Control_L + Shift_L + " + string(keyName) << endl;
			} else if (lastModifier == "Alt") {
				cout << "Alt + Shift_L + " + string(keyName) << endl;
			} else {
				cout << "Shift_L + " + modifierList + string(keyName) << endl;
			}
		} else {
			cout << modifierList + string(keyName) << endl;
		}
	} catch (const exception &e) {
		cerr << "Error: " << e.what() << "\n";
	} catch (...) {
		cerr << "Unknown error occurred\n";
	}
}

int main() {
//Get the current X11 session
	Display *display = XOpenDisplay(nullptr);
	cout << "Display Opened: " << display << endl;
	if (!display) {
		cerr << "Error: Could not open X display." << endl;
		return 1;
	}
// Create the window
	int screen = DefaultScreen(display);
	Window window = XCreateSimpleWindow(display, RootWindow(display, screen), 0,
			0, 400, 400, 0, BlackPixel(display, screen),
			WhitePixel(display, screen));
	XSelectInput(display, window, KeyPressMask);
//Map the root window to display it on the screen
	XMapWindow(display, window);
	XStoreName(display, window, "Key Window");
//Listen for events on the window
	XEvent event;
	while (true) {
		XNextEvent(display, &event);
		if (event.type == KeyPress) {
			XKeyEvent *keyEvent = reinterpret_cast<XKeyEvent*>(&event);
			if (keyEvent->keycode == XKeysymToKeycode(display, XK_Escape)) {
				break;
			}
			printKeyCombo(event, display);
			keysPressed.clear();
		}
	}
	XCloseDisplay(display);
	return 0;
}
