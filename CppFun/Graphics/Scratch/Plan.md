Split-safe

Do something different. 

ECS architecture to enable "easy" multi-threading for rendering systems.

Threadable systems
Logging
Collision
Camera

Hierarchical systems might need to be effectively single-threaded. 
- UI
- Animation

Command buffers to execute in Command batches. Batch items can be reorganized

Multi-threaded systems
Easy scripting. When you do something like ```Instantiate(Raycast(origin, direction))```, commands are created and executed on the next batch. They are ordered based on systems i.e. Physics for raycast, 


