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

#ifndef _RTSOL_H
#define _RTSOL_H

/* Code for doing real-time solenoids.
 * These are handled directly by polling the switches and
 * then driving the appropriate solenoids.
 */

extern inline void rt_sol_enable (const U8 sol)
{
	sol_timers[sol] = 0xFF;
}


extern inline void rt_sol_disable (const U8 sol)
{
	sol_timers[sol] = 0;
}


/* Each rt solenoid is governed by a rt_sol_state object,
 * which tracks the current status of that solenoid.
 * The state is an 8-bit value divided into two parts:
 * 1-bit for whether the solenoid is in its on/off phase,
 * and 7-bits for the timer coutdown/countup.
 */
extern inline void rt_solenoid_update (
	S8 * state,
	const U8 solno,
	const U8 swno,
	const U8 sol_on_irqs,
	const U8 sol_off_irqs )
{
	if (!in_live_game)
	{
		/* Solenoid is never pulsed except during a game */
		rt_sol_disable (solno);
	}
	else if (*state == 0)
	{
		/* Solenoid is idle - normal case */
		/* Only here are allowed to poll the switch */
		if (rt_switch_poll (swno))
		{
			/* Yes, the switch is active, so the solenoid can
			 * be scheduled to activate now */
			*state = sol_on_irqs;
		}
	}
	else if (*state < 0)
	{
		/* Solenoid is in its off-phase */
		rt_sol_disable (solno);
		(*state)++;
	}
	else if (*state > 0)
	{
		/* Solenoid is in its on-phase */
		rt_sol_enable (solno);
		(*state)--;
		if (*state == 0)
		{
			*state = -sol_off_irqs;
		}
	}
}


/**
 * rt_solenoid_update is the main interface for realtime drivers like
 * slingshots and jet bumpers.  It will poll the switch of the device
 * to see if it's being actuated, and then begin a short phase pulse
 * to the solenoid, followed by a minimum off-phase.  It then goes back
 * to polling the switch.
 *
 * The switch & solenoid are parameterized so that this macro may be
 * used in multiple places (at the cost of duplicating the code).
 * The on/off phase time and the switch/solenoid polarity can also be
 * given (in case the switch is an opto, or the drive goes from on to off?)
 */

#endif /* _RTSOL_H */

/* vim: set ts=3: */
