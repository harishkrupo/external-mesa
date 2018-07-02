/*
 * Copyright © 2008 Intel Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 *
 * Authors:
 *    Harish Krupo <harish.krupo.kps@intel.com>
 *
 */

/**
 * \file
 * \brief Support for EGL_KHR_partial_update
 */

#include <GL/gl.h>
#include <GL/internal/dri_interface.h>

#include "brw_context.h"
#include "main/imports.h"
#include "main/scissor.h"
#include "main/enable.h"

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

static GLboolean
brw_set_damage_region(__DRIcontext *ctx, GLuint n_rects, GLint *rects) {
   struct brw_context *brw = ctx->driverPrivate;

   if (n_rects == 0) {
      _mesa_set_enable(&brw->ctx, GL_SCISSOR_TEST, GL_FALSE);
      return GL_TRUE;
   }

   GLuint final_rect[4] = {rects[0],
                           rects[1],
                           rects[0] + rects[2],
                           rects[1] + rects[3]};

   for (GLint num_drects = 1; num_drects < n_rects; num_drects++) {
      GLint i = num_drects * 4;

      final_rect[0] = MIN(final_rect[0], rects[i]);
      final_rect[1] = MIN(final_rect[1], rects[i + 1]);
      final_rect[2] = MAX(final_rect[2], (rects[i] + rects[i + 2]));
      final_rect[3] = MAX(final_rect[3], (rects[i + 1] + rects[i + 3]));
   }

   //enable scissor
   _mesa_set_enable(&brw->ctx, GL_SCISSOR_TEST, GL_TRUE);

   _mesa_Scissor(final_rect[0],
                 final_rect[1],
                 final_rect[2] - final_rect[0],
                 final_rect[3] - final_rect[1]);

   return GL_TRUE;
}

const __DRI2damageExtension intelDamageExtension = {
   .base = { __DRI2_DAMAGE, 1 },

   .set_damage_region = brw_set_damage_region
};
