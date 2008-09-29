// -*- C++ -*-
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

#ifndef V3D_GPU_PYRAMID_H
#define V3D_GPU_PYRAMID_H

# if defined(V3DLIB_GPGPU_ENABLE_CG)

#include "v3d_gpubase.h"

namespace V3D_GPU
{

   struct PyramidCreator
   {
         PyramidCreator()
            : _width(0), _height(0), _nLevels(0)
         { }

         ~PyramidCreator() { }

         int numberOfLevels() const { return _nLevels; }

         void allocate(int w, int h, int nLevels);
         void deallocate();

         void buildPyramidForGrayscaleImage(uchar const * image);

         void activateTarget(int level);

         unsigned int textureID() const { return _pyrTexID; }
         unsigned int sourceTextureID() const { return _srcTexID; }

         protected:
         int _width, _height, _nLevels;
         unsigned int _pyrFbIDs[8], _tmpFbIDs[8], _tmp2FbID;
         unsigned int _srcTexID, _pyrTexID, _tmpTexID, _tmpTex2ID;
   }; // end struct PyramidCreator

   struct PyramidWithDerivativesCreator
   {
         PyramidWithDerivativesCreator()
            : _width(0), _height(0), _nLevels(0)
         { }

         ~PyramidWithDerivativesCreator() { }

         int numberOfLevels() const { return _nLevels; }

         void allocate(int w, int h, int nLevels);
         void deallocate();

         void buildPyramidForGrayscaleImage(uchar const * image);

         void activateTarget(int level);

         unsigned int textureID() const { return _pyrTexID; }
         unsigned int sourceTextureID() const { return _srcTexID; }

         protected:
         int _width, _height, _nLevels;
         unsigned int _pyrFbIDs[8], _tmpFbIDs[8], _tmp2FbID;
         unsigned int _srcTexID, _pyrTexID, _tmpTexID, _tmpTex2ID;
   }; // end struct PyramidWithDerivativesCreator

} // end namespace V3D_GPU

# endif // defined(V3DLIB_GPGPU_ENABLE_CG)

#endif
