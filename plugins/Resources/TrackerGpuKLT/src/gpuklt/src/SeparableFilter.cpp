// --------------------------------------------------------------------------
/*! \file SeparableFilter.cpp
* \brief Separable Class File. 
*
* GpuUrban Library Code:
*/

//----------------------------------------------------------------------
//
// Date: Feb 01, 2006. UNC. (c) Sudipta N. Sinha @ Urbanscape
//
// Work: GpuUrban Library Src Code
//
// Copyright Notice:
//
//   This code and attached libraries cannot be modified, reproduced or copied in any 
//   form without the expressed consent of Dept. of Computer Science, UNC Chapel Hill.
// 
//   Copyright Dept. of Computer Science, UNC Chapel Hill.
//   All rights reserved.
//
//----------------------------------------------------------------------

#define _BUILDING_GPU_VIS

#include <stdio.h>
#include <windows.h>

#include "SeparableFilter.h"
#include "utils.h"

/*!
\fn int getLevel(float Sig, float Extent)
\brief Returns the Size of Kernel for a particular sigma \a Sig
*/
int getLevel(float Sig, float Extent)
{
	int n  = (int) rint(Sig*Extent);
	if (!(n%2)) 
		n = n+1;
	return n;
}



/*!
 \fn SeparableFilter::SeparableFilter();
 \brief Constructor
 */
SeparableFilter::SeparableFilter(GLenum target)
{
	row_kernel = NULL;
	col_kernel = NULL;
	swapNeeded = false;
	texTarget  = target;
}

/*!
 \fn SeparableFilter::SeparableFilter(int Arch, GLenum Target, float Sig, CGcontext& context, CGprofile& vProfile , CGprofile& fProfile)
 \brief Constructor
 */
SeparableFilter::SeparableFilter(int Arch, GLenum Target, float Sig, CGcontext& context, CGprofile& vProfile , CGprofile& fProfile)
{
	arch		 = Arch;
	texTarget    = Target;
	sigma        = Sig;
	row_kernel   = NULL;
	row_deriv    = NULL;
	col_kernel   = NULL;
	col_deriv    = NULL;
	swapNeeded   = false;
	printf("\tGpuVis:SeparableFilter::Constructor called\n");
	computeKernelLevels();

	cgContext       = context;			   //!< Recovered CG Context
    vertexProfile   = vProfile;		   //!< Recovered Vertex Shader Profile
    fragmentProfile = fProfile;		   //!< Recovered Fragment Shader Profile
}



/*!
\fn void SeparableFilter::computeKernelLevels()
\brief Compute the Kernel Levels.
*/
void SeparableFilter::computeKernelLevels()
{
		num_kernel_levels = 1;
		int nkernel       = getLevel(sigma);
		kernel_levels[0] = nkernel;
	    
		printf("kernel level [0] = %d\n",kernel_levels[0]);

		// Output : 
		printf("\n\tGpuVis:SeparableFilter::computeKernelLevels:: Kernel footprints\n");
		for (int j=0;j<num_kernel_levels;j++)
		{
			int n = kernel_levels[j];
			printf("\t%5d - %2d x %2d\n",n,2*n+1,2*n+1);
		}
		printf("\n");
}




/*!
 \fn void SeparableFilter::setRowKernel(int r, float *rowKernelBuffer, float *derivBuffer)
 \brief set current 1D row filter kernel to \a r entries from \a rowKernelBuffer.
 \param r               : size of kernel
 \param rowKernelBuffer : kernel values
 \param derivBuffer     : array for 1d gaussian derivative.
 */
void SeparableFilter::setRowKernel(int r, float *rowKernelBuffer, float *derivBuffer)
{
	setKernelWidth(r);
	if (row_kernel!=NULL)
		delete [] row_kernel;
	if (row_deriv !=NULL)
		delete [] row_deriv;

	row_kernel = new float [r];
	row_deriv  = new float [r];
    for (int i=0;i<r;i++)
	{
		row_kernel[i]=rowKernelBuffer[i];
		row_deriv[i] =derivBuffer[i];
	}
}


/*!
 \fn void SeparableFilter::setColKernel(int c, float *colKernelBuffer, float *derivBuffer)
 \brief set current 1D column filter kernel to \a r entries from \a colKernelBuffer.
 \param r               : size of kernel
 \param colKernelBuffer : kernel values
 \param derivBuffer     : array for 1d gaussian derivative.
 */
void SeparableFilter::setColKernel(int c, float *colKernelBuffer, float *derivBuffer)
{
	setKernelHeight(c);
	if (col_kernel!=NULL)
		delete [] col_kernel;
	if (col_deriv !=NULL)
		delete [] col_deriv;

	col_kernel = new float [c];
	col_deriv   = new float [c];
	for (int i=0;i<c;i++)
	{
		col_kernel[i]=colKernelBuffer[i];
		col_deriv[i] =derivBuffer[i];
	}
}





