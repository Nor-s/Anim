# skeleton mapping

-   스켈레톤은 모델마다 다르므로, openpose등을 이용하여 키값을 추출하여 애니메이션을 만들 때, 이를 적용하기 위해서는 스켈레톤이 호환이 되어야한다.

-   하지만 이 작업은 손이 많이가는 작업이므로, 일단 openpose등과 연동할 스켈레톤을 하나 만들어서 테스트해야한다.

## 해야할것

0. openpose와 연동할 스켈레톤 모델을 만든다. (계층구조, 본의 이름 등 설정)
1. openpose 에서 keypoint 값들을 가져온다.
2. keypoint 값들을 사용하여 0번에서 만든 모델의 계층형태를 기반으로 scale, rotation, traslation등을 생성한다.
3. 0번에서 만든 모델에 변환행렬들을 사용하여 애니메이션을 만든다.

## 할 수 있으면 해야할 것

-   리타겟팅: 서로 다른 스켈레톤모델에 애니메이션을 호환시키는 작업

## 질문답변

![](https://learnopengl.com/img/guest/2020/skeletal_animation/assimp1.jpeg)

-   어떻게 매핑?
    -   정점들을 가져와서 계층구조를 만들어야함.
    -   애니메이션들은 positions, rotataion, scaling으로 구성이됨(위 이미지참고)
    -   그러므로 이 정보들을 정점에서 생성해야함.
