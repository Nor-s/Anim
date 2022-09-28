## **devlog**

### **3/25**

-   test model loading, pixelate, imgui

![](/screenshot/Mar-25-2022%2012-58-15.gif?raw=true)

### **4/1**

-   ImGui docking, refactoring, camera, skybox blur

![](</screenshot/Animation%20(32).gif>)

-   3D model to png, pixelate

![](/screenshot/Apr-01-2022%2014-53-09.gif)

### **4/9**

-   outline, outline color, animation

![](/screenshot/Apr-09-2022%2004-14-49.gif)

### **4/17**

-   custom animation

    1. get `aiNode ::mTransformation`
    2. decompose this matrix
    3. update translation, rotation, scale

-   or

    1. get `aiNode::mTransformation`
    2. mTransformation = mTransformation *T*R\*S
       ![](/screenshot/Apr_2022-04-17_12-45-38.png)

### **4/23**

-   gui for hierarchy, gui for transformation

    ![](/screenshot/anim.gif)

### **4/29**

-   animation gui: simple timeline
    ![](/screenshot/Apr-30-2022-00-11-45.gif)
-   retargeting(same bone hierarchy: mixamo): model B _ animation B^-1 _ (animation TRS)
    ![](/screenshot/Apr-30-2022-00-32-28.gif)

### **5/1**

-   animation with json file, text edit

    ![](/screenshot/May-01-2022-10-54-32.gif)

### **5/28**

-   Add mediapipe open button

    ![](/screenshot/animation22.gif)

### **6/02**

-   Add export button: glft2
-   an screenshot in Blender.
    ![](/screenshot/screenshot.gif)

### **08/05 Restart, to make it into an animation tool**

-   simple lighting, add imguizmo

![](/screenshot/2022-0805.gif)

### **08/10**

-   skeleton visualization, imguizmo bar, change grid

![](/screenshot/2022-08-10.gif)

### **8/21**

-   picking object(read pixel), outline (stencil + wireframe)
-   refactor entity, components

![](/screenshot/2022-08-21.gif)

### **8/30**

-   refactor mediapipe to animation
-   add embeded python

![](/screenshot/88910.gif)