/*!
 \fn void SeparableFilter::createAndLoadRowShaders_ATI();
 \brief Procedurally Generate the Row Filter Shaders
 */
void SeparableFilter::createAndLoadRowShaders_ATI() 
{
  int i,p;

  for (i=0; i<NUM_FILTER_LEVELS; i++)
	  row_shader[i]=NULL;

  char fragmentSource[4*4096];

  for (p=0;p< num_kernel_levels; p++) 
  {
	i = kernel_levels[p];
	/////////////// Shader for ROW CONVOLUTION WITH BOUNDARY PADDING //////////////////////////////
	sprintf(fragmentSource,"\n \
		fragout main( vf30 IN, uniform sampler2D texture,                \n \
					  uniform float offset,                             \n \
					  uniform float kernel1[%d],                         \n \
					  uniform float kernel2[%d]) {                       \n \
		fragout OUT;                                                     \n \
		float4  vec0, vec1, vec2, vec3, vec4, vec5, acc1, acc2;          \n \
		float   sum1,sum2;                                               \n \
		vec0   = float4(kernel2[%d], kernel2[%d], kernel2[%d], 0.0 );    \n \
		vec1   = float4(kernel1[%d], kernel1[%d], kernel1[%d], 0.0 );    \n \
		vec2.x = tex2D(texture, IN.TEX1.xy).r;                           \n \
		vec2.y = tex2D(texture, IN.TEX0.xy).r;                           \n \
		vec2.z = tex2D(texture, IN.TEX2.xy).r;                           \n \
		vec2.w = 0.0;                                                    \n \
		acc1    = vec1.xyzw * vec2.xyzw;                                 \n \
		acc2    = vec0.xyzw * vec2.xyzw;                                 \n \
		sum1    = acc1.r + acc1.g + acc1.b ;                             \n \
		sum2    = acc2.r + acc2.g + acc2.b ;                             \n \
																		 \n",
			2*i+1, 2*i+1, 
			i-1, i, i+1, 
			i-1, i, i+1 );
	
	for (int j = 1; j <= 1; j++) //  HACK HACK !!! Save 2 ms by doing only 1 iteration. Actually j <=i/2
	{                                                        
      char loopSource[4096];                              
      if (j==1)
	  {
	  sprintf(loopSource,"\n \
		vec3   = float4( kernel1[%2d], kernel1[%2d], kernel1[%2d], kernel1[%2d]);  \n \
		vec5   = float4( kernel2[%2d], kernel2[%2d], kernel2[%2d], kernel2[%2d]);  \n \
		vec4.x = tex2D(texture, IN.TEX3.xy).r;									   \n \
		vec4.y = tex2D(texture, IN.TEX4.xy).r;								       \n \
		vec4.z = tex2D(texture, IN.TEX5.xy).r;									   \n \
		vec4.w = tex2D(texture, IN.TEX6.xy).r;									   \n \
		acc1    = vec3.xyzw * vec4.xyzw;                                           \n \
		acc2    = vec5.xyzw * vec4.xyzw;                                           \n \
		sum1    = sum1 + acc1.r + acc1.g + acc1.b + acc1.a;                        \n \
		sum2    = sum2 + acc2.r + acc2.g + acc2.b + acc2.a; \n",
			i-2*j-1 , i-2*j , i+2*j  , i+2*j+1, 
			i-2*j-1 , i-2*j , i+2*j  , i+2*j+1,
			2*j+1,    2*j ,   2*j ,   2*j+1);
	  }
	  else 
	  {
		sprintf(loopSource,"\n \
		vec3   = float4( kernel1[%2d], kernel1[%2d], kernel1[%2d], kernel1[%2d]);  \n \
		vec5   = float4( kernel2[%2d], kernel2[%2d], kernel2[%2d], kernel2[%2d]);  \n \
		vec4.x = tex2D(texture, float2(IN.TEX0.x - %2d*offset , IN.TEX0.y)).r;     \n \
		vec4.y = tex2D(texture, float2(IN.TEX0.x - %2d*offset , IN.TEX0.y)).r;     \n \
		vec4.z = tex2D(texture, float2(IN.TEX0.x + %2d*offset , IN.TEX0.y)).r;     \n \
		vec4.w = tex2D(texture, float2(IN.TEX0.x + %2d*offset , IN.TEX0.y)).r;     \n \
		acc1    = vec3.xyzw * vec4.xyzw;                                           \n \
		acc2    = vec5.xyzw * vec4.xyzw;                                           \n \
		sum1    = sum1 + acc1.r + acc1.g + acc1.b + acc1.a;                        \n \
		sum2    = sum2 + acc2.r + acc2.g + acc2.b + acc2.a; \n",
			i-2*j-1 , i-2*j , i+2*j  , i+2*j+1, 
			i-2*j-1 , i-2*j , i+2*j  , i+2*j+1,
			2*j+1,    2*j ,   2*j ,   2*j+1);
	  }
      strcat(fragmentSource,loopSource);
    } 
	strcat(fragmentSource,"\n \
		OUT.col = float4(sum1, 0.5 + 0.5 * sum2 ,1.0,1.0);                         \n \
	    return OUT;                                                                \n \
	} \n");

	///////////////////////////////////////////////////////////////////////////////////////////////
    row_shader[p] = new FragmentProgram( arch, texTarget, cgContext, fragmentProfile, fragmentSource, 0, false);
    row_shader[p]->getKernelParams();
	row_shader[p]->getOffsetParam();
  }
}







