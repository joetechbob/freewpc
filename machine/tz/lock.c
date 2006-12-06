/*
 * Copyright 2006 by Brian Dominy <brian@oddchange.com>
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


CALLSET_ENTRY (lock, dev_lock_enter)
{
	if (lamp_test (LM_PANEL_FAST_LOCK))
		score (SC_250K);
	else
		score (SC_50K);
	// task_sleep_sec (1);
	sound_send (SND_ROBOT_FLICKS_GUN);
}


CALLSET_ENTRY (lock, dev_lock_kick_attempt)
{
	sound_send (SND_LOCK_KICKOUT);
	switch_can_follow (lock_exit, right_loop, TIME_3S);
}

