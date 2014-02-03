/*****************************************************************************************
 * File: RenderTransform2dOpenGL.cpp
 * Desc: Transformations applied before blitting a 2d object usind OpenGL
 *****************************************************************************************/

/*********************************** The zlib License ************************************
 *
 * Copyright (c) 2013 Indielib-crossplatform Development Team
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 * claim that you wrote the original software. If you use this software
 * in a product, an acknowledgment in the product documentation would be
 * appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not be
 * misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source
 * distribution.
 *
 *****************************************************************************************/
#include "Defines.h"

#ifdef INDIERENDER_GLES_IOS
// ----- Includes -----

#include "Global.h"
#include "Defines.h"
#include "OpenGLES2Render.h"
#include "IND_Camera2d.h"
#include "IND_Window.h"

/** @cond DOCUMENT_PRIVATEAPI */

GLenum blendValueFromIndielibValue(IND_BlendingType pBlendType) {
    
    switch (pBlendType) {
        case IND_ZERO:
            return GL_ZERO;
        case IND_ONE:
            return GL_ONE;
        case IND_SRCCOLOR:
            return GL_SRC_COLOR;
        case IND_INVSRCCOLOR:
            return GL_ONE_MINUS_SRC_COLOR;
        case IND_SRCALPHA:
            return GL_SRC_ALPHA;
        case IND_INVSRCALPHA:
            return GL_ONE_MINUS_SRC_ALPHA;
        case IND_DESTALPHA:
            return GL_DST_ALPHA;
        case IND_INVDESTALPHA:
            return GL_ONE_MINUS_DST_ALPHA;
        case IND_DESTCOLOR:
            return GL_DST_COLOR;
        case IND_INVDESTCOLOR:
            return GL_ONE_MINUS_DST_COLOR;
        case IND_SRCALPHASAT:
            return GL_SRC_ALPHA_SATURATE;

            // Unsupported, DirectX-only
        case IND_BOTHSRCALPHA:
        case IND_BOTHINVSRCALPHA:
        case IND_BLENDFACTOR:
        case IND_INVBLENDFACTOR:
            return GL_ZERO;
        default:
            break;
    }
    return GL_ZERO;
}

// --------------------------------------------------------------------------------
//							         Public methods
// --------------------------------------------------------------------------------

bool OpenGLES2Render::setViewPort2d(int pX,
                                 int pY,
                                 int pWidth,
                                 int pHeight) {
	// ----- If the region is outside the framebuffer return error -----

	if (pX +  pWidth > _info._fbWidth)  return 0;
	if (pX < 0)                         return 0;
	if (pY + pHeight > _info._fbHeight) return 0;
	if (pY < 0)                         return 0;

	// ----- Viewport characteristics -----

	_info._viewPortX      = pX;
	_info._viewPortY      = pY;
	_info._viewPortWidth  = pWidth;
	_info._viewPortHeight = pHeight;
    _info._viewPortApectRatio = static_cast<float>(pWidth/pHeight);

	//Clear projection matrix
	_shaderProjectionMatrix = IND_Matrix::identity();

	//Define the viewport
	glViewport(static_cast<GLint>(pX),
	           static_cast<GLint>(pY),
	           static_cast<GLsizei>(pWidth),
	           static_cast<GLsizei>(pHeight));

	setDefaultGLState();
	return true;
}


