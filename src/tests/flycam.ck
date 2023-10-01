public class FlyCam {

	CglCamera @ mainCamera;
	InputManager @ IM;
	MouseManager @ MM;

	fun void init(InputManager @ im, MouseManager @ mm, CglCamera @ cam) {
		im @=> this.IM;
		mm @=> this.MM;
		cam @=> this.mainCamera;
	}

	fun void init(InputManager @ im, MouseManager @ mm) {
		im @=> this.IM;
		mm @=> this.MM;
		CGL.mainCam() @=> this.mainCamera;
	}

	@(0.0, 1.0, 0.0) => vec3 UP;
	@(1.0, 0.0, 0.0) => vec3 RIGHT;
	fun void update(time t, dur dt)
	{
		this.update(dt/second);
	}

	fun void update(float dt) {
		2.5 * dt => float cameraSpeed;
		// camera movement
		if (IM.isKeyDown(IM.KEY_W))
			mainCamera.TranslateBy(cameraSpeed * mainCamera.GetForward());
		if (IM.isKeyDown(IM.KEY_S))
			mainCamera.TranslateBy(-cameraSpeed * mainCamera.GetForward());
		if (IM.isKeyDown(IM.KEY_D))
			mainCamera.TranslateBy(cameraSpeed * mainCamera.GetRight());
		if (IM.isKeyDown(IM.KEY_A))
			mainCamera.TranslateBy(-cameraSpeed * mainCamera.GetRight());
		if (IM.isKeyDown(IM.KEY_Q))
			mainCamera.TranslateBy(cameraSpeed * UP);
		if (IM.isKeyDown(IM.KEY_E))
			mainCamera.TranslateBy(-cameraSpeed * UP);

		// <<< "pos", mainCamera.GetPosition() >>>;
		// <<< "rot", mainCamera.GetRotation() >>>;

		// mouse lookaround

		.001 => float mouseSpeed;
		MM.GetDeltas() * mouseSpeed => vec3 mouseDeltas;

		// <<< mouseDeltas >>>;

		// for mouse deltaY, rotate around GetRight axis
		mainCamera.RotateOnLocalAxis(RIGHT, -mouseDeltas.y);

		// for mouse deltaX, rotate around (0,1,0)
		mainCamera.RotateOnWorldAxis(UP, -mouseDeltas.x);
	}

	fun void selfUpdate() {
		<<< "flycam: selfUpdate mode" >>>;
		now => time lastTime;
		CglUpdate UpdateEvent;
		while (true) {
			// time bookkeeping
			now - lastTime => dur deltaTime;
			now => lastTime;

			update(now, deltaTime);

			// signal ready for next update
			// UpdateEvent => now;
			CGL.nextFrame() => now;
		}
	}

}