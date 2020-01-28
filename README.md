# OpenGL Examples
Some projects that I completed in CTIS 164 - Technical Mathematics with Programming course, first year. (OpenGL, C, GLUT)

Reqired library: sudo apt-get install mesa-utils freeglut3-dev  
To compile (example): gcc freefall-ball-simulation.cpp -o fall -lGL -lGLU -lglut -lm

## Lighting Simulation
User can select one of two types of light source namely point light source and spotlight light source. The light source swings like a pendulum. The surface is modeled by many circles with the radius of 10 pixels. The colors of the circles are calculated based on the light source properties. The color of the light source is white. The brightness (sum of red, green and blue) is represented by arrows. For the spotlight, cut-off/field of view angle is 30 degree.

![a gif of the project running](https://media.giphy.com/media/fnjxjG5uqWhEwCp4s2/giphy.gif)

## Free Falling Ball Simulation

It is a physical simulation that dynamically produces a surface, and when you drop a ball with a mouse click, it freefalls due to the gravity and stops at the surface. F1 key is used to change the surface. The surface is composed of many line segments whose y-coordinates are randomly generated. Random y-coordinates are between 0 and -200. Random points are marked with green circles. The surface is drawn by connecting those green points in red color with a thickness of 3. The distance between adjacent two green points is 50 pixels in x coordinate. Also, below the surface is filled with white vertical lines. The distance between two white vertical lines is 20 pixels. The top coordinate of each white vertical line is marked with a white circle.

![a gif of the project running](https://media.giphy.com/media/LwHbQT7ypoMlLbboR8/giphy.gif)

## Speedometer Simulation

It is a speedometer with a rotating speed indicator. The shape of the speedometer is drawn using trigonometry. Up arrow key is used to increase speed, and rotates the indicator. It displays the current speed. Minimum speed is 0 and maximum is 320. If you release up arrow key, speed automatically decreases.

![a gif of the project running](https://media.giphy.com/media/37R9i7tUmYSl0zWqRz/giphy.gif)
