/*
 * CANAL interface DLL for RUSOKU technologies for TouCAN, TouCAN Marine, TouCAN Duo USB to CAN bus converter
 *
 * Copyright (C) 2020 Gediminas Simanskis (gediminas@rusoku.com)
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published
 * by the Free Software Foundation; version 3.0 of the License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.
 *
 */

#include "stdafx.h"


bool MyStrTrim(LPWSTR str) {

	UINT16	index = 0;
	size_t	len = 0;
	UINT16  src_index = 0;
	UINT16	dst_index = 0;
	bool	hr = FALSE;
	
	len = wcslen(str);

	for (src_index = 0; src_index <= len; src_index++)
	{
		if (!iswspace(*(str + src_index)))
		{
			*(str + dst_index) = *(str + src_index);
			dst_index++;
			hr = TRUE;
		}
	}
	return hr;
}