/*!
 \fn void SeparableFilter::createAndLoadRowShaders_NVIDIA();
 \brief Procedurally Generate the Row Filter Shaders
 */
void SeparableFilter::createAndLoadRowShaders_NVIDIA() 
{
  int i,p;

  for (i=0; i<NUM_FILTER_LEVELS; i++)
	  row_shader[i]=NULL;

  char fragmentSource[4*4096];

  for (p=0;p< num_kernel_levels; p++) 
  {
	i = kernel_levels[p];
	/////////////// Shader for ROW CONVOLUTION WITH BOUNDARY PADDING //////////////////////////////
	sprintf(fragmentSource,"\n \
		fragout main( vf30 IN, uniform samplerRECT texture,                \n \
					  uniform float offset,                             \n \
					  uniform float kernel1[%d],                         \n \
					  uniform float kernel2[%d]) {                       \n \
		fragout OUT;                                                     \n \
		float4  vec0, vec1, vec2, vec3, vec4, vec5, acc1, acc2;          \n \
		float   sum1,sum2;                                               \n \
		vec0   = float4(kernel2[%d], kernel2[%d], kernel2[%d], 0.0 );    \n \
		vec1   = float4(kernel1[%d], kernel1[%d], kernel1[%d], 0.0 );    \n \
		vec2.x = f4texRECT(texture, IN.TEX1.xy).r;                           \n \
		vec2.y = f4texRECT(texture, IN.TEX0.xy).r;                           \n \
		vec2.z = f4texRECT(texture, IN.TEX2.xy).r;                           \n \
		vec2.w = 0.0;                                                    \n \
		acc1    = vec1.xyzw * vec2.xyzw;                                 \n \
		acc2    = vec0.xyzw * vec2.xyzw;                                 \n \
		sum1    = acc1.r + acc1.g + acc1.b ;                             \n \
		sum2    = acc2.r + acc2.g + acc2.b ;                             \n \
																		 \n",
			2*i+1, 2*i+1, 
			i-1, i, i+1, 
			i-1, i, i+1 );
	
	for (int j = 1; j <= 1; j++) //  HACK HACK !!! Save 2 ms by doing only 1 iteration. Actually j <=i/2
	{                                                        
      char loopSource[4096];                              
      if (j==1)
	  {
	  sprintf(loopSource,"\n \
		vec3   = float4( kernel1[%2d], kernel1[%2d], kernel1[%2d], kernel1[%2d]);  \n \
		vec5   = float4( kernel2[%2d], kernel2[%2d], kernel2[%2d], kernel2[%2d]);  \n \
		vec4.x = f4texRECT(texture, IN.TEX3.xy).r;									   \n \
		vec4.y = f4texRECT(texture, IN.TEX4.xy).r;								       \n \
		vec4.z = f4texRECT(texture, IN.TEX5.xy).r;									   \n \
		vec4.w = f4texRECT(texture, IN.TEX6.xy).r;									   \n \
		acc1    = vec3.xyzw * vec4.xyzw;                                           \n \
		acc2    = vec5.xyzw * vec4.xyzw;                                           \n \
		sum1    = sum1 + acc1.r + acc1.g + acc1.b + acc1.a;                        \n \
		sum2    = sum2 + acc2.r + acc2.g + acc2.b + acc2.a; \n",
			i-2*j-1 , i-2*j , i+2*j  , i+2*j+1, 
			i-2*j-1 , i-2*j , i+2*j  , i+2*j+1,
			2*j+1,    2*j ,   2*j ,   2*j+1);
	  }
	  else 
	  {
		sprintf(loopSource,"\n \
		vec3   = float4( kernel1[%2d], kernel1[%2d], kernel1[%2d], kernel1[%2d]);  \n \
		vec5   = float4( kernel2[%2d], kernel2[%2d], kernel2[%2d], kernel2[%2d]);  \n \
		vec4.x = f4texRECT(texture, float2(IN.TEX0.x - %2d*offset , IN.TEX0.y)).r;     \n \
		vec4.y = f4texRECT(texture, float2(IN.TEX0.x - %2d*offset , IN.TEX0.y)).r;     \n \
		vec4.z = f4texRECT(texture, float2(IN.TEX0.x + %2d*offset , IN.TEX0.y)).r;     \n \
		vec4.w = f4texRECT(texture, float2(IN.TEX0.x + %2d*offset , IN.TEX0.y)).r;     \n \
		acc1    = vec3.xyzw * vec4.xyzw;                                           \n \
		acc2    = vec5.xyzw * vec4.xyzw;                                           \n \
		sum1    = sum1 + acc1.r + acc1.g + acc1.b + acc1.a;                        \n \
		sum2    = sum2 + acc2.r + acc2.g + acc2.b + acc2.a; \n",
			i-2*j-1 , i-2*j , i+2*j  , i+2*j+1, 
			i-2*j-1 , i-2*j , i+2*j  , i+2*j+1,
			2*j+1,    2*j ,   2*j ,   2*j+1);
	  }
      strcat(fragmentSource,loopSource);
    } 
	strcat(fragmentSource,"\n \
		OUT.col = float4(sum1, 0.5 + 0.5 * sum2 ,1.0,1.0);                         \n \
	    return OUT;                                                                \n \
	} \n");

	///////////////////////////////////////////////////////////////////////////////////////////////
    row_shader[p] = new FragmentProgram( arch, texTarget, cgContext, fragmentProfile, fragmentSource, 0, false);
    row_shader[p]->getKernelParams();
	row_shader[p]->getOffsetParam();
  }
}







