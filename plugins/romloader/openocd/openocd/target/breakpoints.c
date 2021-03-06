/***************************************************************************
 *   Copyright (C) 2005 by Dominic Rath                                    *
 *   Dominic.Rath@gmx.de                                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>

#include "binarybuffer.h"
#include "target.h"
#include "log.h"
#include "types.h"

#include "breakpoints.h"

char *breakpoint_type_strings[] =
{
	"hardware",
	"software"
};

char *watchpoint_rw_strings[] =
{
	"read",
	"write",
	"access"
};

int breakpoint_add(target_t *target, u32 address, u32 length, enum breakpoint_type type)
{
	breakpoint_t *breakpoint = target->breakpoints;
	breakpoint_t **breakpoint_p = &target->breakpoints;
	int retval;
		
	while (breakpoint)
	{
		if (breakpoint->address == address)
			return ERROR_OK;
		breakpoint_p = &breakpoint->next;
		breakpoint = breakpoint->next;
	}
		
	(*breakpoint_p) = malloc(sizeof(breakpoint_t));
	(*breakpoint_p)->address = address;
	(*breakpoint_p)->length = length;
	(*breakpoint_p)->type = type;
	(*breakpoint_p)->set = 0;
	(*breakpoint_p)->orig_instr = malloc(length);
	(*breakpoint_p)->next = NULL;
	
	if ((retval = target->type->add_breakpoint(target, *breakpoint_p)) != ERROR_OK)
	{
		switch (retval)
		{
			case ERROR_TARGET_RESOURCE_NOT_AVAILABLE:
				INFO("can't add %s breakpoint, resource not available", breakpoint_type_strings[(*breakpoint_p)->type]);
				free((*breakpoint_p)->orig_instr);
				free(*breakpoint_p);
				*breakpoint_p = NULL;
				return retval;
				break;
			case ERROR_TARGET_NOT_HALTED:
				INFO("can't add breakpoint while target is running");
				free((*breakpoint_p)->orig_instr);
				free(*breakpoint_p);
				*breakpoint_p = NULL;
				return retval;
				break;
			default:
				ERROR("unknown error");
				exit(-1);
				break;
		}
	}
	
	DEBUG("added %s breakpoint at 0x%8.8x of length 0x%8.8x", 
		breakpoint_type_strings[(*breakpoint_p)->type],
		(*breakpoint_p)->address, (*breakpoint_p)->length);
	
	return ERROR_OK;
}

int breakpoint_remove(target_t *target, u32 address)
{
	breakpoint_t *breakpoint = target->breakpoints;
	breakpoint_t **breakpoint_p = &target->breakpoints;
	int retval;
	
	while (breakpoint)
	{
		if (breakpoint->address == address)
			break;
		breakpoint_p = &breakpoint->next;
		breakpoint = breakpoint->next;
	}
	
	if (breakpoint)
	{
		if ((retval = target->type->remove_breakpoint(target, breakpoint)) != ERROR_OK)
		{
			switch (retval)
			{
				case ERROR_TARGET_NOT_HALTED:
					INFO("can't remove breakpoint while target is running");
					return retval;
					break;
				default:
					ERROR("unknown error");
					exit(-1);
					break;
			}
		}
		(*breakpoint_p) = breakpoint->next;
		free(breakpoint->orig_instr);
		free(breakpoint);
	}
	else
	{
		ERROR("no breakpoint at address 0x%8.8x found", address);
	}
	
	return ERROR_OK;
}

void breakpoint_remove_all(target_t *target)
{
	breakpoint_t *ptBp, *ptLastBp;

	ptBp = target->breakpoints;
	while (ptBp != NULL)
	{
		ptLastBp = ptBp;
		ptBp = ptBp->next;
		free(ptLastBp->orig_instr);
		free(ptLastBp);
	}
	target->breakpoints = NULL;
}

breakpoint_t* breakpoint_find(target_t *target, u32 address)
{
	breakpoint_t *breakpoint = target->breakpoints;
	
	while (breakpoint)
	{
		if (breakpoint->address == address)
			return breakpoint;
		breakpoint = breakpoint->next;
	}
	
	return NULL;
}

int watchpoint_add(target_t *target, u32 address, u32 length, enum watchpoint_rw rw, u32 value, u32 mask)
{
	watchpoint_t *watchpoint = target->watchpoints;
	watchpoint_t **watchpoint_p = &target->watchpoints;
	int retval;
		
	while (watchpoint)
	{
		if (watchpoint->address == address)
			return ERROR_OK;
		watchpoint_p = &watchpoint->next;
		watchpoint = watchpoint->next;
	}
	
	(*watchpoint_p) = malloc(sizeof(watchpoint_t));
	(*watchpoint_p)->address = address;
	(*watchpoint_p)->length = length;
	(*watchpoint_p)->value = value;
	(*watchpoint_p)->mask = mask;
	(*watchpoint_p)->rw = rw;
	(*watchpoint_p)->set = 0;
	(*watchpoint_p)->next = NULL;
		
	if ((retval = target->type->add_watchpoint(target, *watchpoint_p)) != ERROR_OK)
	{
		switch (retval)
		{
			case ERROR_TARGET_RESOURCE_NOT_AVAILABLE:
				INFO("can't add %s watchpoint, resource not available", watchpoint_rw_strings[(*watchpoint_p)->rw]);
				free (*watchpoint_p);
				*watchpoint_p = NULL;
				return retval;
				break;
			case ERROR_TARGET_NOT_HALTED:
				INFO("can't add watchpoint while target is running");
				free (*watchpoint_p);
				*watchpoint_p = NULL;
				return retval;
				break;
			default:
				ERROR("unknown error");
				exit(-1);
				break;
		}
	}
	
	DEBUG("added %s watchpoint at 0x%8.8x of length 0x%8.8x",
		watchpoint_rw_strings[(*watchpoint_p)->rw],
		(*watchpoint_p)->address, (*watchpoint_p)->length);
	
	return ERROR_OK;
}

int watchpoint_remove(target_t *target, u32 address)
{
	watchpoint_t *watchpoint = target->watchpoints;
	watchpoint_t **watchpoint_p = &target->watchpoints;
	int retval;
	
	while (watchpoint)
	{
		if (watchpoint->address == address)
			break;
		watchpoint_p = &watchpoint->next;
		watchpoint = watchpoint->next;
	}
	
	if (watchpoint)
	{
		if ((retval = target->type->remove_watchpoint(target, watchpoint)) != ERROR_OK)
		{
			switch (retval)
			{
				case ERROR_TARGET_NOT_HALTED:
					INFO("can't remove watchpoint while target is running");
					return retval;
					break;
				default:
					ERROR("unknown error");
					exit(-1);
					break;
			}
		}
		(*watchpoint_p) = watchpoint->next;
		free(watchpoint);
	}
	else
	{
		ERROR("no watchpoint at address 0x%8.8x found", address);
	}
	
	return ERROR_OK;
}

void watchpoint_remove_all(target_t *target)
{
	watchpoint_t *ptWp, *ptLastWp;

	ptWp = target->watchpoints;
	while (ptWp != NULL)
	{
		ptLastWp = ptWp;
		ptWp = ptWp->next;
		free(ptLastWp);
	}
	target->watchpoints = NULL;
}
