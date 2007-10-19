/*
 * Copyright 2006, 2007 by Brian Dominy <brian@oddchange.com>
 *
 * This file is part of FreeWPC.
 *
 * FreeWPC is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * FreeWPC is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with FreeWPC; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <freewpc.h>

__local__ U8 clock_round_hits;

__local__ U8 clock_default_hits;


void clock_millions_hit_deff (void)
{
	generic_deff ("CLOCK MILLIONS", "5,000,000");
}


void clock_default_hit_deff (void)
{
	dmd_alloc_low_clean ();
	sprintf ("CLOCK HIT %d", clock_default_hits);
	font_render_string_center (&font_fixed6, 64, 10, sprintf_buffer);
	font_render_string_center (&font_mono5, 64, 21, "50,000");
	dmd_show_low ();
	task_sleep_sec (2);
	deff_exit ();
}


CALLSET_ENTRY (clocktarget, sw_clock_target)
{
	if (!lamp_flash_test (LM_CLOCK_MILLIONS))
	{
		score (SC_50K);
		sound_send (SND_NO_CREDITS);
	}
	else
	{
		sound_send (SND_CLOCK_BELL);
		leff_start (LEFF_CLOCK_TARGET);
		deff_start (DEFF_CLOCK_MILLIONS_HIT);
		score (SC_5M);
	
		if (clock_round_hits <= 4)
		{
			clock_round_hits++;
		}
	}
}


CALLSET_ENTRY(clocktarget, start_player)
{
	clock_default_hits = 0;
	clock_round_hits = 0;
}


CALLSET_ENTRY (clocktarget, start_ball)
{
	lamp_tristate_off (LM_CLOCK_MILLIONS);
}


CALLSET_ENTRY (clocktarget, door_start_clock_millions)
{
	/* TODO : need to start a round timer here */
	lamp_tristate_flash (LM_CLOCK_MILLIONS);
}