/*!
 \fn void SeparableFilter::createAndLoadColShaders_ATI(); 
 \brief Procedurally Generate the Col Filter Shaders
 */
void SeparableFilter::createAndLoadColShaders_ATI() 
{
   int i;	
   for (i=0; i<NUM_FILTER_LEVELS; i++)
	  col_shader[i]=NULL;
	
   char fragmentSource[8192];

  for (int p=0;p < num_kernel_levels; p++) 
  {
	  i = kernel_levels[p];

    //////////////////  Shader for COL CONVOLUTION WITH BOUNDARY PADDING  /////////////////////////
    sprintf(fragmentSource,"\n \
		fragout main( vf30 IN, uniform sampler2D   texture,             \n \
					  uniform float offset,                            \n \
					  uniform float kernel1[%d],                        \n \
					  uniform float kernel2[%d]) {                      \n \
		fragout  OUT;                                                   \n \
		float4   vec0, vec1, vec2, vec3, vec4, vec5, vec6, orig4, acc1, acc2, acc3; \n \
		float    sum1, sum2, sum3;			                            \n \
		float2   t1,t2,t3,t4;											\n \
		orig4  = tex2D(texture, IN.TEX0.xy);                            \n \
		vec1   = float4(kernel1[%d], kernel1[%d], kernel1[%d], 0.0 );   \n \
		vec0   = float4(kernel2[%d], kernel2[%d], kernel2[%d], 0.0 );   \n \
		vec2.x = tex2D(texture, IN.TEX1.xy).r;                          \n \
		vec2.y = orig4.r;                                               \n \
		vec2.z = tex2D(texture, IN.TEX2.xy).r;                          \n \
		vec2.w = 0.0;                                                   \n \
		acc1   = vec1.xyzw * vec2.xyzw;                                 \n \
		acc2   = vec0.xyzw * vec2.xyzw;                                 \n \
		sum1   = acc1.r + acc1.g + acc1.b ;                             \n \
		sum2   = acc2.r + acc2.g + acc2.b ;                             \n \
		vec2.x = tex2D(texture, IN.TEX1.xy).g;							\n \
		vec2.y = orig4.g;                                               \n \
		vec2.z = tex2D(texture, IN.TEX2.xy).g;							\n \
		vec2.w = 0.0;                                                   \n \
		vec2   = 2 * (vec2 - float4(0.5, 0.5, 0.5, 0.5));               \n \
		acc3   = vec1.xyzw * vec2.xyzw;                                 \n \
		sum3   = acc3.r + acc3.g + acc3.b ;                             \n",
		2*i+1,2*i+1, i-1,i,i+1, i-1,i,i+1); 
	
	for (int j = 1; j <= 1; j++) //  HACK HACK !!! Save 2 ms by doing only 1 iteration. Actually j <=i/2
	{                                                        
      char loopSource[4096];             
	  
	  if (j==1)
	  {
	  	sprintf(loopSource,"\n \
		t1 = IN.TEX3.xy;														  \n \
		t2 = IN.TEX4.xy;														  \n \
		t3 = IN.TEX5.xy;														  \n \
		t4 = IN.TEX6.xy;														  \n");
	  }
	  else
	  {
		sprintf(loopSource,"\n \
		t1 = float2(IN.TEX0.x , IN.TEX0.y - %2d*offset);						  \n \
		t2 = float2(IN.TEX0.x , IN.TEX0.y - %2d*offset);					      \n \
		t3 = float2(IN.TEX0.x , IN.TEX0.y + %2d*offset);			              \n \
		t4 = float2(IN.TEX0.x , IN.TEX0.y + %2d*offset);						  \n",
				2*j+1,    2*j,   2*j,   2*j+1);
	  }
	  strcat(fragmentSource,loopSource);
		sprintf(loopSource,"\n \
		vec3   = float4( kernel1[%2d], kernel1[%2d], kernel1[%2d], kernel1[%2d]); \n \
		vec5   = float4( kernel2[%2d], kernel2[%2d], kernel2[%2d], kernel2[%2d]); \n \
		vec4.x = tex2D(texture, t1 ).r;											  \n \
		vec4.y = tex2D(texture, t2 ).r;                                           \n \
		vec4.z = tex2D(texture, t3 ).r;                                           \n \
		vec4.w = tex2D(texture, t4 ).r;                                           \n \
		acc1   = vec3.xyzw * vec4.xyzw;                                           \n \
		acc2   = vec5.xyzw * vec4.xyzw;                                           \n \
		sum1  += acc1.r + acc1.g + acc1.b + acc1.a;                               \n \
		sum2  += acc2.r + acc2.g + acc2.b + acc2.a;                               \n \
		vec4.x = tex2D(texture, t1 ).g;									          \n \
		vec4.y = tex2D(texture, t2 ).g;											  \n \
		vec4.z = tex2D(texture, t3 ).g;											  \n \
		vec4.w = tex2D(texture, t4 ).g;											  \n \
		vec4   = 2 * (vec4 - float4(0.5,0.5,0.5,0.5));                            \n \
		acc3   = vec3.xyzw * vec4.xyzw;                                           \n \
		sum3  += acc3.r + acc3.g + acc3.b + acc3.a; \n",
	
		i-2*j-1 , i-2*j , i+2*j  , i+2*j+1,   
		i-2*j-1 , i-2*j , i+2*j  , i+2*j+1);
	
      strcat(fragmentSource,loopSource);
    } 
	char endSource[1024];
	sprintf(endSource,"\n \
		OUT.col = float4(sum1,0.5 + 0.5*sum3 ,0.5 + 0.5*sum2,1.0);                 \n \
	    return OUT;                                                                \n \
	} \n");
	strcat(fragmentSource,endSource);

    //////////////////////////////////////////////////////////////////////////////////////////
	col_shader[p] = new FragmentProgram( arch, texTarget, cgContext, fragmentProfile, fragmentSource, 0, false);
	col_shader[p]->getKernelParams();
	col_shader[p]->getOffsetParam();
  }
}





