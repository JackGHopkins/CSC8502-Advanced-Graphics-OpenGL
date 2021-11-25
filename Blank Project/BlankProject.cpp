#include "../NCLGL/window.h"
#include "Renderer.h"

int main()	{
	Window w("Make your own project!", 1280, 720, false);

	if(!w.HasInitialised()) {
		return -1;
	}
	
	Renderer renderer(w);
	if(!renderer.HasInitialised()) {
		return -1;
	}

	w.LockMouseToWindow(true);
	w.ShowOSPointer(false);

	while(w.UpdateWindow()  && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE)){
		renderer.UpdateScene(w.GetTimer()->GetTimeDeltaSeconds());
		renderer.RenderScene();
		renderer.SwapBuffers();
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_F1)) {
			w.LockMouseToWindow(false);
			w.ShowOSPointer(true);
		}		if (Window::GetKeyboard()->KeyDown(KEYBOARD_F2)) {
			w.LockMouseToWindow(true);
			w.ShowOSPointer(false);
		}
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_F3)) {
			renderer.PrintCoord();
		}
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_F5)) {
			Shader::ReloadAllShaders();
		}
		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_1)) {
			std::cout << "Came1ra Auto Toggled.";
			renderer.ToggleAutomaticCamera();
			if (!renderer.automaticCamera) {
				w.LockMouseToWindow(true);
				w.ShowOSPointer(false);
			}
			else {
				w.LockMouseToWindow(false);
				w.ShowOSPointer(true);
			}
		}
	}
	return 0;
}