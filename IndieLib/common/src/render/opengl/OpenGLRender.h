/****************************************************************************************
 * File: OpenGLRender.h
 * Desc: Render class using OpenGL
 *
 * Divided in:
 * - OpenGLRender.cpp
 * - RenderTransform2dOpenGL.cpp
 * - RenderTransform3dOpenGL.cpp
 * - RenderTransformCommonOpenGL.cpp
 * - RenderObject2dOpenGL.cpp
 * - RenderObject3dOpenGL.cpp
 * - RenderPrimitive2dOpenGL.cpp
 * - RenderText2dOpenGL.cpp
 * - RenderCollision2dOpenGL.cpp
 ****************************************************************************************/

/*
IndieLib 2d library Copyright (C) 2005 Javier L�pez L�pez (info@pixelartgames.com)
THIS FILE IS AN ADDITIONAL FILE ADDED BY Miguel Angel Qui�ones (2011) (mail:m.quinones.garcia@gmail.com / mikeskywalker007@gmail.com), BUT HAS THE
SAME LICENSE AS THE WHOLE LIBRARY TO RESPECT ORIGINAL AUTHOR OF LIBRARY

This library is free software; you can redistribute it and/or modify it under the
terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
this library; if not, write to the Free Software Foundation, Inc., 59 Temple Place,
Suite 330, Boston, MA 02111-1307 USA
*/


#ifndef _OPENGLRENDER_H_
#define _OPENGLRENDER_H_

// ----- Includes -----

#include <string.h>
#include "Defines.h"
#include "IND_Math.h"
#include "IND_Render.h"
#include "IND_Vector2.h"

// ----- Forward Declarations -----
class IND_Window;
class IND_FontManager;
class IND_SurfaceManager;
class IND_Surface;
class IND_3dMeshManager;
class IND_AnimationManager;
class IND_FontManager;
class IND_Timer;
class IND_Font;
class IND_3dMesh;
class IND_Animation;
class IND_Camera2d;
class IND_Camera3d;
class OSOpenGLManager;

// ----- Libs -----

//#include "dependencies/SDL 2.0/include/SDL.h"
//#include "dependencies/SDL 2.0/include/SDL_opengl.h"
#include "dependencies/glew-1.9.0/include/GL/glew.h" //Extension loading facilites library

// ----- Defines ------
#define MAX_PIXELS 2048

// --------------------------------------------------------------------------------
//									  OpenGLRender
// --------------------------------------------------------------------------------

class OpenGLRender {
public:

	// ----- Init/End -----

	OpenGLRender():
    	_window(NULL),
    	_osOpenGLMgr(NULL),
		_ok(false),
    	_numrenderedObjects(0),
    	_numDiscardedObjects(0),
		_doubleBuffer(false)
	{ }
	~OpenGLRender()              {
		end();
	}


	IND_Window* initRenderAndWindow(IND_WindowProperties& props);
	bool    reset(IND_WindowProperties& props);
	bool    toggleFullScreen();
	void    end();
	bool    isOK() {
		return _ok;
	}

	// ----- Public methods -----

	void beginScene();
	void endScene();
	void showFpsInWindowTitle(char *pFPSString);

	// ----- Viewports and cameras -----


	void clearViewPort(BYTE pR,
	                   BYTE pG,
	                   BYTE pB);

	bool setViewPort2d(int pX,
	                   int pY,
	                   int pWidth,
	                   int pHeight);

	void setCamera2d(IND_Camera2d *pCamera2d);

	bool setViewPort3d(int pX,
	                   int pY,
	                   int pWidth,
	                   int pHeight);

	void setCamera3d(IND_Camera3d *pCamera3d);


	// ----- Render Primitive 2d -----

	bool setAntialiasing(bool pSwitch);

	void blitPixel(int pX,
	               int pY,
	               BYTE pR,
	               BYTE pG,
	               BYTE pB,
	               BYTE pA);

	void blitLine(int pX1,
	              int pY1,
	              int pX2,
	              int pY2,
	              BYTE pR,
	              BYTE pG,
	              BYTE pB,
	              BYTE pA);

	void blitRectangle(int pX1,
	                   int pY1,
	                   int pX2,
	                   int pY2,
	                   BYTE pR,
	                   BYTE pG,
	                   BYTE pB,
	                   BYTE pA);

	void blitFillRectangle(int pX1,
	                       int pY1,
	                       int pX2,
	                       int pY2,
	                       BYTE pR,
	                       BYTE pG,
	                       BYTE pB,
	                       BYTE pA);

	void blitColoredTriangle(int pX1,
	                         int pY1,
	                         int pX2,
	                         int pY2,
	                         int pX3,
	                         int pY3,
	                         BYTE pR1, BYTE pG1, BYTE pB1,
	                         BYTE pR2, BYTE pG2, BYTE pB2,
	                         BYTE pR3, BYTE pG3, BYTE pB3,
	                         BYTE pA);

