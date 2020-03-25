# Computer-Graphics---Competition-OpenGL

This is a simulation of the solar system.

Planets/sun rotate at different speeds and directions, and planets orbit at different speeds. The values used in the simulation are taken from the Planetary Fact Sheet of NASA. Flying cat spaceships travel in different directions and speeds. They are navigating to a planet and upon arrival go on a new adventure. There is one light source positioned at (0,0,0), which is the sun. The camera is looking from one object to another.

Controls:
* Look from: dropdown to select from which object the camera is looking
* Look at: dropdown to select which object the camera is looking at
	- You can look from an object to itself: the camera will look along the +z-axis
* Zoom: zoom in or out by changing the field of view
* Height: how far away the camera is from the center of the object it is looking at
* Angle: angle relative to the view direction.
	- Examples:
		- Angle = 0: the camera is behind LookFrom, so LookFrom lies directly in the line of sight with LookAt
		- Angle - 90: the camera is exactly below LookFrom
		- Angle = 180: the camera is in front of lookFrom, looking directly at LookAt with LookFrom behind the camera
		- Angle = 270: the camera is exactly above lookFrom
* Time: slider to make time go faster/slower
* The "eye" object is there to give a better view of the whole solar system
