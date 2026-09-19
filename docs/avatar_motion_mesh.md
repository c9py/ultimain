# Avatar Motion Mesh Pipeline

Composition implemented by `engines/avatar`:

```
{ /meshy3d ( /live2d-dtecho [ /meta-echo-dna < /rig-logic + /facs > ] )
    => avatar motion mesh /deltecho }
```

The output is not a static portrait. It is a skinned humanoid mesh whose vertices move from FACS action units, MetaHuman-style Rig Logic, Live2D/DTECHO parameters, and a Deep Tree Echo residual (Deltecho).

## Stage order

| Stage | Command | Role |
|---|---|---|
| Innermost pair | `/rig-logic + /facs` | Ekman recipes become action units; Rig Logic maps them onto Mixamo-named joints and blendshape channels. |
| Identity wrap | `/meta-echo-dna` | Encodes rest-pose (rig + FACS) plus the eight Echo persona dimensions into a 32-float genotype and FNV-1a fingerprint. |
| 2D drive | `/live2d-dtecho` | Deep Tree Echo attention/energy plus the DNA drive Cubism-style parameters (`ParamMouthForm`, `ParamEyeLOpen`, …). |
| 3D bind | `/meshy3d` | Emits a Meshy-compatible auto-rig task and synthesizes a local humanoid (head + torso) with facial regions. A real Meshy GLB can replace the synthesizer later; bind keeps the same DNA/Live2D contract. |
| Output | `avatar motion mesh` | Linear-blend skinning + regional morph deltas. |
| Post | `/deltecho` | Echo-state reservoir remembers prior affect and re-skins the mesh with a temporal residual so motion carries echo instead of popping. |

## Build and test

```bash
cd engines/avatar
cmake -B build -DAVATAR_BUILD_TESTS=ON -DAVATAR_BUILD_EXAMPLES=ON
cmake --build build
cd build && ctest --output-on-failure
./examples/avatar_pipeline_demo /tmp/avatar.json /tmp/avatar.obj
```

NPC entities evaluate the same pipeline from persona affect when `NPC_USE_AVATAR=ON` (the default):

```cpp
npc.evaluateAvatar(dt);
const auto& frame = npc.lastAvatarFrame();
```

## Inspector

`web/avatar/index.html` visualizes the same composition: FACS bars, Live2D parameters, DNA fingerprint, Deltecho reservoir, and a deforming wireframe mesh.