	void blitTriangleList(IND_Point *pTrianglePoints,
	                      int pNumPoints,
	                      BYTE pR,
	                      BYTE pG,
	                      BYTE pB,
	                      BYTE pA);

	bool blitPoly2d(IND_Point *pPixel,
	                int pNumLines,
	                BYTE pR,
	                BYTE pG,
	                BYTE pB,
	                BYTE pA);

	bool blitRegularPoly(int pX,
	                     int pY,
	                     int pRadius,
	                     int pN,
	                     float pAngle,
	                     BYTE pR,
	                     BYTE pG,
	                     BYTE pB,
	                     BYTE pA);

	void setTransform2d(int pX,
	                    int  pY,
	                    float pAngleX,
	                    float pAngleY,
	                    float pAngleZ,
	                    float pScaleX,
	                    float pScaleY,
	                    int pAxisCalX,
	                    int pAxisCalY,
	                    bool pMirrorX,
	                    bool pMirrorY,
	                    int pWidth,
	                    int pHeight,
	                    IND_Matrix *pMatrix);

	void setTransform2d(IND_Matrix &pTransformMatrix);

	void setIdentityTransform2d ();

	void setTransform3d(float pX,
	                    float  pY,
	                    float  pZ,
	                    float pAngleX,
	                    float pAngleY,
	                    float pAngleZ,
	                    float pScaleX,
	                    float pScaleY,
	                    float pScaleZ,
	                    IND_Matrix *pMatrix);

	void setRainbow2d(IND_Type pType,
	                  bool pCull,
	                  bool pMirrorX,
	                  bool pMirrorY,
	                  IND_Filter pFilter,
	                  BYTE pR,
	                  BYTE pG,
	                  BYTE pB,
	                  BYTE pA,
	                  BYTE pFadeR,
	                  BYTE pFadeG,
	                  BYTE pFadeB,
	                  BYTE pFadeA,
	                  IND_BlendingType pSo,
	                  IND_BlendingType pDs);

	void setRainbow3d(bool pCull,
	                  bool pFlipNormals,
	                  IND_Filter pFilter,
	                  BYTE pR,
	                  BYTE pG,
	                  BYTE pB,
	                  BYTE pA,
	                  BYTE pFadeR,
	                  BYTE pFadeG,
	                  BYTE pFadeB,
	                  BYTE pFadeA,
	                  IND_BlendingType pSo,
	                  IND_BlendingType pDs);

	void lookAt(float pEyeX, float pEyeY, float pEyeZ,
	            float pLookAtX, float pLookAtY, float pLookAtZ,
	            float pUpX, float pUpY, float pUpZ);

	void perspectiveFov(float pFov, float pAspect, float pNearClippingPlane, float pFarClippingPlane);

	void perspectiveOrtho(float pWidth, float pHeight, float pNearClippingPlane, float pFarClippingPlane);

	// ----- Render Object 2d -----

	void blitSurface(IND_Surface *pSu);

	void blitGrid(IND_Surface *pSu, BYTE pR, BYTE pG, BYTE pB, BYTE pA);

	void blitRegionSurface(IND_Surface *pSu,
	                       int pX,
	                       int pY,
	                       int pWidth,
	                       int pHeight);

	bool blitWrapSurface(IND_Surface *pSu,
	                     int pWidth,
	                     int pHeight,
	                     float pUDisplace,
	                     float pVDisplace);

	int blitAnimation(IND_Animation *pAn,
	                  int pSequence,
	                  int pX, int pY,
	                  int pWidth, int pHeight,
	                  bool pToggleWrap,
	                  float pUDisplace,
	                  float pVDisplace);

	// ------ Render Text 2d -----
	void blitText(IND_Font *pFo,
	              char *pText,
	              int pX,
	              int pY,
	              int pOffset,
	              int pLineSpacing,
	              float pScaleX,
	              float pScaleY,
	              BYTE pR,
	              BYTE pG,
	              BYTE pB,
	              BYTE pA,
	              BYTE pFadeR,
	              BYTE pFadeG,
	              BYTE pFadeB,
	              BYTE pFadeA,
	              IND_Filter pLinearFilter,
	              IND_BlendingType pSo,
	              IND_BlendingType pDs,
	              IND_Align pAlign);


	void blit3dMesh(IND_3dMesh *p3dMesh);
	void set3dMeshSequence(IND_3dMesh *p3dMesh, unsigned int pIndex);


	// ----- Rendering steps -----
	void calculeFrustumPlanes();
	
	// ----- Atributtes -----

