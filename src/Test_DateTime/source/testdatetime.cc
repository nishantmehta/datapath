//
//  Copyright 2012 Alin Dobra and Christopher Jermaine
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <vector>
#include "date.h"
#include "time.h"
#include "timestamp.h"
#include "datecalc.h"

using namespace std;

static Int32 DaysPerMonth[] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

void dummy(...)
{
}

#ifdef _DEBUG
#define localassert(x) assert(x)
#else
#define localassert(x) (testassert(x, __FILE__, __LINE__));
#endif

void testassert(bool cond, char * file, int line) {
	if (!cond) {
		fprintf(stderr, "Assert failed : %s, %d\n", file, line);
		exit(1);
	}
}

void testdate();
void testtime();
void testtimestamp();

int main()
{
	fprintf(stderr, "Starting tests. This may take several minutes\n");

	clock_t start = clock();

	fprintf(stderr, "Testing timestamp\n");
	testtimestamp();
	fprintf(stderr, "Testing time\n");
	testtime();
	fprintf(stderr, "Testing date\n");
	testdate();

	CTimestamp a = CTimestamp(2001, 5, 25, 8, 35, 52, 0);
	CDate *c = &a;
	localassert(c->GetDay() == 25);
	CTime *b = &a;
	localassert(b->GetHour() == 8);

	printf("Test time = %f\n", double(clock()-start)/CLOCKS_PER_SEC);

	return 0;
}

void testdate() {

	// Basic known values first
	CDate dt(2000, 1, 1);
	localassert(dt.GetDayOfWeek() == strSaturday);
	localassert(dt.GetWeekOfYear() == 52);
	dt += 1;
	localassert(dt.GetDayOfWeek() == strSunday);
	localassert(dt.GetWeekOfYear() == 52);
	dt += 1;
	localassert(dt.GetDayOfWeek() == strMonday);
	localassert(dt.GetWeekOfYear() == 1);

	dt = CDate(2001, 1, 1);
	localassert(dt.GetDayOfWeek() == strMonday);
	localassert(dt.GetWeekOfYear() == 1);
	dt += 5;
	localassert(dt.GetDayOfWeek() == strSaturday);
	localassert(dt.GetWeekOfYear() == 1);
	dt += 1;
	localassert(dt.GetDayOfWeek() == strSunday);
	localassert(dt.GetWeekOfYear() == 1);
	
	// Looping tests
	int month = 1, day = 1, year = 1500, yday = 1;
	CDate date(year, month, day);

	for (int i = 0; i < 365000; i++) {

		// To Julian day from d,m,y
		int JD, JD2;
		{
			int a = (14-month)/12;
			int y = year+4800-a;
			int m = month + 12*a - 3;

			JD = day + (153*m+2)/5 + y*365 + y/4 - y/100 + y/400 - 32045;
		}

		// To Julian day from d,yd
		{
			int y2 = year+4799;
			JD2 = yday + y2*365 + y2/4 - y2/100 + y2/400 - 31739;
		}

		// To d,m,y from Julian day
		int jday, jmonth, jyear, jyday, WeekNumber, WeekDay, WeekYear;
		{
			int a = JD + 32044;
			int b = (4*a+3)/146097;
			int c = a - (b*146097)/4;

			int d = (4*c+3)/1461;
			int e = c - (1461*d)/4;
			int m = (5*e+2)/153;

			jday   = e - (153*m+2)/5 + 1;
			jmonth = m + 3 - 12*(m/10);
			jyear  = b*100 + d - 4800 + m/10;

			int y2 = jyear+4799;
			jyday = JD - (y2*365 + y2/4 - y2/100 + y2/400 - 31739);

			int d4 = (JD+31741 - (JD % 7)) % 146097 % 36524 % 1461;
			int L  = d4/1460;
			int d1 = ((d4-L) % 365) + L;
			WeekNumber = d1/7+1;

			WeekDay = JD%7 + 1;

			WeekYear = jyear + ((WeekNumber == 1) & (jmonth == 12)) - ((WeekNumber > 51) & (jmonth == 1));
		}

		// Stop the optimiser throwing out values for timing tests
		dummy(day, month, year, jday, jmonth, jyear, jyday, WeekNumber, WeekDay, WeekYear, JD, JD2);

		// http://www.faqs.org/faqs/calendars/faq/part2/

		localassert(jday == day);
		localassert(jmonth == month);
		localassert(jyear == year);
		localassert(jyday == yday);

		Int32 tmp;

		localassert(JD == (tmp = date.GetJulianDay()));
		localassert(day == (tmp = date.GetDay()));
		localassert(JD == (tmp = date.GetJulianDay()));
		localassert(month == (tmp = date.GetMonth()));
		localassert(JD == (tmp = date.GetJulianDay()));
		localassert(year == (tmp = date.GetYear()));
		localassert(JD == (tmp = date.GetJulianDay()));
		localassert(yday == (tmp = date.GetDayOfYear()));
		localassert(JD == (tmp = date.GetJulianDay()));
		localassert(WeekNumber == (tmp = date.GetWeekOfYear()));
		localassert(JD == (tmp = date.GetJulianDay()));
		localassert(WeekDay == (tmp = date.GetDayOfWeek()));
		localassert(JD == (tmp = date.GetJulianDay()));
		localassert(WeekYear == (tmp = date.GetYearForWeekOfYear()));
		localassert(JD == (tmp = date.GetJulianDay()));

		localassert(day == (tmp = date.GetDay()));
		localassert(date == CDate(year, month, day));
		localassert(day == (tmp = date.GetDay()));
		localassert(date - 1 < CDate(year, month, day));
		localassert(day == (tmp = date.GetDay()));
		localassert(date < CDate(year, month, day) + 1);
		localassert(day == (tmp = date.GetDay()));
		localassert(date - CDate(year, month, day) == 0);
		localassert(day == (tmp = date.GetDay()));
		localassert((date + 1) - CDate(year, month, day) == 1);
		localassert(day == (tmp = date.GetDay()));
		localassert((date - 1) - CDate(year, month, day) == -1);
		localassert(day == (tmp = date.GetDay()));

		CDate d2(date);
		CDate d3;
		d3 = date+1;
		localassert((d3 - 1) == d2);

		// Move to next day
		date += 1;

		day++;
		yday++;
		if (day > CDateCalc::DaysInMonth(month, year % 4 == 0 && (year % 100 != 0 || year % 400 == 0))) {
			day = 1;
			month++;
			if (month > 12) {
				month = 1;
				yday = 1;
				year++;
			}
		}
	}

}