void OpenGLES2Render::setCamera2d(IND_Camera2d *pCamera2d) {
	// ----- Lookat matrix -----
	//Rotate that axes in Z by the camera angle
	//Roll is rotation around the z axis (_look)
	IND_Matrix rollmatrix;
	float rotAngle (pCamera2d->_angle - pCamera2d->_prevAngle);
	_math.matrix4DSetRotationAroundAxis(rollmatrix, rotAngle , pCamera2d->_look);
	_math.transformVector3DbyMatrix4D(pCamera2d->_right, rollmatrix);
	_math.transformVector3DbyMatrix4D(pCamera2d->_up, rollmatrix);
	//Reset camera angle difference
	pCamera2d->setAngle(pCamera2d->_angle);
	
	//Build the view matrix from the transformed camera axis
	IND_Matrix lookatmatrix; 
	_math.matrix4DLookAtMatrixLH(pCamera2d->_right,
	                           pCamera2d->_up,
							   pCamera2d->_look,
							   pCamera2d->_pos,
	                           lookatmatrix);

    
    _cameraMatrix = IND_Matrix::identity();
    
    //------ Zooming -----
	if (pCamera2d->_zoom != 1.0f) {
        //Zoom global scale (around where camera points - screen center)
        IND_Matrix zoom = IND_Matrix();
        _math.matrix4DSetScale(zoom, pCamera2d->_zoom, pCamera2d->_zoom,0.f);
        _math.matrix4DMultiplyInPlace(_cameraMatrix, zoom);
	} 

	//------ Lookat transform -----
    _math.matrix4DMultiplyInPlace(_cameraMatrix, lookatmatrix);
    
	//------ Global point to pixel ratio -----
    IND_Matrix pointPixelScale;
    _math.matrix4DSetScale(pointPixelScale,_info._pointPixelScale, _info._pointPixelScale, 1.0f);
    _math.matrix4DMultiplyInPlace(_cameraMatrix, pointPixelScale);
    
	// ----- Projection Matrix -----
	//Setup a 2d projection (orthogonal)
	perspectiveOrtho(static_cast<float>(_info._viewPortWidth), static_cast<float>(_info._viewPortHeight), 2048.0f, -2048.0f);
}


void OpenGLES2Render::setTransform2d(int pX,
                                  int pY,
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
                                  IND_Matrix *pMatrix) {

	//Temporal holders for all accumulated transforms
	IND_Matrix totalTrans;
	_math.matrix4DSetIdentity(totalTrans);

	//Initialize to identity given matrix, if exists
	if (pMatrix) {
		_math.matrix4DSetIdentity(*pMatrix);
	}
    
	// Translations
	if (pX != 0 || pY != 0) {
		IND_Matrix trans;
		_math.matrix4DSetTranslation(trans,static_cast<float>(pX),static_cast<float>(pY),0.0f);
		_math.matrix4DMultiplyInPlace(totalTrans,trans);
	}

	// Scaling
	if (pScaleX != 1.0f || pScaleY != 1.0f) {
		IND_Matrix scale;
		_math.matrix4DSetScale(scale,pScaleX,pScaleY,0.0f);
		_math.matrix4DMultiplyInPlace(totalTrans,scale);
	}

	// Rotations
	if (pAngleX != 0.0f) {
		IND_Matrix angleX;
		_math.matrix4DSetRotationAroundAxis(angleX,pAngleX,IND_Vector3(1.0f,0.0f,0.0f));
		_math.matrix4DMultiplyInPlace(totalTrans,angleX);
	}

	if (pAngleY != 0.0f) {
		IND_Matrix angleY;
		_math.matrix4DSetRotationAroundAxis(angleY,pAngleY,IND_Vector3(0.0f,1.0f,0.0f));
		_math.matrix4DMultiplyInPlace(totalTrans,angleY);
	}

	if (pAngleZ != 0.0f) {
		IND_Matrix angleZ;
		_math.matrix4DSetRotationAroundAxis(angleZ,pAngleZ,IND_Vector3(0.0f,0.0f,1.0f));
		_math.matrix4DMultiplyInPlace(totalTrans,angleZ);
	}

	// Hotspot - Add hotspot to make all transforms to be affected by it
	if (pAxisCalX != 0 || pAxisCalY != 0) {
		IND_Matrix hotspot;
		_math.matrix4DSetTranslation(hotspot,static_cast<float>(pAxisCalX),static_cast<float>(pAxisCalY),0.0f);
		_math.matrix4DMultiplyInPlace(totalTrans,hotspot);
	}

    // Mirroring (180 degrees rotations) and translation
	if (pMirrorX || pMirrorY) {
		//A mirror is a rotation in desired axis (the actual mirror) and a repositioning because rotation
		//also moves 'out of place' the entity translation-wise
		if (pMirrorX) {
			IND_Matrix mirrorX;
            //After rotation around origin, move back texture to correct place
            _math.matrix4DSetTranslation(mirrorX,
                                         static_cast<float>(pWidth),
                                         0.0f,
                                         0.0f);
			_math.matrix4DMultiplyInPlace(totalTrans,mirrorX);
            
            //Rotate in y, to invert texture
            IND_Matrix rotation;
			_math.matrix4DSetRotationAroundAxis(rotation,180.0f,IND_Vector3(0.0f,1.0f,0.0f));
			_math.matrix4DMultiplyInPlace(totalTrans,rotation);
		}
        
		//A mirror is a rotation in desired axis (the actual mirror) and a repositioning because rotation
		//also moves 'out of place' the entity translation-wise
		if (pMirrorY) {
			IND_Matrix mirrorY;
            //After rotation around origin, move back texture to correct place
            _math.matrix4DSetTranslation(mirrorY,
                                         0.0f,
                                         static_cast<float>(pHeight),
                                         0.0f);
			_math.matrix4DMultiplyInPlace(totalTrans,mirrorY);
            
            //Rotate in x, to invert texture
            IND_Matrix rotation;
			_math.matrix4DSetRotationAroundAxis(rotation,180.0f,IND_Vector3(1.0f,0.0f,0.0f));
			_math.matrix4DMultiplyInPlace(totalTrans,rotation);
		}
	}
    
    // Cache the change
    _modelToWorld = totalTrans;
    
    // Affect modelView Matrix
    _math.matrix4DMultiply(_cameraMatrix, _modelToWorld, _shaderModelViewMatrix);
    
	// ----- Return World Matrix (in IndieLib format) ----
	//Transformations have been applied where needed
	if (pMatrix) {
		*pMatrix = totalTrans;
	}
}