	//This function returns the x position of the actual viewport
	int getViewPortX()      {
		return _info._viewPortX;
	}
	//This function returns the y position of the actual viewport
	int getViewPortY()      {
		return _info._viewPortY;
	}
	//This function returns the width position of the actual viewport
	int getViewPortWidth()      {
		return _info._viewPortWidth;
	}
	// This function returns the width position of the actual viewport
	int getViewPortHeight()      {
		return _info._viewPortHeight;
	}
	//This function returns the actual version of Direct3d that is using IndieLib.
	char *getVersion()      {
		return _info._version;
	}
	//This function returns the name of the graphic card vendor.
	char *getVendor()      {
		return _info._vendor;
	}
	//This function returns the name of the graphic card.
	char *getRenderer()      {
		return _info._renderer;
	}
	// This function returns the maximum texture size allowed by the graphic card.
	int  getMaxTextureSize()      {
		return _info._maxTextureSize;
	}

	//This function returns a pointer to the IND_Window object where the render has been created
	IND_Window *getWindow()      {
		return _window;
	}

	void getNumrenderedObjectsString(char* pBuffer);

	void getNumDiscardedObjectsString(char* pBuffer);

	int getNumrenderedObjectsInt()      {
		return _numrenderedObjects;
	};
	int getNumDiscardedObjectsInt()      {
		return _numDiscardedObjects;
	};

	void resetNumrenderedObject()      {
		_numrenderedObjects = 0;
	}
	void resetNumDiscardedObjects()      {
		_numDiscardedObjects = 0;
	}

private:

	// ----- Private methods -----
	void initVars();
	void freeVars();

	void getInfo();
	void writeInfo();

	bool initializeOpenGLRender();   //Render init
	bool checkGLExtensions();        //GL extensions managing

	//Blitting helpers
	void fillPixel(PIXEL *pPixel, float pX, float pY,  float pR, float pG, float pB, float pA);
	void fillVertex2d(CUSTOMVERTEX2D *pVertex2d, float pX, float pY, float pU, float pV);
	void setForPrimitive(BYTE pA, bool pResetTransform);

	void BlitGridQuad    (int pAx, int pAy,
                          int pBx, int pBy,
                          int pCx, int pCy,
                          int pDx, int pDy,
						  BYTE pR, BYTE pG, BYTE pB, BYTE pA);

	void BlitGridLine (int pPosX1, int pPosY1, int pPosX2, int pPosY2,  BYTE pR, BYTE pG, BYTE pB, BYTE pA);

	//Culling helpers
	unsigned short CullFrustumBox(const IND_Vector3 &pAABBMin, const IND_Vector3 &pAABBMax);
	void CalculateBoundingRectangle(IND_Vector3 *mP1, IND_Vector3 *mP2, IND_Vector3 *mP3, IND_Vector3 *mP4);
	void MinAndMax4(int p1,
	                int p2,
	                int p3,
	                int p4,
	                int *pMax,
	                int *pMin);

	//Text rendering helpers
	int getLongInPixels(IND_Font *pFo, char *pText, int pPos, int pOffset);

	//Setup helper
	bool resetViewport();

		// ----- Collisions -----
	void blitCollisionCircle(int pPosX, int pPosY, int pRadius, float pScale, BYTE pR, BYTE pG, BYTE pB, BYTE pA, IND_Matrix pWorldMatrix);
	void blitCollisionLine(int pPosX1, int pPosY1, int pPosX2, int pPosY2,  BYTE pR, BYTE pG, BYTE pB, BYTE pA, IND_Matrix pIndWorldMatrix);

	// ----- Objects -----
	IND_Math _math;
	IND_Window *_window;
	OSOpenGLManager *_osOpenGLMgr;

	// ----- Vars -----
	bool _ok;

	int _numrenderedObjects;
	int _numDiscardedObjects;

	bool _doubleBuffer;

	struct infoStruct {
		infoStruct():
			_fbWidth(0),
			_fbHeight(0),
			_viewPortX(0),
			_viewPortY(0),
			_viewPortWidth(0),
			_viewPortHeight(0),
			_antialiasing(0),
			_maxTextureSize(0) {
			strcpy(_version, "NO DATA");
			strcpy(_vendor, "NO DATA");
			strcpy(_renderer, "NO DATA");
		}
		int _fbWidth;
		int _fbHeight;
		int _viewPortX;
		int _viewPortY;
		int _viewPortWidth;
		int _viewPortHeight;
		bool _antialiasing;
		char _version [1024] ;
		char _vendor [1024];
		char _renderer [1024];
		int _maxTextureSize;
		int _textureUnits;
	};
	struct infoStruct _info;

    //Current 'camera' matrix
    IND_Matrix _cameraMatrix;
    
	// ----- Primitives vertices -----

	// Temporal buffer of pixels for drawing primitives
	PIXEL _pixels [MAX_PIXELS];

	// ----- Vertex array -----

	// Temporal buffer of vertices for drawing regions of an IND_Surface
	CUSTOMVERTEX2D _vertices2d [MAX_PIXELS];

	// ----- Primitives vertices -----

	// ----- Vertex array -----

	// ----- Info -----

	// ----- Friends ------
	friend class OpenGLTextureBuilder;
	friend class IND_Render;
};

#endif // _OPENGLRENDER_H_
