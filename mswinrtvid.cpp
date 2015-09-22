/*
mswinrtvid.cpp

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


#include "mediastreamer2/msfilter.h"
#include "mediastreamer2/msvideo.h"
#include "mediastreamer2/mswebcam.h"

#include "mswinrtcap.h"
#include "mswinrtdis.h"
#ifdef MS2_WINDOWS_PHONE
#include "IVideoRenderer.h"
#endif


using namespace libmswinrtvid;
#ifdef MS2_WINDOWS_PHONE
using namespace Mediastreamer2::WinRTVideo;
#endif


/******************************************************************************
 * Methods to (de)initialize and run the WinRT video capture filter           *
 *****************************************************************************/

static void ms_winrtcap_read_init(MSFilter *f) {
	MSWinRTCap *r = new MSWinRTCap();
	f->data = r;
}

static void ms_winrtcap_read_preprocess(MSFilter *f) {
	MSWinRTCap *r = static_cast<MSWinRTCap *>(f->data);
	r->activate();
}

static void ms_winrtcap_read_process(MSFilter *f) {
	MSWinRTCap *r = static_cast<MSWinRTCap *>(f->data);
	if (!r->isStarted()) {
		r->start();
	}
	r->feed(f);
}

static void ms_winrtcap_read_postprocess(MSFilter *f) {
	MSWinRTCap *r = static_cast<MSWinRTCap *>(f->data);
	r->stop();
	r->deactivate();
}

static void ms_winrtcap_read_uninit(MSFilter *f) {
	MSWinRTCap *r = static_cast<MSWinRTCap *>(f->data);
	delete r;
}


/******************************************************************************
 * Methods to configure the WinRT video capture filter                        *
 *****************************************************************************/

static int ms_winrtcap_get_fps(MSFilter *f, void *arg) {
	MSWinRTCap *r = static_cast<MSWinRTCap *>(f->data);
	if (f->ticker) {
		*((float *)arg) = r->getAverageFps();
	} else {
		*((float *)arg) = r->getFps();
	}
	return 0;
}

static int ms_winrtcap_set_fps(MSFilter *f, void *arg) {
	MSWinRTCap *r = static_cast<MSWinRTCap *>(f->data);
	r->setFps(*((float *)arg));
	return 0;
}

static int ms_winrtcap_get_pix_fmt(MSFilter *f, void *arg) {
	MSWinRTCap *r = static_cast<MSWinRTCap *>(f->data);
	MSPixFmt *fmt = static_cast<MSPixFmt *>(arg);
	*fmt = r->getPixFmt();
	return 0;
}

static int ms_winrtcap_get_vsize(MSFilter *f, void *arg) {
	MSWinRTCap *r = static_cast<MSWinRTCap *>(f->data);
	MSVideoSize *vs = static_cast<MSVideoSize *>(arg);
	*vs = r->getVideoSize();
	return 0;
}

static int ms_winrtcap_set_vsize(MSFilter *f, void *arg) {
	MSWinRTCap *r = static_cast<MSWinRTCap *>(f->data);
	MSVideoSize *vs = static_cast<MSVideoSize *>(arg);
	r->setVideoSize(*vs);
	return 0;
}

static int ms_winrtcap_set_device_orientation(MSFilter *f, void *arg) {
	MSWinRTCap *r = static_cast<MSWinRTCap *>(f->data);
	int orientation = *((int *)arg);
	if (r->getDeviceOrientation() != orientation) {
		r->setDeviceOrientation(orientation);
	}
	return 0;
}

static int ms_winrtcap_set_native_window_id(MSFilter *f, void *arg) {
#ifdef MS2_WINDOWS_UNIVERSAL
	MSWinRTCap *r = static_cast<MSWinRTCap *>(f->data);
	RefToPtrProxy<Platform::Object^> *proxy = static_cast<RefToPtrProxy<Platform::Object^>*>((void *)(*((PULONG_PTR)arg)));
	Windows::UI::Xaml::Controls::CaptureElement^ captureElement = dynamic_cast<Windows::UI::Xaml::Controls::CaptureElement^>(proxy->Ref());
	r->setCaptureElement(captureElement);
#endif
	return 0;
}

