
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

/**
 * \file
 * \brief Coin switch handlers
 */

#include <freewpc.h>

/** The number of credits */
__nvram__ U8 credit_count;

/** The number of additional units purchased, less than the value
 * of one credit */
__nvram__ U8 unit_count;


void credits_render (void)
{
#ifdef FREE_ONLY
	sprintf ("FREE ONLY");
#else
	if (price_config.free_play)
		sprintf ("FREE PLAY");
	else
	{
		if (unit_count != 0)
		{
			if (credit_count == 0)
			{
				sprintf ("%d/%d CREDIT", unit_count, price_config.units_per_credit);
			}
			else
			{
				sprintf ("%d %d/%d CREDITS",
					credit_count, unit_count, price_config.units_per_credit);
			}
		}
		else
		{
			if (credit_count == 1)
				sprintf ("%d CREDIT", credit_count);
			else
				sprintf ("%d CREDITS", credit_count);
		}
	}
#endif
}


void credits_draw (void)
{
	dmd_alloc_low_clean ();
	dmd_alloc_high ();

	credits_render ();
	font_render_string_center (&font_fixed6, 64, 9, sprintf_buffer);
	dmd_copy_low_to_high ();

	if (!has_credits_p ())
	{
		sprintf ("INSERT COINS");
	}
	else
	{
		sprintf ("PRESS START");
	}
	font_render_string_center (&font_fixed6, 64, 22, sprintf_buffer);

	deff_swap_low_high (in_live_game ? 13 : 21, 2 * TIME_100MS);
}


void credits_deff (void)
{
	credits_draw ();
	deff_delay_and_exit (TIME_100MS * 10);
}


void lamp_start_update (void)
{
	/* TODO : start button is flashing very early after reset, before
	 * a game can actually be started. */
	if (has_credits_p ())
	{
		if (!in_game)
			lamp_flash_on (MACHINE_START_LAMP);
		else
		{
			lamp_flash_off (MACHINE_START_LAMP);
			lamp_on (MACHINE_START_LAMP);
		}
	}
	else
	{
		lamp_off (MACHINE_START_LAMP);
		lamp_flash_off (MACHINE_START_LAMP);
	}
}


void add_credit (void)
{
	if (credit_count >= price_config.max_credits)
		return;

#ifndef FREE_ONLY
	wpc_nvram_get ();
	credit_count++;
	wpc_nvram_put ();

	lamp_start_update ();
#endif

#ifdef MACHINE_ADD_CREDIT_SOUND
	sound_send (MACHINE_ADD_CREDIT_SOUND);
#endif

	leff_start (LEFF_FLASH_ALL);
	deff_restart (DEFF_CREDITS);
}


bool has_credits_p (void)
{
#ifndef FREE_ONLY
	if (price_config.free_play)
		return (TRUE);
	else
		return (credit_count > 0);
#else
	return (TRUE);
#endif
}


void remove_credit (void)
{
#ifndef FREE_ONLY
	if (credit_count > 0)
	{
		wpc_nvram_get ();
		credit_count--;
		wpc_nvram_put ();

		lamp_start_update ();
	}
#endif
}


void add_units (int n)
{
	if (credit_count >= price_config.max_credits)
		return;

	wpc_nvram_get ();
	unit_count += n;
	wpc_nvram_put ();

	if (unit_count >= price_config.units_per_credit)
	{
		while (unit_count >= price_config.units_per_credit)
		{
			wpc_nvram_get ();
			unit_count -= price_config.units_per_credit;
			wpc_nvram_put ();

			add_credit ();
			audit_increment (&system_audits.paid_credits);
		}
	}
	else
	{
#ifdef MACHINE_ADD_COIN_SOUND
		sound_send (MACHINE_ADD_COIN_SOUND);
#endif
		deff_restart (DEFF_CREDITS);
	}
}


void coin_deff (void) __taskentry__
{
	register int8_t z = 4;
	while (--z > 0)
	{
		dmd_invert_page (dmd_low_buffer);
		task_sleep (TIME_100MS * 2);
	}
	deff_exit ();
}

static void do_coin (uint8_t slot)
{
	add_units (price_config.slot_values[slot]);
	audit_increment (&system_audits.coins_added[slot]);
}

void sw_left_coin_handler (void)
{
	do_coin (0);
}

void sw_center_coin_handler (void)
{
	do_coin (1);
}

void sw_right_coin_handler (void)
{
	do_coin (2);
}

void sw_fourth_coin_handler (void)
{
	do_coin (3);
}


DECLARE_SWITCH_DRIVER (sw_left_coin)
{
	.fn = sw_left_coin_handler,
};


DECLARE_SWITCH_DRIVER (sw_center_coin)
{
	.fn = sw_center_coin_handler,
};


DECLARE_SWITCH_DRIVER (sw_right_coin)
{
	.fn = sw_right_coin_handler,
};


DECLARE_SWITCH_DRIVER (sw_fourth_coin)
{
	.fn = sw_fourth_coin_handler,
};


void credits_clear (void)
{
	wpc_nvram_get ();
	credit_count = 0;
	unit_count = 0;
	wpc_nvram_put ();
}


void coin_init (void)
{
	lamp_start_update ();
}

