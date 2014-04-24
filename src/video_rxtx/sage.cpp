/**
 * @file   video_rxtx/sage.cpp
 * @author Martin Pulec     <pulec@cesnet.cz>
 */
/*
 * Copyright (c) 2013-2014 CESNET z.s.p.o.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, is permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of CESNET nor the names of its contributors may be
 *    used to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHORS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESSED OR IMPLIED WARRANTIES, INCLUDING,
 * BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#ifdef HAVE_CONFIG_H
#include "config.h"
#include "config_unix.h"
#include "config_win32.h"
#endif // HAVE_CONFIG_H

#include <string>

#include "host.h"
#include "video_display.h"
#include "video_rxtx/sage.h"
#include "video.h"

using namespace std;

sage_video_rxtx::sage_video_rxtx(struct module *parent, struct video_export *video_exporter,
                        const char *requested_compression,
                        const char *requested_receiver, const char *sage_opts) :
        video_rxtx(parent, video_exporter, requested_compression)
{
                sage_receiver = requested_receiver;
                int ret = initialize_video_display("sage",
                                sage_opts, 0, &m_sage_tx_device);
                if(ret != 0) {
                        throw string("Unable to initialize SAGE TX.");
                }
                pthread_create(&m_thread_id, NULL, (void * (*)(void *)) display_run,
                                &m_sage_tx_device);
                memset(&m_saved_video_desc, 0, sizeof(m_saved_video_desc));
}

void sage_video_rxtx::send_frame(struct video_frame *tx_frame)
{
        if(!video_desc_eq(m_saved_video_desc,
                                video_desc_from_frame(tx_frame))) {
                display_reconfigure(m_sage_tx_device,
                                video_desc_from_frame(tx_frame));
                m_saved_video_desc = video_desc_from_frame(tx_frame);
        }
        struct video_frame *frame =
                display_get_frame(m_sage_tx_device);
        memcpy(frame->tiles[0].data, tx_frame->tiles[0].data,
                        tx_frame->tiles[0].data_len);
        display_put_frame(m_sage_tx_device, frame, PUTF_NONBLOCK);

        VIDEO_FRAME_DISPOSE(tx_frame);
}

sage_video_rxtx::~sage_video_rxtx()
{
        // poisoned pill to exit thread
        display_put_frame(m_sage_tx_device, NULL, PUTF_NONBLOCK);
        pthread_join(m_thread_id, NULL);

        display_done(m_sage_tx_device);
}

