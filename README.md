# Solar System Scene Engine

Solar System Scene Engine involves developing a mini scene graph-based 3D engine. It is divided into four phases, each building upon the previous one. The aim is to create a robust engine capable of rendering complex 3D scenes using various graphical primitives, geometric transformations, and textures.

<img src="https://i.ibb.co/VD6hG11/image.png">

## Setup and Installation (Assuming the models are generated)

2. **Install the necessary dependencies:**
    - libglew-dev
    - freeglut3-dev
    - libdevil-dev
    - CMake

2. **Clone the repository:**
    ```bash
    git clone https://github.com/joaobaptista03/UMinho-SolarSystem-Scene-Engine-CPP
    ```

3. **Change directory to the cloned repo, and to one of the Phases, and then to its Engine folder:**
   ```bash
   cd UMinho-SolarSystem-Scene-Engine-CPP/Phase4/Engine
   ```

3. **Use CMake to compile the Engine (Engine/main.cpp + Engine/CMakeLists.txt):**

4. **Compile using make and run the app:**
   ```bash
   make -C build && build/engine xml.xml
   ```
   
## Phases

### Phase 1: Graphical Primitives

In this phase, the focus is on creating basic graphical primitives and the engine to render them. Two applications are developed: a generator and the engine.

#### Generator
The generator creates model files with vertices information for various graphical primitives:
- **Plane**: A square in the XZ plane, centered at the origin.
- **Box**: Centered at the origin, requires dimensions and number of divisions per edge.
- **Sphere**: Centered at the origin, requires radius, slices, and stacks.
- **Cone**: Base in the XZ plane, requires bottom radius, height, slices, and stacks.

**Example Commands**:
```bash
generator sphere 1 10 10 sphere.3d
generator box 2 3 box.3d
generator cone 1 2 4 3 cone.3d
generator plane 1 3 plane.3d
```

#### Engine

The engine reads a configuration XML file containing camera settings and model files to load.

**Example XML configuration**:
```
<world>
    <window width="512" height="512" />
    <camera>
        <position x="3" y="2" z="1" />
        <lookAt x="0" y="0" z="0" />
        <up x="0" y="1" z="0" />
        <projection fov="60" near="1" far="1000" />
    </camera>
    <group>
        <models>
            <model file="plane.3d" />
            <model file="cone.3d" />
        </models>
    </group>
</world>
```

### Phase 2: Geometric Transforms

This phase involves creating hierarchical scenes using geometric transforms. A scene is defined as a tree where each node contains geometric transforms and optionally models.

**Example XML configuration**:

```
<world>
    <window width="512" height="512" />
    <camera>
        <position x="10" y="10" z="10" />
        <lookAt x="0" y="0" z="0" />
        <up x="0" y="1" z="0" />
        <projection fov="60" near="1" far="1000" />
    </camera>
    <group>
        <transform>
            <translate x="4" y="0" z="0" />
            <rotate angle="30" x="0" y="1" z="0" />
            <scale x="2" y="0.3" z="1" />
        </transform>
        <models>
            <model file="cone.3d" />
            <model file="plane.3d" />
        </models>
    </group>
</world>
```

### Phase 3: Curves, Cubic Surfaces, and VBOs

This phase introduces Bezier patches and animations based on Catmull-Rom cubic curves. The engine is updated to use Vertex Buffer Objects (VBOs) for rendering.

**Bezier Patches Example:**:

```
generator bezier patch_file.bez 10 patch.3d
```

**Example XML configuration**:

```
<translate time="10" align="True">
    <point x="1" y="0" z="1" />
    <point x="0.707" y="0.707" z="1" />
    <point x="0" y="1" z="1" />
    <point x="-1" y="0" z="1" />
</translate>
<rotate time="10" x="0" y="1" z="0" />
```

### Phase 4: Normals and Texture Coordinates

In this final phase, the generator application is enhanced to include texture coordinates and normals for each vertex. The engine activates lighting and texturing functionalities, reading and applying normals and texture coordinates from model files.

**Textured model example**:

```
<models>
    <model file="sphere.3d">
        <texture file="earth.jpg" />
        <color>
            <diffuse R="200" G="200" B="200" />
            <ambient R="50" G="50" B="50" />
            <specular R="0" G="0" B="0" />
            <emissive R="0" G="0" B="0" />
            <shininess value="0" />
        </color>
    </model>
</models>
```

**Lighting Example**:

```
<lights>
    <light type="point" posX="0" posY="10" posZ="0" />
    <light type="directional" dirX="1" dirY="1" dirZ="1"/>
    <light type="spotlight" posX="0" posY="10" posZ="0" dirX="1" dirY="1" dirZ="1" cutoff="45" />
</lights>
```
