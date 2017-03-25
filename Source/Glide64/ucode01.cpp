/*
* Glide64 - Glide video plugin for Nintendo 64 emulators.
* Copyright (c) 2002  Dave2001
* Copyright (c) 2003-2009  Sergey 'Gonetz' Lipski
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

//****************************************************************
//
// Glide64 - Glide Plugin for Nintendo 64 emulators
// Project started on December 29th, 2001
//
// Authors:
// Dave2001, original author, founded the project in 2001, left it in 2002
// Gugaman, joined the project in 2002, left it in 2002
// Sergey 'Gonetz' Lipski, joined the project in 2002, main author since fall of 2002
// Hiroshi 'KoolSmoky' Morii, joined the project in 2007
//
//****************************************************************
//
// To modify Glide64:
// * Write your name and (optional)email, commented by your work, so I know who did it, and so that you can find which parts you modified when it comes time to send it to me.
// * Do NOT send me the whole project or file that you modified.  Take out your modified code sections, and tell me where to put them.  If people sent the whole thing, I would have many different versions, but no idea how to combine them all.
//
//****************************************************************
#include <Glide64/rdp.h>
#include <Glide64/Gfx_1.3.h>
#include <Glide64/trace.h>
#include <Glide64/ucode.h>
#include "ucode00.h"

//
// vertex - loads vertices
//

void uc1_vertex()
{
    int v0 = (rdp.cmd0 >> 17) & 0x7F;     // Current vertex
    int n = (rdp.cmd0 >> 10) & 0x3F;    // Number to copy
    rsp_vertex(v0, n);
}

//
// tri1 - renders a triangle
//

void uc1_tri1()
{
    if (rdp.skip_drawing)
    {
        WriteTrace(TraceRDP, TraceDebug, "uc1:tri1. skipped");
        return;
    }
    WriteTrace(TraceRDP, TraceDebug, "uc1:tri1 #%d - %d, %d, %d - %08lx - %08lx", rdp.tri_n,
        ((rdp.cmd1 >> 17) & 0x7F),
        ((rdp.cmd1 >> 9) & 0x7F),
        ((rdp.cmd1 >> 1) & 0x7F), rdp.cmd0, rdp.cmd1);

    VERTEX *vtx[3] = {
        &rdp.vtx((rdp.cmd1 >> 17) & 0x7F),
        &rdp.vtx((rdp.cmd1 >> 9) & 0x7F),
        &rdp.vtx((rdp.cmd1 >> 1) & 0x7F)
    };

    rsp_tri1(vtx);
}

void uc1_tri2()
{
    if (rdp.skip_drawing)
    {
        WriteTrace(TraceRDP, TraceDebug, "uc1:tri2. skipped");
        return;
    }
    WriteTrace(TraceRDP, TraceDebug, "uc1:tri2");

    WriteTrace(TraceRDP, TraceDebug, " #%d, #%d - %d, %d, %d - %d, %d, %d", rdp.tri_n, rdp.tri_n + 1,
        ((rdp.cmd0 >> 17) & 0x7F),
        ((rdp.cmd0 >> 9) & 0x7F),
        ((rdp.cmd0 >> 1) & 0x7F),
        ((rdp.cmd1 >> 17) & 0x7F),
        ((rdp.cmd1 >> 9) & 0x7F),
        ((rdp.cmd1 >> 1) & 0x7F));

    VERTEX *vtx[6] = {
        &rdp.vtx((rdp.cmd0 >> 17) & 0x7F),
        &rdp.vtx((rdp.cmd0 >> 9) & 0x7F),
        &rdp.vtx((rdp.cmd0 >> 1) & 0x7F),
        &rdp.vtx((rdp.cmd1 >> 17) & 0x7F),
        &rdp.vtx((rdp.cmd1 >> 9) & 0x7F),
        &rdp.vtx((rdp.cmd1 >> 1) & 0x7F)
    };

    rsp_tri2(vtx);
}

void uc1_line3d()
{
    if (!g_settings->force_quad3d() && ((rdp.cmd1 & 0xFF000000) == 0) && ((rdp.cmd0 & 0x00FFFFFF) == 0))
    {
        uint16_t width = (uint16_t)(rdp.cmd1 & 0xFF) + 3;

        WriteTrace(TraceRDP, TraceDebug, "uc1:line3d width: %d #%d, #%d - %d, %d", width, rdp.tri_n, rdp.tri_n + 1,
            (rdp.cmd1 >> 17) & 0x7F,
            (rdp.cmd1 >> 9) & 0x7F);

        VERTEX *vtx[3] = {
            &rdp.vtx((rdp.cmd1 >> 17) & 0x7F),
            &rdp.vtx((rdp.cmd1 >> 9) & 0x7F),
            &rdp.vtx((rdp.cmd1 >> 9) & 0x7F)
        };
        uint32_t cull_mode = (rdp.flags & CULLMASK) >> CULLSHIFT;
        rdp.flags |= CULLMASK;
        rdp.update |= UPDATE_CULL_MODE;
        rsp_tri1(vtx, width);
        rdp.flags ^= CULLMASK;
        rdp.flags |= cull_mode << CULLSHIFT;
        rdp.update |= UPDATE_CULL_MODE;
    }
    else
    {
        WriteTrace(TraceRDP, TraceDebug, "uc1:quad3d #%d, #%d", rdp.tri_n, rdp.tri_n + 1);

        VERTEX *vtx[6] = {
            &rdp.vtx((rdp.cmd1 >> 25) & 0x7F),
            &rdp.vtx((rdp.cmd1 >> 17) & 0x7F),
            &rdp.vtx((rdp.cmd1 >> 9) & 0x7F),
            &rdp.vtx((rdp.cmd1 >> 1) & 0x7F),
            &rdp.vtx((rdp.cmd1 >> 25) & 0x7F),
            &rdp.vtx((rdp.cmd1 >> 9) & 0x7F)
        };

        rsp_tri2(vtx);
    }
}

uint32_t branch_dl = 0;

void uc1_rdphalf_1()
{
    WriteTrace(TraceRDP, TraceDebug, "uc1:rdphalf_1");
    branch_dl = rdp.cmd1;
    rdphalf_1();
}

void uc1_branch_z()
{
    uint32_t addr = segoffset(branch_dl);
    WriteTrace(TraceRDP, TraceDebug, "uc1:branch_less_z, addr: %08lx", addr);
    uint32_t vtx = (rdp.cmd0 & 0xFFF) >> 1;
    if (fabs(rdp.vtx(vtx).z) <= (rdp.cmd1/*&0xFFFF*/))
    {
        rdp.pc[rdp.pc_i] = addr;
    }
}
