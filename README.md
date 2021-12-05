# SoftwarePixelRenderer
소프트웨어 3D 렌더링 프레임워크 Turotial
Direct2D 환경에서 3D 모델을 소프트웨어 렌더링 하기 위한 Tutorial 입니다.

Tutorial을 컴파일하고 테스트하기 위한 시스템 최소사양은 아래와 같습니다.
※하드웨어 가속을 지원하지만 내장 GPU로도 충분히 구동됩니다.

------------------------------------- 아 래 ----------------------------------------

    ○ OS                : Windows 7 이상
    ○ Compiler & IDE    : VisualStudio 2015 이상 
    ○ CPU               : 논리 2 core 이상
    ○ RAM               : 256MB 이상
    ○ VGA               : Intel iGPU 또는 Amd VegaX 또는 외장 VGA




Tutorial 목차는 다음과 같습니다.

------------------------------------- 목 차 ----------------------------------------

    1. multithread render             : Direct2D를 이용한 렌더링 프레임 워크.
    2. transform and mesh render      : DirectXMath와 SIMD 가속을 이용한 3D 렌더링 pipeline 환경
    3. mesh load and optimized        : Wavefront .OBJ 파일 Parser 및 렌더링 환경
    4. camera                         : Free, FPS, TPS 카메라 구현
    5. frustum culling                : Rendering pipeline 상의 object culling, backface culling  구현
    6. clipping                       : Rendering pipeline 상의 face clipping 구현
    7. vertex color                   : 정점 색상을 이용한 fill triangle 구현
    8. z buffer                       : Z buffer test 환경 구현
    9. texturing                      : Texture mapping 구현
    10. texturing solved distortion   : Texture mapping 외곡
    11. lighting                      : 빠른 조명 구현
    12. lighting smooth               : 정확한 조명 구현
    13. lighting stencil and shadow   : Stencil 버퍼와 그림자 매핑
    
