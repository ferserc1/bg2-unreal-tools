
# Bg2 - Unreal tools

Integration utilities between bg2 engine and Unreal. Allows to load bg2 models and scenes, created with Composer, in games and applications created with Unreal Engine 4.

## Usage

Checkout this repository (or add a reference to the repository) inside the `Plugins` folder of your Unreal Engine project and regenerate your project files (Visual Studio or Xcode).

## Example of json material data struct

```json
[
    {
        "name": "inside",
        "class": "PBRMaterial",
        "metallicChannel": 0,
        "roughnessChannel": 0,
        "lightEmissionChannel": 0,
        "heightChannel": 0,
        "alphaCutoff": 0.5,
        "isTransparent": false,
        "diffuseScale": [1,1],
        "metallicScale": [1,1],
        "roughnessScale": [1,1],
        "lightEmissionScale": [1,1],
        "hightScale": [1,1],
        "normalScale": [1,1],
        "diffuseUV": 0,
        "metallicUV": 0,
        "roughnessUV": 0,
        "ambientOcclussionUV": 1,
        "lightEmissionUV": 0,
        "heightUV": 0,
        "normalUV": 0,
        "castShadows": true,
        "heightScale": [1,1],
        "cullFace": true,
        "unlit": false,
        "vislble": true,
        "visibleToShadows": true,
        "groupName": "",
        "diffuse": [0.19,0.45,1,1],
        "metallic": 0.5,
        "roughness": 0.03,
        "fresnel": [1,1,1,1],
        "lightEmission": 0,
        "height": 0,
        "normal": [0.5,0.5,1,1],
        "ambientOcclussion": 1
    },
    {
        "name": "outside",
        "class": "PBRMaterial",
        "metallicChannel": 0,
        "roughnessChannel": 0,
        "lightEmissionChannel": 0,
        "heightChannel": 0,
        "alphaCutoff": 0.5,
        "isTransparent": false,
        "diffuseScale": [2,2],
        "metallicScale": [4,4],
        "roughnessScale": [8,8],
        "fresnelScale": [1,1],
        "lightEmissionScale": [1,1],
        "hightScale": [1,1],
        "normalScale": [2,2],
        "diffuseUV": 1,
        "metallicUV": 1,
        "roughnessUV": 1,
        "fresnelUV": 0,
        "ambientOcclussionUV": 0,
        "lightEmissionUV": 0,
        "heightUV": 1,
        "normalUV": 1,
        "castShadows": true,
        "heightScale": [1,1],
        "cullFace": true,
        "unlit": false,
        "vislble": true,
        "visibleToShadows": true,
        "groupName": "",
        "diffuse": "bricks2_diffuse.jpg",
        "metallic": "bricks_disp.jpg",
        "roughness": "bricks_disp.jpg",
        "fresnel": [1,1,1,1],
        "lightEmission": 0,
        "height": "bricks2_disp.jpg",
        "normal": "bricks2_normal.jpg",
        "ambientOcclussion": 1
    }
]
```