#pragma comment(lib, "nclgl.lib")

#include "../../nclgl/window.h"
#include "Renderer.h"

int main() {
	Window w("Cube Mapping! sky textures courtesy of http://www.hazelwhorley.com", 800, 600, false);
	if (!w.HasInitialised()) {
		return -1;
	}

	Renderer renderer(w);
	if (!renderer.HasInitialised()) {
		return -1;
	}

	w.LockMouseToWindow(true);
	w.ShowOSPointer(false);

	renderer.ChangeScene(0);

	while (w.UpdateWindow() && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE)) {

		if (w.GetKeyboard()->KeyTriggered(KEYBOARD_LEFT)) {
			renderer.ChangeScene(-1);
		}

		if (w.GetKeyboard()->KeyTriggered(KEYBOARD_RIGHT)) {
			renderer.ChangeScene(1);
		}

		if (w.GetKeyboard()->KeyTriggered(KEYBOARD_1)) {
			renderer.MoveAlien();
		}

		renderer.UpdateScene(w.GetTimer()->GetTimedMS());
		renderer.RenderScene();
	}

	return 0;
}