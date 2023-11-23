/*
 * Copyright (C) 2006-2010 - Frictional Games
 *
 * This file is part of HPL1 Engine.
 *
 * HPL1 Engine is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * HPL1 Engine is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with HPL1 Engine.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "system/Date.h"

namespace hpl {

	cDate cDate::FromGMTIme(struct tm* apClock)
	{
		cDate date;

		date.seconds = apClock->tm_sec;
		date.minutes = apClock->tm_min;
		date.hours = apClock->tm_hour;
		date.month_day = apClock->tm_mday;
		date.month = apClock->tm_mon;
		date.year = 1900 + apClock->tm_year;
		date.week_day = apClock->tm_wday;
		date.year_day = apClock->tm_yday;

		return date;
	}

	tString cDate::ToString() const
	{
		char buff[256];

		snprintf(buff, 256, "%d/%d-%d %d:%d:%d", month_day, month, 1900 + year, hours, minutes, seconds);

		return buff;
	}

	bool cDate::operator==(const cDate &aDate) const
	{
		if(	seconds == aDate.seconds &&
			minutes == aDate.minutes &&
			hours == aDate.hours &&
			month_day == aDate.month_day &&
			month == aDate.month &&
			year == aDate.year)
			{
				return true;
			}

		return false;
	}

	bool cDate::operator>(const cDate &aDate) const
	{
		if(year > aDate.year) return true;
		else if(year < aDate.year) return false;

		if(month > aDate.month) return true;
		else if(month < aDate.month) return false;

		if(month_day > aDate.month_day) return true;
		else if(month_day < aDate.month_day) return false;

		if(hours > aDate.hours) return true;
		else if(hours < aDate.hours) return false;

		if(minutes > aDate.minutes) return true;
		else if(minutes < aDate.minutes) return false;

		if(seconds > aDate.seconds) return true;
		else if(seconds < aDate.seconds) return false;

		return false;
	}

	bool cDate::operator<(const cDate &aDate) const
	{
		if(year < aDate.year) return true;
		else if(year > aDate.year) return false;

		if(month < aDate.month) return true;
		else if(month > aDate.month) return false;

		if(month_day < aDate.month_day) return true;
		else if(month_day > aDate.month_day) return false;

		if(hours < aDate.hours) return true;
		else if(hours > aDate.hours) return false;

		if(minutes < aDate.minutes) return true;
		else if(minutes > aDate.minutes) return false;

		if(seconds < aDate.seconds) return true;
		else if(seconds > aDate.seconds) return false;

		return false;
	}

	//-----------------------------------------------------------------------

	cDate GetDate()
	{
		time_t lTime;
		time(&lTime);

		struct tm* pClock;
		pClock = localtime(&lTime);

		return cDate::FromGMTIme(pClock);
	}

}