static MSFilterMethod ms_winrtcap_read_methods[] = {
	{ MS_FILTER_GET_FPS,                           ms_winrtcap_get_fps                    },
	{ MS_FILTER_SET_FPS,                           ms_winrtcap_set_fps                    },
	{ MS_FILTER_GET_PIX_FMT,                       ms_winrtcap_get_pix_fmt                },
	{ MS_FILTER_GET_VIDEO_SIZE,                    ms_winrtcap_get_vsize                  },
	{ MS_FILTER_SET_VIDEO_SIZE,                    ms_winrtcap_set_vsize                  },
	{ MS_VIDEO_CAPTURE_SET_DEVICE_ORIENTATION,     ms_winrtcap_set_device_orientation     },
	{ MS_VIDEO_DISPLAY_SET_NATIVE_WINDOW_ID,       ms_winrtcap_set_native_window_id       },
	{ 0,                                           NULL                                   }
};


/******************************************************************************
 * Definition of the WinRT video capture filter                               *
 *****************************************************************************/

#define MS_WINRTCAP_READ_ID          MS_FILTER_PLUGIN_ID
#define MS_WINRTCAP_READ_NAME        "MSWinRTCap"
#define MS_WINRTCAP_READ_DESCRIPTION "WinRT video capture"
#define MS_WINRTCAP_READ_CATEGORY    MS_FILTER_OTHER
#define MS_WINRTCAP_READ_ENC_FMT     NULL
#define MS_WINRTCAP_READ_NINPUTS     0
#define MS_WINRTCAP_READ_NOUTPUTS    1
#define MS_WINRTCAP_READ_FLAGS       0

MSFilterDesc ms_winrtcap_read_desc = {
	MS_WINRTCAP_READ_ID,
	MS_WINRTCAP_READ_NAME,
	MS_WINRTCAP_READ_DESCRIPTION,
	MS_WINRTCAP_READ_CATEGORY,
	MS_WINRTCAP_READ_ENC_FMT,
	MS_WINRTCAP_READ_NINPUTS,
	MS_WINRTCAP_READ_NOUTPUTS,
	ms_winrtcap_read_init,
	ms_winrtcap_read_preprocess,
	ms_winrtcap_read_process,
	ms_winrtcap_read_postprocess,
	ms_winrtcap_read_uninit,
	ms_winrtcap_read_methods,
	MS_WINRTCAP_READ_FLAGS
};

MS_FILTER_DESC_EXPORT(ms_winrtcap_read_desc)



/******************************************************************************
 * Definition of the WinRT video camera detection                             *
 *****************************************************************************/

static void ms_winrtcap_detect(MSWebCamManager *m);

static MSFilter *ms_winrtcap_create_reader(MSWebCam *cam) {
	MSFilter *f = ms_filter_new_from_desc(&ms_winrtcap_read_desc);
	MSWinRTCap *r = static_cast<MSWinRTCap *>(f->data);
	WinRTWebcam* winrtcam = static_cast<WinRTWebcam *>(cam->data);
	r->setDeviceId(ref new Platform::String(winrtcam->id));
	r->setFront(winrtcam->front == TRUE);
	r->setExternal(winrtcam->external == TRUE);
	return f;
}

static MSWebCamDesc ms_winrtcap_desc = {
	"MSWinRTCap",
	ms_winrtcap_detect,
	NULL,
	ms_winrtcap_create_reader,
	NULL,
	NULL
};

static void ms_winrtcap_detect(MSWebCamManager *m) {
	MSWinRTCap::detectCameras(m, &ms_winrtcap_desc);
}



/******************************************************************************
 * Methods to (de)initialize and run the WinRT video display filter           *
 *****************************************************************************/

static void ms_winrtdis_init(MSFilter *f) {
	MSWinRTDis *w = new MSWinRTDis();
	f->data = w;
}

static void ms_winrtdis_preprocess(MSFilter *f) {
	MSWinRTDis *w = static_cast<MSWinRTDis *>(f->data);
	w->activate();
	w->start();
}

