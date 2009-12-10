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

#ifndef V3D_GPU_UNDISTORT_H
#define V3D_GPU_UNDISTORT_H

# if defined(V3DLIB_GPGPU_ENABLE_CG)

#include "GL/v3d_gpubase.h"

namespace V3D_GPU
{

   struct Undistortion
   {
         Undistortion()
            : _width(0), _height(0), _destBuffer("rgb=8", "Undistortion::_destBuffer")
         { }

         void setDistortionParameters(double const fx, double const fy,
                                      double const radial[4], double const center[2])
         {
            _fx = fx;
            _fy = fy;
            _k1 = radial[0];
            _k2 = radial[1];
            _p1 = radial[2];
            _p2 = radial[3];
            _center[0] = center[0];
            _center[1] = center[1];
         }

         void setDistortionParameters(double const fx, double const fy, double const k1, double const k2,
                                      double const p1, double const p2, double const u, double const v)
         {
            _fx = fx;
            _fy = fy;
            _k1 = k1;
            _k2 = k2;
            _p1 = p1;
            _p2 = p2;
            _center[0] = u;
            _center[1] = v;
         }

         void allocate(int w, int h);
         void deallocate();

         void undistortColorImage(unsigned char const * image, unsigned char * result);
         void undistortIntensityImage(unsigned char const * image, unsigned char * result);

      protected:
         int _width, _height;
         double _fx, _fy;
         double _k1, _k2, _p1, _p2;
         double _center[2];

         ImageTexture2D _srcTex;
         RTT_Buffer  _destBuffer;
   }; // end struct Undistortion

} // end namespace V3D_GPU

# endif // defined(V3DLIB_GPGPU_ENABLE_CG)

#endif
