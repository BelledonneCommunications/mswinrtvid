/*
IVideoRenderer.h

mediastreamer2 library - modular sound and video processing and streaming
Windows Audio Session API sound card plugin for mediastreamer2
Copyright (C) 2010-2013 Belledonne Communications, Grenoble, France

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/


#pragma once

#include "IVideoDispatcher.h"

namespace Mediastreamer2
{
	namespace WP8Video
	{
		public interface class IVideoRenderer
		{
		public:
			void Start(Platform::String^ format, int width, int height);
			void Stop();
			void ChangeFormat(Platform::String^ format, int width, int height);
			IVideoDispatcher^ GetDispatcher();
		};
	}
}