/*!
 \fn void SeparableFilter::createAndLoadColShaders_NVIDIA(); 
 \brief Procedurally Generate the Col Filter Shaders
 */
void SeparableFilter::createAndLoadColShaders_NVIDIA() 
{
   int i;	
   for (i=0; i<NUM_FILTER_LEVELS; i++)
	  col_shader[i]=NULL;
	
   char fragmentSource[8192];

  for (int p=0;p < num_kernel_levels; p++) 
  {
	  i = kernel_levels[p];

    //////////////////  Shader for COL CONVOLUTION WITH BOUNDARY PADDING  /////////////////////////
    sprintf(fragmentSource,"\n \
		fragout main( vf30 IN, uniform samplerRECT   texture,             \n \
					  uniform float offset,                            \n \
					  uniform float kernel1[%d],                        \n \
					  uniform float kernel2[%d]) {                      \n \
		fragout  OUT;                                                   \n \
		float4   vec0, vec1, vec2, vec3, vec4, vec5, vec6, orig4, acc1, acc2, acc3; \n \
		float    sum1, sum2, sum3;			                            \n \
		float2   t1,t2,t3,t4;											\n \
		orig4  = f4texRECT(texture, IN.TEX0.xy);                            \n \
		vec1   = float4(kernel1[%d], kernel1[%d], kernel1[%d], 0.0 );   \n \
		vec0   = float4(kernel2[%d], kernel2[%d], kernel2[%d], 0.0 );   \n \
		vec2.x = f4texRECT(texture, IN.TEX1.xy).r;                          \n \
		vec2.y = orig4.r;                                               \n \
		vec2.z = f4texRECT(texture, IN.TEX2.xy).r;                          \n \
		vec2.w = 0.0;                                                   \n \
		acc1   = vec1.xyzw * vec2.xyzw;                                 \n \
		acc2   = vec0.xyzw * vec2.xyzw;                                 \n \
		sum1   = acc1.r + acc1.g + acc1.b ;                             \n \
		sum2   = acc2.r + acc2.g + acc2.b ;                             \n \
		vec2.x = f4texRECT(texture, IN.TEX1.xy).g;							\n \
		vec2.y = orig4.g;                                               \n \
		vec2.z = f4texRECT(texture, IN.TEX2.xy).g;							\n \
		vec2.w = 0.0;                                                   \n \
		vec2   = 2 * (vec2 - float4(0.5, 0.5, 0.5, 0.5));               \n \
		acc3   = vec1.xyzw * vec2.xyzw;                                 \n \
		sum3   = acc3.r + acc3.g + acc3.b ;                             \n",
		2*i+1,2*i+1, i-1,i,i+1, i-1,i,i+1); 
	
	for (int j = 1; j <= 1; j++) //  HACK HACK !!! Save 2 ms by doing only 1 iteration. Actually j <=i/2
	{                                                        
      char loopSource[4096];             
	  
	  if (j==1)
	  {
	  	sprintf(loopSource,"\n \
		t1 = IN.TEX3.xy;														  \n \
		t2 = IN.TEX4.xy;														  \n \
		t3 = IN.TEX5.xy;														  \n \
		t4 = IN.TEX6.xy;														  \n");
	  }
	  else
	  {
		sprintf(loopSource,"\n \
		t1 = float2(IN.TEX0.x , IN.TEX0.y - %2d*offset);						  \n \
		t2 = float2(IN.TEX0.x , IN.TEX0.y - %2d*offset);					      \n \
		t3 = float2(IN.TEX0.x , IN.TEX0.y + %2d*offset);			              \n \
		t4 = float2(IN.TEX0.x , IN.TEX0.y + %2d*offset);						  \n",
				2*j+1,    2*j,   2*j,   2*j+1);
	  }
	  strcat(fragmentSource,loopSource);
		sprintf(loopSource,"\n \
		vec3   = float4( kernel1[%2d], kernel1[%2d], kernel1[%2d], kernel1[%2d]); \n \
		vec5   = float4( kernel2[%2d], kernel2[%2d], kernel2[%2d], kernel2[%2d]); \n \
		vec4.x = f4texRECT(texture, t1 ).r;											  \n \
		vec4.y = f4texRECT(texture, t2 ).r;                                           \n \
		vec4.z = f4texRECT(texture, t3 ).r;                                           \n \
		vec4.w = f4texRECT(texture, t4 ).r;                                           \n \
		acc1   = vec3.xyzw * vec4.xyzw;                                           \n \
		acc2   = vec5.xyzw * vec4.xyzw;                                           \n \
		sum1  += acc1.r + acc1.g + acc1.b + acc1.a;                               \n \
		sum2  += acc2.r + acc2.g + acc2.b + acc2.a;                               \n \
		vec4.x = f4texRECT(texture, t1 ).g;									          \n \
		vec4.y = f4texRECT(texture, t2 ).g;											  \n \
		vec4.z = f4texRECT(texture, t3 ).g;											  \n \
		vec4.w = f4texRECT(texture, t4 ).g;											  \n \
		vec4   = 2 * (vec4 - float4(0.5,0.5,0.5,0.5));                            \n \
		acc3   = vec3.xyzw * vec4.xyzw;                                           \n \
		sum3  += acc3.r + acc3.g + acc3.b + acc3.a; \n",
	
		i-2*j-1 , i-2*j , i+2*j  , i+2*j+1,   
		i-2*j-1 , i-2*j , i+2*j  , i+2*j+1);
	
      strcat(fragmentSource,loopSource);
    } 
	char endSource[1024];
	sprintf(endSource,"\n \
		OUT.col = float4(sum1,0.5 + 0.5*sum3 ,0.5 + 0.5*sum2,1.0);                 \n \
		//OUT.col = float4(0.8); \n \
	    return OUT;                                                                \n \
	} \n");
	strcat(fragmentSource,endSource);

    //////////////////////////////////////////////////////////////////////////////////////////
	col_shader[p] = new FragmentProgram( arch, texTarget, cgContext, fragmentProfile, fragmentSource, 0, false);
	col_shader[p]->getKernelParams();
	col_shader[p]->getOffsetParam();
  }
}






