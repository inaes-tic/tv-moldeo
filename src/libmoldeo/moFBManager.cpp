/*******************************************************************************

                              moFBManager.cpp

  ****************************************************************************
  *                                                                          *
  *   This source is free software; you can redistribute it and/or modify    *
  *   it under the terms of the GNU General Public License as published by   *
  *   the Free Software Foundation; either version 2 of the License, or      *
  *  (at your option) any later version.                                    *
  *                                                                          *
  *   This code is distributed in the hope that it will be useful, but       *
  *   WITHOUT ANY WARRANTY; without even the implied warranty of             *
  *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU      *
  *   General Public License for more details.                               *
  *                                                                          *
  *   A copy of the GNU General Public License is available on the World     *
  *   Wide Web at <http://www.gnu.org/copyleft/gpl.html>. You can also       *
  *   obtain it by writing to the Free Software Foundation,                  *
  *   Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.         *
  *                                                                          *
  ****************************************************************************

  Copyright(C) 2006 Fabricio Costa

  Authors:
  Fabricio Costa
  Andrés Colubri

*******************************************************************************/

#include "moFBManager.h"

//===========================================
//
//				moFBManager
//
//===========================================

moFBManager::moFBManager()
{
	SetType( MO_OBJECT_RESOURCE );
	SetResourceType( MO_RESOURCETYPE_FB );
    SetName("FB Manager");
	m_glmanager = NULL;
}

moFBManager::~moFBManager()
{
	Finish();
}

MOboolean moFBManager::Init()
{
	if ( m_pResourceManager ) {
		m_glmanager = m_pResourceManager->GetGLMan();
	}
	m_fbo_array.Init(0, NULL);
	return (m_glmanager!=NULL);
}

MOboolean moFBManager::Finish()
{
	m_fbo_array.Finish();
	return true;
}

MOuint moFBManager::CreateFBO(MOboolean p_screen_fbo)
{
	moFBO* new_fbo = new moFBO();
	new_fbo->Init(p_screen_fbo);
	m_fbo_array.Add(new_fbo);
	return m_fbo_array.Count() - 1;
}

MOboolean moFBManager::DeleteFBO(MOuint p_fbo)
{
	if ( p_fbo!=(MOuint)MO_UNDEFINED && p_fbo < m_fbo_array.Count())
	{
		m_fbo_array.Remove(p_fbo);
		return true;
	}
	else return false;
}

MOboolean moFBManager::BindFBO(MOuint p_fbo)
{
	if ( p_fbo!=(MOuint)MO_UNDEFINED && p_fbo < m_fbo_array.Count())
	{
		m_fbo_array[p_fbo]->Bind();
		return true;
	}
	else return false; // Wrong FBO index.
}