void testtime() {

	// Known value tests
	CTime t1;
	localassert(t1.GetMilliSecondsPastMidnight() == 0);
	t1 += 86399999;
	localassert(t1.GetMilliSecondsPastMidnight() == 86399999);
	t1 += 1;
	localassert(t1.GetMilliSecondsPastMidnight() == 0);
	t1 += -1;
	localassert(t1.GetMilliSecondsPastMidnight() == 86399999);
	t1 += -86399999;
	localassert(t1.GetMilliSecondsPastMidnight() == 0);

	// Looping tests
	int hour = 0, minute = 1, second = 0, millisecond = 0, MSPM = 60000;
	CTime tme(hour, minute, second, millisecond);

	// Count up 47 milliseconds at a time
	for (int i = 0; i < 2*86400*1000; i+=47) {
		Int32 tmp;

		localassert(MSPM == (tmp = tme.GetMilliSecondsPastMidnight()));
		localassert(millisecond == (tmp = tme.GetMilliSecond()));
		localassert(MSPM == (tmp = tme.GetMilliSecondsPastMidnight()));
		localassert(second == (tmp = tme.GetSecond()));
		localassert(MSPM == (tmp = tme.GetMilliSecondsPastMidnight()));
		localassert(minute == (tmp = tme.GetMinute()));
		localassert(MSPM == (tmp = tme.GetMilliSecondsPastMidnight()));
		localassert(hour == (tmp = tme.GetHour()));
		localassert(MSPM == (tmp = tme.GetMilliSecondsPastMidnight()));

		localassert(tme == CTime(hour, minute, second, millisecond));
		if (!(tme == 0)) localassert(tme - 1 < CTime(hour, minute, second, millisecond));
		if (!(tme == 86400000-1)) localassert(tme < CTime(hour, minute, second, millisecond) + 1);
		localassert(tme - CTime(hour, minute, second, millisecond) == 0);
		localassert((tme + 1) - CTime(hour, minute, second, millisecond) == 1);
		localassert((tme - 1) - CTime(hour, minute, second, millisecond) == 86400000-1);

		CTime t2(tme);
		CTime t3;
		t3 = tme+1;
		localassert((t3 - 1) == t2);

		// Move to next millisecond
		tme += 47;

		MSPM += 47;
		millisecond += 47;
		if (millisecond >= 1000) {
			millisecond -= 1000;
			second++;
			if (second >= 60) {
				second -= 60;
				minute++;
				if (minute >= 60) {
					minute -= 60;
					hour++;
					if (hour >= 24) {
						hour -= 24;
						MSPM -= (24*60*60*1000);
					}
				}
			}
		}

	}

}

