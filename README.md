# Basic Implementation

## 1. Textured Floor and Teapot with Per-Pixel Lighting
The first implementation includes a floor and teapot model, each with unique textures derived from previous lab exercises. Diffuse maps simulate realistic surface details, and the teapot's complex geometry allows for detailed pixel-by-pixel lighting effects. Vertex shaders handle vertex transformations from model to projection space, passing necessary normals and texture coordinates for lighting and texturing to the pixel shader. This stage is followed by the pixel shader, which implements per-pixel lighting with two dynamic point lights, calculating diffuse and specular lighting effects.

## 2. Textured Sphere with Vertex and Pixel Shader Effects
This implementation introduces a spherical model with vertex and pixel shaders designed to create a pulsating effect. Vertex shaders adjust vertices' positions along their normal vectors using a sinusoidal time function, simulating a breathing motion. The pixel shader continuously adjusts the texture coordinates, creating an evolving visual appearance without any lighting calculations.

## 3. Textured Cube with Texture Transition
A cube model designed to transition between two textures (brick and wood) uses a pixel shader for texture blending. The shader employs linear interpolation (lerp function) between the two textures based on a transitioning variable, resulting in a smooth texture change over time.

# Advanced Lighting Shaders

## 1. Textured Cube with Specular Map
To enhance realism and texture quality, specular maps are added, simulating different light reflection levels across the cube's surface. Areas on the specular map in white reflect more light, contributing to a high gloss, while black areas reflect no light, appearing matte.

## 2. Textured Cube with Normal Mapping
Normal mapping addresses the lack of surface detail in conventional textures by storing normal vectors at each pixel. This allows for detailed lighting effects and surface simulations, such as dents, on the cube. Additionally, dynamic color variations are applied to the texture, changing over time.

## 3. Textured Cube with Parallax Mapping
Parallax mapping improves upon normal mapping by incorporating height map information, enhancing depth perception and detail from various viewing angles. This technique shifts texture coordinates based on the viewer's perspective, simulating three-dimensional depth on flat surfaces.

## 4. Textured Troll Model with Shadow Mapping
This model integrates shadow maps to enhance depth perception and realism. Shadow maps store depth information, which helps determine shadowed regions from the camera's perspective. Additionally, spotlights with adjustable angles and distances are used to dynamically influence the shadows cast by the model.

## 5. Textured Floor Model combined with Parallax and Shadow Mapping
Combining both parallax and shadow mapping techniques, this implementation enriches the scene's realism by adding detailed textures and dynamic shadows to the floor model, simulating a more three-dimensional appearance.

## 6. Sphere Model with Render to Scene Texture
This method involves rendering parts of the scene into a texture, which can then be applied to various models, useful for creating effects like portals or mirrors. The texture captures and displays dynamic scenes, adding depth and interaction possibilities.

## 7. Cube Model with Render to Scene and TV Screen Texture
Similar to the sphere, this cube model incorporates a render to texture feature, but also includes a texture that simulates a TV screen. This content is eventually converted to grayscale, mimicking the appearance of an old television.

## 8. Textured Cube Model with Pixel Shader Effect
This cube features a texture that dynamically changes direction, creating a lively appearance. A new pixel shader manipulates the texture coordinates to produce effects such as color transitions and ripple effects, further enhanced by additive blending techniques.

## 9. Textured Cube Model with Additive Blending Technique
This model uses additive blending to seamlessly integrate the cube's edges with its textured surface, creating an effect where only the texture is visible, enhancing the visual integration with its surroundings.

## 10. Textured Cube Model with Multiplicative Blending Technique
This cube uses a multiplicative blending technique to darken scenes viewed through it, effectively blending the texture with the background, suited for simulating glass or other translucent materials.

## 11. Textured Troll Model with Cell Mapping
Cell mapping simulates a cartoon-like effect, achieved by rendering the troll model with outlines and flat colors, giving it a distinct comic book style appearance.

## 12. Bike Model with Animations
This model of a motorcycle includes movable parts like the front and rear wheels, which can be animated to simulate motion, enhancing the interactive experience.

## 13. Cube Mapping
This involves applying different textures to each face of a cube model, requiring careful setup of textures and shader resources for realistic rendering.

## 14. Complex Textured Model Crate with Shadow Mapping
This model features a complex texture combined with shadow mapping, using dynamic lighting to create realistic shadows and enhance the model's three-dimensional appearance.

# Further Notes
All models except for the textured troll with shadow mapping include controls for movement and rotation, allowing interactive scene manipulation.