static void ms_winrtdis_process(MSFilter *f) {
	MSWinRTDis *w = static_cast<MSWinRTDis *>(f->data);
	if (w->isStarted()) {
		w->feed(f);
	}
}

static void ms_winrtdis_postprocess(MSFilter *f) {
	MSWinRTDis *w = static_cast<MSWinRTDis *>(f->data);
	w->stop();
	w->deactivate();
}

static void ms_winrtdis_uninit(MSFilter *f) {
	MSWinRTDis *w = static_cast<MSWinRTDis *>(f->data);
	delete w;
}


/******************************************************************************
 * Methods to configure the WinRT video display filter                        *
 *****************************************************************************/

static int ms_winrtdis_get_vsize(MSFilter *f, void *arg) {
	MSWinRTDis *w = static_cast<MSWinRTDis *>(f->data);
	*((MSVideoSize *)arg) = w->getVideoSize();
	return 0;
}

static int ms_winrtdis_set_vsize(MSFilter *f, void *arg) {
	MSWinRTDis *w = static_cast<MSWinRTDis *>(f->data);
	MSVideoSize *vs = static_cast<MSVideoSize *>(arg);
	w->setVideoSize(*vs);
	return 0;
}

static int ms_winrtdis_set_native_window_id(MSFilter *f, void *arg) {
	MSWinRTDis *w = static_cast<MSWinRTDis *>(f->data);
	RefToPtrProxy<Platform::Object^> *proxy = static_cast<RefToPtrProxy<Platform::Object^>*>((void *)(*((PULONG_PTR)arg)));
	Windows::UI::Xaml::Controls::MediaElement^ mediaElement = dynamic_cast<Windows::UI::Xaml::Controls::MediaElement^>(proxy->Ref());
	w->setMediaElement(mediaElement);
	return 0;
}

static MSFilterMethod ms_winrtdis_methods[] = {
	{ MS_FILTER_GET_VIDEO_SIZE,              ms_winrtdis_get_vsize            },
	{ MS_FILTER_SET_VIDEO_SIZE,              ms_winrtdis_set_vsize            },
	{ MS_VIDEO_DISPLAY_SET_NATIVE_WINDOW_ID, ms_winrtdis_set_native_window_id },
	{ 0,                                     NULL                             }
};


/******************************************************************************
 * Definition of the WinRT video display filter                               *
 *****************************************************************************/

#define MS_WINRTDIS_ID          MS_FILTER_PLUGIN_ID
#define MS_WINRTDIS_NAME        "MSWinRTDis"
#define MS_WINRTDIS_DESCRIPTION "WinRT video display"
#define MS_WINRTDIS_CATEGORY    MS_FILTER_OTHER
#define MS_WINRTDIS_ENC_FMT     NULL
#define MS_WINRTDIS_NINPUTS     2
#define MS_WINRTDIS_NOUTPUTS    0
#define MS_WINRTDIS_FLAGS       0

MSFilterDesc ms_winrtdis_desc = {
	MS_WINRTDIS_ID,
	MS_WINRTDIS_NAME,
	MS_WINRTDIS_DESCRIPTION,
	MS_WINRTDIS_CATEGORY,
	MS_WINRTDIS_ENC_FMT,
	MS_WINRTDIS_NINPUTS,
	MS_WINRTDIS_NOUTPUTS,
	ms_winrtdis_init,
	ms_winrtdis_preprocess,
	ms_winrtdis_process,
	ms_winrtdis_postprocess,
	ms_winrtdis_uninit,
	ms_winrtdis_methods,
	MS_WINRTDIS_FLAGS
};

MS_FILTER_DESC_EXPORT(ms_winrtdis_desc)




extern "C" __declspec(dllexport) void libmswinrtvid_init(void) {
	MSWebCamManager *manager = ms_web_cam_manager_get();
	ms_web_cam_manager_register_desc(manager, &ms_winrtcap_desc);
	ms_filter_register(&ms_winrtcap_read_desc);
	ms_filter_register(&ms_winrtdis_desc);
	ms_message("libmswinrtvid plugin loaded");
}