void testtimestamp()
{
	CDate dt(2200, 1, 1);
	Int64 EJD = Int64(dt.GetJulianDay()) * 86400000;

	CTimestamp ts1(dt);
	Int32 Year = 2200, Month = 1, Day = 1;
	Int32 Hour = 0, Minute = 0, Second = 0, MilliSecond = 0;

	// Start from year 2200 and count down in 4359673 milliseconds at a time
	int i;
	for (i = 0; i < 1000000; i++) {
		localassert(ts1.GetExtendedJulianDay() == EJD - Int64(i)*4359673);
		localassert(ts1.GetYear() == Year);
		localassert(ts1.GetMonth() == Month);
		localassert(ts1.GetDay() == Day);
		localassert(ts1.GetHour() == Hour);
		localassert(ts1.GetMinute() == Minute);
		localassert(ts1.GetSecond() == Second);
		localassert(ts1.GetMilliSecond() == MilliSecond);

		ts1 -= 4359673;

		MilliSecond -= 4359673;
		while (MilliSecond < 0) {
			MilliSecond += 1000;
			Second--;
			if (Second < 0) {
				Second += 60;
				Minute--;
				if (Minute < 0) {
					Minute += 60;
					Hour--;
					if (Hour < 0) {
						Hour += 24;
						
						Day--;
						if (Day == 0) {
							Month--;
							if (Month == 0) {
								Year--;
								// printf("%i\n", Year);
								Month = 12;
							}
							Day = DaysPerMonth[Month];
							if ((Year % 4 == 0 && (Year % 100 != 0 || Year % 400 == 0)) && Month == 2)
								Day++;
						}	
					}
				}
			}
		}

	}

	// Count back up 2337071 milliseconds at a time
	int j;
	for (j = 0; j < 1000000; j++) {
		localassert(ts1.GetExtendedJulianDay() == EJD - Int64(i)*4359673 + Int64(j)*2337071);
		localassert(ts1.GetYear() == Year);
		localassert(ts1.GetMonth() == Month);
		localassert(ts1.GetDay() == Day);
		localassert(ts1.GetHour() == Hour);
		localassert(ts1.GetMinute() == Minute);
		localassert(ts1.GetSecond() == Second);

		ts1 += 2337071;

		MilliSecond += 2337071;
		while (MilliSecond >= 1000) {
			MilliSecond -= 1000;
			Second++;
			if (Second >= 60) {
				Second -= 60;
				Minute++;
				if (Minute >= 60) {
					Minute -= 60;
					Hour++;
					if (Hour >= 24) {
						Hour -= 24;

						Day++;
						int IsLeapMonth = ((Year % 4 == 0 && (Year % 100 != 0 || Year % 400 == 0)) && Month == 2);
						if (Day == DaysPerMonth[Month]+1+IsLeapMonth) {
							Month++;
							if (Month == 13) {
								Year++;
								// printf("%i\n", Year);
								Month = 1;
							}
							Day = 1;
						}
					}
				}
			}
		}
	}

}