void OpenGLES2Render::setTransform2d(IND_Matrix &pMatrix) {
    // Affect modelView Matrix
    _math.matrix4DMultiply(_cameraMatrix, pMatrix, _shaderModelViewMatrix);

    // Cache the change
	_modelToWorld = pMatrix;
}

void OpenGLES2Render::setIdentityTransform2d ()  {
	// ----- Applies the transformation -----
    _shaderModelViewMatrix = _cameraMatrix;

	//Finally cache the change
	_math.matrix4DSetIdentity(_modelToWorld);
}

void OpenGLES2Render::setRainbow2d(IND_Type pType,
                                bool pCull,
                                bool pMirrorX,
                                   bool pMirrorY,
                                   IND_Filter pFilter,
                                   unsigned char pR,
                                   unsigned char pG,
                                   unsigned char pB,
                                   unsigned char pA,
                                   unsigned char pFadeR,
                                   unsigned char pFadeG,
                                   unsigned char pFadeB,
                                   unsigned char pFadeA,
                                   IND_BlendingType pSo,
                                   IND_BlendingType pDs) {
	//Parameters error correction:
	if (pA > 255) {
		pA = 255;
	}
    
	//Setup neutral 'blend' for texture stage
    _renderState._blendR = 0.f;
    _renderState._blendG = 0.f;
    _renderState._blendB = 0.f;
    _renderState._blendA = 1.f;
    
	// ----- Filters -----
    // In GL, texture filtering is applied to the bound texture. From this method we don't know which is the
    // bound texture, so we cache the requested state, so before actually rendering, we could set the state
    // to the bound texture
	int filterType = GL_NEAREST;
	if (IND_FILTER_LINEAR == pFilter) {
		filterType = GL_LINEAR;
	}
    
    _tex2dState._magFilter = filterType;
    _tex2dState._minFilter = filterType;
    
	// ----- Back face culling -----
    _renderState._cullingEnabled = pCull;
    _renderState._frontFaceIsCW = true;
    
	// Mirroring. When mirrored only in one axis, culling face is inverted
    if ((pMirrorX && !pMirrorY) ||
        (!pMirrorX && pMirrorY)) {
        // Back face culling CCW
        _renderState._frontFaceIsCW = false;
    }
    
	// ----- Blending -----
	switch (pType) {
        case IND_OPAQUE: {
            // Tinting
            if (pR != 255 || pG != 255 || pB != 255) {
				_renderState._blendR = static_cast<float>(pR) / 255.0f;
				_renderState._blendG = static_cast<float>(pG) / 255.0f;
				_renderState._blendB = static_cast<float>(pB) / 255.0f;
            }
            
            // Alpha
            if (pA != 255) {
                _renderState._blendA = static_cast<float>(pA) / 255.0f;
            }
            
            // Fade to color
            if (pFadeA != 255) {
                // TODO
//				blendA = static_cast<float>(pFadeA) / 255.0f;
//                blendR = static_cast<float>(pFadeR) / 255.0f;
//                blendG = static_cast<float>(pFadeG) / 255.0f;
//                blendB = static_cast<float>(pFadeB) / 255.0f;
            }
            
            if (pSo && pDs) {
                // Explicit blending values
                _renderState._alphaBlendEnabled = true;
                _renderState._srcBlendFactor = blendValueFromIndielibValue(pSo);
                _renderState._dstBlendFactor = blendValueFromIndielibValue(pDs);
            } else {
                _renderState._alphaBlendEnabled = true;
                _renderState._srcBlendFactor = GL_ONE;
                _renderState._dstBlendFactor = GL_ZERO;
            }
        }
            break;
            
        case IND_ALPHA: {
            // Tinting
            if (pR != 255 || pG != 255 || pB != 255) {
				_renderState._blendR = static_cast<float>(pR) / 255.0f;
				_renderState._blendG = static_cast<float>(pG) / 255.0f;
				_renderState._blendB = static_cast<float>(pB) / 255.0f;
            }
            
            // Alpha
            if (pA != 255) {
				_renderState._blendA = static_cast<float>(pA) / 255.0f;
            }
            
            // Fade to color
            if (pFadeA != 255) {
                //				blendA = static_cast<float>(pFadeA) / 255.0f;
                //                blendR = static_cast<float>(pFadeR) / 255.0f;
                //                blendG = static_cast<float>(pFadeG) / 255.0f;
                //                blendB = static_cast<float>(pFadeB) / 255.0f;
            }
            
            _renderState._alphaBlendEnabled = true;
            if (!pSo || !pDs) {
                //Default alpha blending
                _renderState._srcBlendFactor = GL_SRC_ALPHA;
                _renderState._dstBlendFactor = GL_ONE_MINUS_SRC_ALPHA;
            } else {
                // Explicit blending values
                _renderState._srcBlendFactor = blendValueFromIndielibValue(pSo);
                _renderState._dstBlendFactor = blendValueFromIndielibValue(pDs);
            }
            
        }
            break;
        default: {
        }
	}
    
    
    if (_renderState._cullingEnabled) {
		glEnable(GL_CULL_FACE);
        if (_renderState._frontFaceIsCW) {
            glFrontFace(GL_CW);
        } else {
            glFrontFace(GL_CCW);
        }
    } else {
        glDisable(GL_CULL_FACE);
    }
    
    if (_renderState._alphaBlendEnabled) {
        glEnable(GL_BLEND);
        glBlendFunc(_renderState._srcBlendFactor, _renderState._dstBlendFactor);
    } else {
        glDisable(GL_BLEND);
    }
}

void OpenGLES2Render::setDefaultGLState() {
    // ----- 2d GLState -----
	//Many defaults are GL_FALSE, but for the sake of explicitly safe operations (and code clearness)
	//I include glDisable explicits
	glDisable(GL_DEPTH_TEST); //No depth testing
    
	// ----- Texturing settings  -----
	//Generally we work with byte-aligned textures.
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
}

void OpenGLES2Render::setGLBoundTextureParams() {
    //Texture wrap mode
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,_tex2dState._wrapS);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,_tex2dState._wrapT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, _tex2dState._magFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, _tex2dState._minFilter);
}

/** @endcond */

#endif //INDIERENDER_GLES_IOS