/*!
\fn void SeparableFilter::bindInput(GLuint texID)
\brief bind the particular texture unit.
*/
void SeparableFilter::bindInput(GLuint texID)
{
	texInputID = texID;
}

/*!
\fn void SeparableFilter::convolveRow(float Op_top, float Op_left, float Op_bot, float Op_right, 
								  float top, float left, float bot, float right, float offset)
\brief Call 1D Row Convolution
*/
void SeparableFilter::convolveRow(float Op_top, float Op_left, float Op_bot, float Op_right, 
								  float top, float left, float bot, float right, float offset)
{
	int index, prog = width/2;
	for (int k=0;k<num_kernel_levels;k++)
		if (prog == kernel_levels[k])
			index = k;
	
	float vtop, vbot, vleft, vright;

	row_shader[index]->activate();
	row_shader[index]->setParams( width, row_kernel, row_deriv, offset );
    row_shader[index]->bindTexture(texInputID);

	vtop   = (2.0 * Op_top  ) / ranH - 1 ;
	vbot   = (2.0 * Op_bot  ) / ranH - 1 ;
	vleft  = (2.0 * Op_left ) / ranW - 1 ;
	vright = (2.0 * Op_right) / ranW - 1 ;
	
	glActiveTextureARB(GL_TEXTURE0_ARB);
    glActiveTextureARB(GL_TEXTURE1_ARB);
	glActiveTextureARB(GL_TEXTURE2_ARB);
	glActiveTextureARB(GL_TEXTURE3_ARB);
	glActiveTextureARB(GL_TEXTURE4_ARB);
	glActiveTextureARB(GL_TEXTURE5_ARB);
	glActiveTextureARB(GL_TEXTURE6_ARB);

	glEnable(texTarget);

	glBegin (GL_QUADS);
	    
	    // upper left      
		glMultiTexCoord2fARB(GL_TEXTURE0_ARB,     left           ,   bot  );
		glMultiTexCoord2fARB(GL_TEXTURE1_ARB,     left -   offset,   bot  );
		glMultiTexCoord2fARB(GL_TEXTURE2_ARB,     left +   offset,   bot  );
		glMultiTexCoord2fARB(GL_TEXTURE3_ARB,     left - 3*offset,   bot  );
		glMultiTexCoord2fARB(GL_TEXTURE4_ARB,     left - 2*offset,   bot  );
		glMultiTexCoord2fARB(GL_TEXTURE5_ARB,     left + 2*offset,   bot  );
		glMultiTexCoord2fARB(GL_TEXTURE6_ARB,     left + 3*offset,   bot  );
		glVertex2f   (vleft     ,   vbot   );

		// upper right
		glMultiTexCoord2fARB(GL_TEXTURE0_ARB,	 right           ,   bot  ); 
		glMultiTexCoord2fARB(GL_TEXTURE1_ARB,	 right -   offset,	 bot  ); 
		glMultiTexCoord2fARB(GL_TEXTURE2_ARB,	 right +   offset,	 bot  ); 
		glMultiTexCoord2fARB(GL_TEXTURE3_ARB,    right - 3*offset,   bot  );
		glMultiTexCoord2fARB(GL_TEXTURE4_ARB,    right - 2*offset,   bot  );
		glMultiTexCoord2fARB(GL_TEXTURE5_ARB,    right + 2*offset,   bot  );
		glMultiTexCoord2fARB(GL_TEXTURE6_ARB,    right + 3*offset,   bot  );
        glVertex2f   (vright    ,   vbot   );

        // lower right
		glMultiTexCoord2fARB(GL_TEXTURE0_ARB,	 right            ,   top );
		glMultiTexCoord2fARB(GL_TEXTURE1_ARB,	 right -   offset ,   top );
		glMultiTexCoord2fARB(GL_TEXTURE2_ARB,	 right +   offset ,   top );
		glMultiTexCoord2fARB(GL_TEXTURE3_ARB,	 right - 3*offset ,   top );
		glMultiTexCoord2fARB(GL_TEXTURE4_ARB,	 right - 2*offset ,   top );
		glMultiTexCoord2fARB(GL_TEXTURE5_ARB,	 right + 2*offset ,   top );
		glMultiTexCoord2fARB(GL_TEXTURE6_ARB,	 right + 3*offset ,   top );
		glVertex2f   (vright    ,   vtop   );
	
		// lower left 
		glMultiTexCoord2fARB(GL_TEXTURE0_ARB,	  left           ,   top  );
		glMultiTexCoord2fARB(GL_TEXTURE1_ARB,	  left -   offset,   top  );
		glMultiTexCoord2fARB(GL_TEXTURE2_ARB,	  left +   offset,   top  );
		glMultiTexCoord2fARB(GL_TEXTURE3_ARB,	  left - 3*offset,   top  );
		glMultiTexCoord2fARB(GL_TEXTURE4_ARB,	  left - 2*offset,   top  );
		glMultiTexCoord2fARB(GL_TEXTURE5_ARB,	  left + 2*offset,   top  );
		glMultiTexCoord2fARB(GL_TEXTURE6_ARB,	  left + 3*offset,   top  );
		glVertex2f   ( vleft    ,   vtop   );

    glEnd ();

	glDisable(texTarget);

    row_shader[index]->deactivate();
}


