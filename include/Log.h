#ifndef LOG_H
#define LOG_H

#include <iostream>
#include <fstream>

#define LOG() nes::Log::get().get_stream()

namespace nes {

	class Log {
	public:
		void set_log_stream(std::ostream& _ost) {
			log_stream = &_ost;
		}
		std::ostream& get_stream() {
			return *log_stream;
		}
		static Log& get() {
			static Log log;
			return log;
		}
	private:
		std::ostream* log_stream;
	};

	class TeeBuf : public std::streambuf {
	public:
		TeeBuf(std::streambuf* _stb1, std::streambuf* _stb2)
			: streambuf1{ _stb1 }, streambuf2{ _stb2 }
		{}
	private:
		virtual int overflow(int c) {
			if (c == EOF) {
				return !EOF;
			}
			else {
				int const r1 = streambuf1->sputc(c);
				int const r2 = streambuf2->sputc(c);
				return r1 == EOF || r2 == EOF ? EOF : c;
			}
		}
		virtual int sync() {
			int const r1 = streambuf1->pubsync();
			int const r2 = streambuf2->pubsync();
			return r1 == 0 && r2 == 0 ? 0 : -1;
		}
	private:
		std::streambuf* streambuf1;
		std::streambuf* streambuf2;
	};

	class TeeStream : public std::ostream {
	public:
		TeeStream(std::ostream& ost1, std::ostream& ost2)
			: std::ostream{ &teeBuf }, teeBuf(ost1.rdbuf(), ost2.rdbuf())
		{}
	private:
		TeeBuf teeBuf;
	};

};

#endif