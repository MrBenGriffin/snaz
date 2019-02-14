//
// Created by Ben on 2019-02-14.
//

#ifndef MACROTEXT_DATE_H
#define MACROTEXT_DATE_H

#include <ctime>
#include <string>
#include <stack>

namespace Support {
	class Date {

	private:
//		static pid_t pid;
		void normalise(struct tm*&);

	protected:
		time_t tt;
		struct tm* local;
		struct tm* utc;

	public:

		Date();

		explicit Date(time_t ttime);
		~Date() = default;

		int	tm_sec;		/* seconds after the minute [0-60] */
		int	tm_min;		/* minutes after the hour [0-59] */
		int	tm_hour;	/* hours since midnight [0-23] */

		void setYearOffset(int);
		void setSecOffset(int);
		void setMinOffset(int);
		void setHourOffset(int);

		void getUTCDateStr(const std::string&, std::string &);
		void getLocalDateStr(const std::string&, std::string &);
		void getDateStr(struct tm*, const std::string&,std::string&);
		void getDateStr(struct tm*, std::string &cont);
		void getNowDateStr(const std::string &format, std::string &cont);
		void getNow(std::string& cont);
		const time_t getUTC() const;

	};
}


#endif //MACROTEXT_DATE_H
