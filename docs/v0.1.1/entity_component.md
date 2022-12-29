# v0.1.1 :  Import, Entity, Component

## Import - Model

```mermaid
classDiagram
    
    Import ..> Model : Create
    Import ..> Animation : Create

    Model --> ModelNode: -model_node
    ModelNode --> ModelNode : child << vector >>

    ModelNode --> Mesh  : +mesh

    class Import{
        +import_model(path)
        +import_animation(path)
    }
    class Model {
        -model_node
    }
    class ModelNode{
        +child
        +mesh
        +mat4 initial_transform
    }
```

- Import 클래스에서는 `Model`과 `Animation` 을 생성한다.
  - `Model`: `Mesh` 와 같은 정보를 포함하고 모델의 계층 구조를 나타내는 `ModelNode` 에 대한 정보
  - `Animation`: `바인딩 포즈 정보` 와 `바인딩 포즈에 상대적인 변환`, `프레임` 등 에 대한 정보  

## SharedResource - Import

```mermaid
classDiagram
    SharedResource ..> Import : use
    Import ..> Entity : create
    SharedResource --> Entity  : -entities

    class Import
    class SharedResource {
        -vector< Entity > entities
        +import()
    }
```

- `SharedResource` 는 `Import` 인스턴스를 사용하여 모델, 애니메이션 리소스를 `Entity` 클래스로 관리한다.

```mermaid
classDiagram
    class MeshComponent {
        -Mesh* mesh
    }
    class AnimationComponent {
        -Animation* animation
        -Skeleton skeleton
    }
    class Skeleton {
        -bindpose
    }
    class SharedResource{
        +add_entity()
        +convert_to_entity()
    }

    SharedResource ..> ComponentManager : use

    ComponentManager ..> MeshComponent : create
    ComponentManager ..> PoseComponent : create
    ComponentManager ..> AnimationComponent : create

    MeshComponent --> Mesh
    AnimationComponent --> Animation
    AnimationComponent --> Skeleton
    Skeleton ..> ModelNode : use
```

## Entity

```mermaid
classDiagram

    Entity --|> Entity : -children << vector >>

    class Entity {
        ...
        -vector< Entity > children
        -vector< Component* > components
    }
```

## Entity and Component

```mermaid
classDiagram

    class Entity 
    class PoseComponent
    class TransformComponent
    class MeshComponent

    Entity --> "1" TransformComponent
    Entity --> "0..1" PoseComponent
    Entity --> "0..1" MeshComponent

    class Entity {
        ...
        -vector< Component* > components
    }
```

## Components

```mermaid
classDiagram
    class Component
    <<interface>>  Component

    class Component {
    }

    class PoseComponent
    class TransformComponent
    class MeshComponent
    class ShaderComponent
    class AnimationComponent
    class ArmatureComponent

    Component <|.. PoseComponent
    Component <|.. TransformComponent
    Component <|.. MeshComponent
    Component <|.. ShaderComponent
    Component <|.. AnimationComponent
    Component <|.. ArmatureComponent
```

### PoseComponent

```mermaid
classDiagram
    class PoseComponent

    class AnimationComponent
    class ArmatureComponent
    class Animator
    class BoneInfo

    PoseComponent --> AnimationComponent
    PoseComponent --> ArmatureComponent
    PoseComponent --> Animator
    PoseComponent --> BoneInfo

    class PoseComponent{
        -AnimationComponent animation
        -ArmatureComponent armature
        -BoneInfo bone_info
        -Animator animator
    }
```