/*!
\fn void SeparableFilter::convolveCol(float Op_top, float Op_left, float Op_bot, float Op_right, 
								  float top, float left, float bot, float right, float offset)
\brief Call 1D Row Convolution (Default Option)
*/
void SeparableFilter::convolveCol(float Op_top, float Op_left, float Op_bot, float Op_right, 
								  float top, float left, float bot, float right, float offset)
{
	int index, prog = height/2;
	for (int k=0;k<num_kernel_levels;k++)
		if (prog == kernel_levels[k])
			index = k;

	float vtop, vbot, vleft, vright;

    col_shader[index]->activate();
	col_shader[index]->setParams( height, col_kernel , col_deriv, offset );
    col_shader[index]->bindTexture(texInputID);

	vtop   = (2.0 * Op_top  ) / ranH - 1 ;
	vbot   = (2.0 * Op_bot  ) / ranH - 1 ;
	vleft  = (2.0 * Op_left ) / ranW - 1 ;
	vright = (2.0 * Op_right) / ranW - 1 ;

	
	glActiveTextureARB(GL_TEXTURE0_ARB);
    glActiveTextureARB(GL_TEXTURE1_ARB);
	glActiveTextureARB(GL_TEXTURE2_ARB);
	glActiveTextureARB(GL_TEXTURE3_ARB);
	glActiveTextureARB(GL_TEXTURE4_ARB);
	glActiveTextureARB(GL_TEXTURE5_ARB);
	glActiveTextureARB(GL_TEXTURE6_ARB);

	glEnable(texTarget);
	
	glBegin (GL_QUADS);
	    
	    // upper left      
		glMultiTexCoord2fARB(GL_TEXTURE0_ARB,     left,   bot            );
		glMultiTexCoord2fARB(GL_TEXTURE1_ARB,     left,   bot -   offset );
		glMultiTexCoord2fARB(GL_TEXTURE2_ARB,     left,   bot +   offset );
		glMultiTexCoord2fARB(GL_TEXTURE3_ARB,     left,   bot - 3*offset );
		glMultiTexCoord2fARB(GL_TEXTURE4_ARB,     left,   bot - 2*offset );
		glMultiTexCoord2fARB(GL_TEXTURE5_ARB,     left,   bot + 2*offset );
		glMultiTexCoord2fARB(GL_TEXTURE6_ARB,     left,   bot + 3*offset );
		glVertex2f   (vleft     ,   vbot   );

		// upper right
		glMultiTexCoord2fARB(GL_TEXTURE0_ARB,	 right,	  bot            ); 
		glMultiTexCoord2fARB(GL_TEXTURE1_ARB,	 right,	  bot -   offset ); 
		glMultiTexCoord2fARB(GL_TEXTURE2_ARB,	 right,	  bot +   offset ); 
		glMultiTexCoord2fARB(GL_TEXTURE3_ARB,	 right,	  bot - 3*offset ); 
		glMultiTexCoord2fARB(GL_TEXTURE4_ARB,	 right,	  bot - 2*offset ); 
		glMultiTexCoord2fARB(GL_TEXTURE5_ARB,	 right,	  bot + 2*offset ); 
		glMultiTexCoord2fARB(GL_TEXTURE6_ARB,	 right,	  bot + 3*offset ); 
        glVertex2f   (vright    ,   vbot   );

        // lower right
		glMultiTexCoord2fARB(GL_TEXTURE0_ARB,	 right,   top	   	     );
		glMultiTexCoord2fARB(GL_TEXTURE1_ARB,	 right,   top -   offset );
		glMultiTexCoord2fARB(GL_TEXTURE2_ARB,	 right,   top +   offset );
		glMultiTexCoord2fARB(GL_TEXTURE3_ARB,	 right,   top - 3*offset );
		glMultiTexCoord2fARB(GL_TEXTURE4_ARB,	 right,   top - 2*offset );
		glMultiTexCoord2fARB(GL_TEXTURE5_ARB,	 right,   top + 2*offset );
		glMultiTexCoord2fARB(GL_TEXTURE6_ARB,	 right,   top + 3*offset );
		glVertex2f   (vright    ,   vtop   );
	
		// lower left 
		glMultiTexCoord2fARB(GL_TEXTURE0_ARB,	  left,   top	   	     );
		glMultiTexCoord2fARB(GL_TEXTURE1_ARB,	  left,   top -   offset );
		glMultiTexCoord2fARB(GL_TEXTURE2_ARB,	  left,   top +   offset );
		glMultiTexCoord2fARB(GL_TEXTURE3_ARB,	  left,   top - 3*offset );
		glMultiTexCoord2fARB(GL_TEXTURE4_ARB,	  left,   top - 2*offset );
		glMultiTexCoord2fARB(GL_TEXTURE5_ARB,	  left,   top + 2*offset );
		glMultiTexCoord2fARB(GL_TEXTURE6_ARB,	  left,   top + 3*offset );
		glVertex2f   ( vleft    ,   vtop   );

    glEnd ();

	glDisable(texTarget);

    col_shader[index]->deactivate();
}
