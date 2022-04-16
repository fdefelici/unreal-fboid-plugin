# FBoid Plugin
A boid implementation as an Uneal Engine Plugin, running with Compute Shaders 

![simulation](./_docs/boids.gif)

> Even if FBoid is a working Plugin for Unreal Engine, and allow you to configure your own boid asset, spawner, and flock behaviour, the aim of this project is to be used as a playground on exploring Unreal Engine feature related to plugin development.

# Features
Here a list of the main feature offered by the plugin:
* Use your own Asset / Actor to generate a "Boidized" Actor
* Configure Flock behaviour in terms of: spawning, speed, alignment, cohesion, separation, obstacle avoidance, and more
* In Editor boid sensors visualizer (in PIE and in Boid Actor Blueprint Viewport)
* Integrated with Unreal physics system 
* Flock simulation run on GPU using Compute Shaders

# Project Structure
The plugin project is organized with the following modules:
* BoidCore: contains the domain and core logic 
* BoidCoreGPU: contains the extension for the BoidCore related to GPU and Compute Shader implementation
* BoidEditor: contains just In-Editor features such as Asset Factory, Sensor Visualizer, and others that will not be needed at runtime.

# IDE
* Unreal Engine: 4.27