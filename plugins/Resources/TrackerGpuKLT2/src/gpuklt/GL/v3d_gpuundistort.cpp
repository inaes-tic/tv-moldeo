/*
Copyright (c) 2008 University of North Carolina at Chapel Hill

This file is part of GPU-KLT.

GPU-KLT is free software: you can redistribute it and/or modify it under the
terms of the GNU Lesser General Public License as published by the Free
Software Foundation, either version 3 of the License, or (at your option) any
later version.

GPU-KLT is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
details.

You should have received a copy of the GNU Lesser General Public License along
with GPU-KLT. If not, see <http://www.gnu.org/licenses/>.
*/

#include "v3d_gpuundistort.h"

#include <GL/glew.h>

#if defined(V3DLIB_GPGPU_ENABLE_CG)

using namespace V3D_GPU;

namespace
{

   Cg_FragmentProgram * shader = 0;

}

namespace V3D_GPU
{

   void
   Undistortion::allocate(int w, int h)
   {
      _width = w;
      _height = h;

      _srcTex.allocateID();
      _srcTex.reserve(w, h, TextureSpecification("rgb=8"));
      _srcTex.bind(GL_TEXTURE0);
      glTexParameteri(_srcTex.textureTarget(), GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(_srcTex.textureTarget(), GL_TEXTURE_MAG_FILTER, GL_LINEAR);

      _destBuffer.allocate(w, h);

      if (shader == 0)
      {
         shader = new Cg_FragmentProgram("Undistortion::shader");
         shader->setProgramFromFile("undistort_parametric.cg");
#if 0
         char const * cgArgs[] = { "-bestprecision", "-nofastmath", "-nofastprecision", 0 };
         shader->compile(cgArgs);
#else
         shader->compile();
#endif
         checkGLErrorsHere0();
      }
   }

   void
   Undistortion::deallocate()
   {
      _srcTex.deallocateID();
      _destBuffer.deallocate();
   }

   void
   Undistortion::undistortColorImage(unsigned char const * image, unsigned char * result)
   {
      _srcTex.overwriteWith(image, 3);

      _destBuffer.activate();
      setupNormalizedProjection();

      shader->parameter("f", _fx, _fy, 1.0f/_fx, 1.0f/_fy);
      shader->parameter("k", _k1, _k2);
      shader->parameter("p", _p1, _p2);
      shader->parameter("center", _center[0], _center[1]);
      shader->parameter("wh", _width, _height, 1.0f/_width, 1.0f/_height);

      _srcTex.enable(GL_TEXTURE0);

      shader->enable();
#if 0
      renderNormalizedQuad();
#else
      glBegin(GL_TRIANGLES);
      glMultiTexCoord2f(GL_TEXTURE0_ARB, 0, 0);
      glVertex2f(0, 0);
      glMultiTexCoord2f(GL_TEXTURE0_ARB, 2*_width, 0);
      glVertex2f(2, 0);
      glMultiTexCoord2f(GL_TEXTURE0_ARB, 0, 2*_height);
      glVertex2f(0, 2);
      glEnd();
#endif
      shader->disable();

      _srcTex.disable(GL_TEXTURE0);

      glReadPixels(0, 0, _width, _height, GL_RGB, GL_UNSIGNED_BYTE, result);
   }

   void
   Undistortion::undistortIntensityImage(unsigned char const * image, unsigned char * result)
   {
      _srcTex.overwriteWith(image, 1);

      _destBuffer.activate();
      setupNormalizedProjection();

      shader->parameter("f", _fx, _fy, 1.0f/_fx, 1.0f/_fy);
      shader->parameter("k", _k1, _k2);
      shader->parameter("p", _p1, _p2);
      shader->parameter("center", _center[0], _center[1]);
      shader->parameter("wh", _width, _height, 1.0f/_width, 1.0f/_height);

      _srcTex.enable(GL_TEXTURE0);

      shader->enable();
      renderNormalizedQuad();
      shader->disable();

      _srcTex.disable(GL_TEXTURE0);

      glReadPixels(0, 0, _width, _height, GL_RED, GL_UNSIGNED_BYTE, result);
   }

} // end namespace V3D_GPU

#endif // defined(V3DLIB_GPGPU_ENABLE_CG)
