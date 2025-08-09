

# Project Get Back

This would be about a guy journey to get back to his old self,
he got separated from his true self and this game is his journey through
tribes and tribulation of conquering the sins of the world to reach his goal 
of getting back his true self but on his journey he slowly chips away the
dirt and rubbish he accumulated because of society and the world slowly 
reaching the goal of reaching his old self



## Game mechanics

* 3rd person
* sylized artstyle ?
* linear story 


> DEADLINE: 1 month :fire:

# Challenge
C11 engine Custom engine 

# Build project
git clone <this repo url>
git submodule update --init --recursive
windows -> ./build.bat run
linux   -> ./build.sh run


## TODOs: 

- [x] Implement a flat platform and load basic humanoid model on top of it
    - [x] Load platform
    - [x] Load humanoid model

- [x] Get camera working - looking around is an absolute mess

- [ ] Implement Kinematic motion control system modal
    - [x] Implement a better hashtable (round robin hashing) - blocker
    - [x] Load bones and weights
    - [x] Setup animation system for the model
    - [x] Implement movement for the model
    - [x] Implement a camera that follows the modal
    - [ ] Include player rotation over mouse movement
    - [ ] Setup Debug view of the bones and weights for models

- [ ] Helper UI in debug mode
    - [x] A system to see where cameras are in world space for easy debug
    - [x] Line drawing capabilities to know where vectors are pointing to 
    - [ ] Axis visual indicators on the top right cornor of the screen
    - [x] Have mouse scroll to move the worldview camera in debug view

- [/] Setup lighting
    - [x] Diffuse
    - [x] Ambient
    - [ ] Attenuated light source
    - [ ] Specular

- [ ] Arena allocator for better memory lookup
    - [ ] Cleanup the filepaths held after heap allocated - maybe have a specified areans to handle string allocations

- [/] Design a GUI library (good one)
    - [x] Simple gui tree setup
    - [x] Margin and positioning
    - [x] button
    - [x] label
    - [ ] checkbox
    - [ ] slider
    - [ ] textbox
    - [ ] Object selection with translation and rotation
    - [ ] UI improvement
        - [ ] Rounded corners
        - [x] Padding styling
        - [x] Margin styling
        - [x] Layout (flexbox like)

- [ ] Physics system
    - [ ] Integrate animation with physics

- [ ] Setup ECS for 3d (use existing implemented setup)
    - [ ] Have each object in the scene have a vectors visually represented

- [ ] Render terrain for the player to walk in non debug mode

## Bugs
- [x] Poggen: Relative paths doesnt work with ASSIMP, (possible fix is to write a wrapper around on how filepath are given to functions)

