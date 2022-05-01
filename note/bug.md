## bug

- gui에서 scene의 객체들에 대한 포인터를 조작할 때 문제가 생길 가능성이 있음
- imgui neo sequencer에서 current frame이 렌더링되지 않으면 current frame line 정지됨

## bug fix

-   몇몇 mixamo 모델에서 애니메이션 오류
    -   이유?
        -   바인딩포즈는 로딩되므로, 애니메이션 변환 행렬 문제일 가능성이 있음.
    -   해결
        -   $AssimpFbx$\_PreRotation, $AssimpFbx$\_PreTranslation
        -   https://github.com/assimp/assimp/issues/1974
        -   assimp에서 모델을 임포트할 때 생기는 문제 아래의 코드를 삽입하면 해결
        -   `import.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false);`
