/*
SwapChainPanelSource.h

mediastreamer2 library - modular sound and video processing and streaming
Windows Audio Session API sound card plugin for mediastreamer2
Copyright (C) 2010-2015 Belledonne Communications, Grenoble, France

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

#include "SharedData.h"

namespace libmswinrtvid
{
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class SwapChainPanelSource sealed
	{
	public:
		SwapChainPanelSource();
		virtual ~SwapChainPanelSource();

		void Start(Windows::UI::Xaml::Controls::SwapChainPanel^ swapChainPanel);
		void Stop();

	private:
		Windows::Foundation::IAsyncAction^ GetEvents();
		void OnUnloaded(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e);

		Windows::UI::Xaml::Controls::SwapChainPanel^ mSwapChainPanel;
		Microsoft::WRL::ComPtr<ISwapChainPanelNative2> mNativeSwapChainPanel;
		HANDLE mCurrentSwapChainHandle;
		HANDLE mMemoryMapping;
		SharedData* mSharedData;
	};
}
