# Scrub squad

A video game developed in our own game engine. Submitted for [ZTGK](https://gry.it.p.lodz.pl/main/index.php/en/) in the development category and placed as a finalist. 

Download the [final build](https://github.com/ReasonPsycho/ZTGK/releases/latest) submited to ZTGK.
[Itch.io page.](https://bubble-bliss-games.itch.io/scrub-squad)

Our project had to complie with the requirements for game development category:

- *No Pre-Made Game Engines:* We built the game without using any ready-to-use engines, relying only on frameworks.
- *Framework Usage:* The framework was used solely for low-level systems like rendering, input, memory management and file parsing.
- *Custom Implementations:* We developed all key systems, ECS, Rendering, Lighting, AI, scene management, gameplay, advanced shaders, and scripting.
- *Asset Licensing:* All assets used are original or properly licensed. 

This project showcases our work in creating a custom engine and core systems from the ground up.

![Ij3Gsq](https://github.com/user-attachments/assets/5bab63e2-8ab7-4370-bcc0-65060ea5d4b7)
![0txhPl](https://github.com/user-attachments/assets/a4d3fe68-07a1-4ab9-82c3-3bd7181215bf)
![rB7afU](https://github.com/user-attachments/assets/4c983c18-9f7b-4e3e-b81f-4232665a3fe4)
![WX1Kn9](https://github.com/user-attachments/assets/39596982-bf56-463f-b4bc-277c33a3a6cf)
![U_OGTc](https://github.com/user-attachments/assets/0e635e3a-6ff8-497d-b1a8-dee1ea9ce4c0)
![ioD1OI](https://github.com/user-attachments/assets/4de6501c-a2d9-4004-bcb5-6fcadfdc8e75)

## List of major features

- **Entity Component System:** Custom component system managing game entities with unique and shared pointers for memory handling.
- **Scene Editor:** Allows real-time modification of object transforms and component attributes, as well as adding new components.
- **Tracey Profiler:** Integrated profiling to monitor performance across system components.
- **Lighting System:** Supports dynamic point lights with shadows, handling up to 40 active lights during presentation. Directional and spotlights were partially implemented.
- **Rendering System:** Renders imported **.fbx** models with material support (diffuse, specular, normal, and depth maps). Uses frustum culling, instancing, and parallax mapping for optimization.
- **Toon Shaders:** Custom toon shaders inspired by [RapidGL](https://github.com/tgalaj/RapidGL).
- **Animation System:** Multithreaded animations with forward kinematics for weapon movements.
- **Sound System:** Plays sound effects and music on demand.
- **Collision System:** Tracks object collisions and interaction rules.
- **AI System:** Manages movement and behavior of game units.
- **Grid System:** Makes implementation of rest of the system easier, and allows for optimization using chunks.
- **Event System:** Calls functions in response to specific in-game events.
- **Inventory System:** Allows unit upgrades and customization with in-game items.
- **HUD System:** Displays and updates game data in real time.  
