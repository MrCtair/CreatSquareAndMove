//
//  main.cpp
//  CreatSquareAndMove
//
//  Created by China on 2020/7/4.
//  Copyright © 2020 China. All rights reserved.
//
/*
 包含了大部分GLTool中类似C语言的独立函数
 */
#include "GLTools.h"
/*
 移入了GLTool着色管理器(shaderMananger)类。
 没有着色管理器，我们就不能再OpenGL(核心框架)进行着色。
 着色管理器不仅允许我们创建并管理着色器，还提供”存储着色器“。
 */
#include "GLShaderManager.h"
/*
 在Mac 系统下，`#include<glut/glut.h>`
 在Windows 和 Linux上，我们使⽤用freeglut的静
 态库版本并且需要添加⼀一个宏
 */
#ifdef __APPLE__
#include <glut/glut.h>
#else
#define FREEGLUT_STATIC
#include <GL/glut.h>
#endif

//着色管理器
GLBatch triangleBatch;
//简单的批次容器，是GLTool的一个简单容器
GLShaderManager shaderManager;

// 定一个长度
GLfloat blockSize = 0.1f;

// 用这个长度创建一个中心在坐标原点的矩形
GLfloat vVerts[] = {
    -blockSize, -blockSize, 0.0f,
    blockSize, -blockSize, 0.0f,
    blockSize, blockSize, 0.0f,
    -blockSize, blockSize, 0.0
};
// x的移动距离
GLfloat xPos = 0.0f;
// 在y轴上的移动距离
GLfloat yPos = 0.0;

/*
 自定义函数，通过glutReshaperFunc(函数名)注册为重塑函数，当屏幕大小发生变化、第一次创建窗口，会调用改函数来调整窗口和视口的大小。
 */
void ChangeSize(int w,int h){
    glViewport(0, 0, w, h);
}

/*
 自定义函数。通过glutDisplayFunc(函数名)注册为显示渲染函数。当屏幕发生变化/或开发者主动渲染会调用此函数
 */
void RenderScene(void){
    //1.清除一个或者一组特定的缓存区
    /*
     缓冲区是一块存在图像信息的储存空间，红色、绿色、蓝色和alpha分量通常一起分量通常一起作为颜色缓存区或像素缓存区引用。
     OpenGL 中不止一种缓冲区（颜色缓存区、深度缓存区和模板缓存区）
      清除缓存区对数值进行预置
     参数：指定将要清除的缓存的
     GL_COLOR_BUFFER_BIT :指示当前激活的用来进行颜色写入缓冲区
     GL_DEPTH_BUFFER_BIT :指示深度缓存区
     GL_STENCIL_BUFFER_BIT:指示模板缓冲区
     */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
    
    
    //2.设置一组浮点数来表示红色
    GLfloat vRed[] = {1.0,0.0,0.0,1.0f};
    
    // 定义一个矩阵
    M3DMatrix44f mTransfromMatrix;
    
    //平移矩阵
    m3dTranslationMatrix44(mTransfromMatrix, xPos, yPos, 0.0f);
    
    //平面着色器(固定着色器)
    shaderManager.UseStockShader(GLT_SHADER_FLAT, mTransfromMatrix, vRed);
    
    //提交着色器
    triangleBatch.Draw();
    
    //在开始的设置openGL 窗口的时候，我们指定要一个双缓冲区的渲染环境。这就意味着将在后台缓冲区进行渲染，渲染结束后交换给前台。这种方式可以防止观察者看到可能伴随着动画帧与动画帧之间的闪烁的渲染过程。缓冲区交换平台将以平台特定的方式进行。
    //将后台缓冲区进行渲染，然后结束后交换给前台
    glutSwapBuffers();
}

/*
 自定义函数，需要渲染图形的相关配置在这里完成(顶点数据，颜色数据)
 */
void SetupRC(){
    //设置清屏颜色（背景颜色）
    glClearColor(0.9f, 0.9f, 0.9f, 1);
   
    //没有着色器，在OpenGL 核心框架中是无法进行任何渲染的。初始化一个渲染管理器。
    shaderManager.InitializeStockShaders();
   
    //修改为GL_TRIANGLE_FAN ，4个顶点
    triangleBatch.Begin(GL_TRIANGLE_FAN, 4);

    triangleBatch.CopyVertexData3f(vVerts);
    triangleBatch.End();
}


/*
  自定义函数。特殊键位响应 函数.(当⽤用户使⽤用特殊键位则会调⽤用该函数)
 */
void SpecialKeys(int key, int x, int y){
    // 设置每一步移动的距离
    GLfloat stepSize = 0.025f;
    
    
    if (key == GLUT_KEY_UP) {
        
        yPos += stepSize;
    }
    
    if (key == GLUT_KEY_DOWN) {
        
        yPos -= stepSize;
    }
    
    if (key == GLUT_KEY_LEFT) {
        xPos -= stepSize;
    }
    
    if (key == GLUT_KEY_RIGHT) {
        xPos += stepSize;
    }

    //碰撞检测
    if (xPos < (-1.0f + blockSize)) {
        
        xPos = -1.0f + blockSize;
    }
    
    if (xPos > (1.0f - blockSize)) {
        xPos = 1.0f - blockSize;
    }
    
    if (yPos < (-1.0f + blockSize)) {
        yPos = -1.0f + blockSize;
    }
    
    if (yPos > (1.0f - blockSize)) {
        yPos = 1.0f - blockSize;
    }
    
    //手动触发渲染函数
    glutPostRedisplay();
}
/*
 主函数：程序入口。
 OpenGL是面向过程编程
 */
int main(int argc,char* argv[]){
    //设置当前工作目录，针对MAC OS X
    gltSetWorkingDirectory(argv[0]);
    //初始化GLUT库
    glutInit(&argc, argv);
    /*初始化双缓冲窗口，其中标志GLUT_DOUBLE、GLUT_RGBA、GLUT_DEPTH、GLUT_STENCIL分别指
     双缓冲窗口、RGBA颜色模式、深度测试、模板缓冲区*/
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA|GLUT_DEPTH|GLUT_STENCIL);
    
    //GLUT窗口大小，标题窗口
    glutInitWindowSize(600,600);
    glutCreateWindow("Triangle");
    
    //注册回调函数
    glutReshapeFunc(ChangeSize);
    glutDisplayFunc(RenderScene);
    glutSpecialFunc(SpecialKeys);
    
    //驱动程序的初始化中没有出现任何问题。
    GLenum err = glewInit();
    if(GLEW_OK != err) {
        fprintf(stderr,"glew error:%s\n",glewGetErrorString(err));
        return 1;
    }
    //调用SetupRC，设置数据
    SetupRC();
    //类似iOS runloop
    glutMainLoop();
    return 0;
    
}


