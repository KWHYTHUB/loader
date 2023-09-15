/**
 * Adapted from https://gist.github.com/altalk23/29b97969e9f0624f783b673f6c1cd279
 */

#include <utils/addresser.hpp>
#include <cstdlib>
#include <stddef.h>
#include <Macros.hpp>

#define SAPPHIRE_ADDRESSER_NEST1(macro, begin)           \
macro(SAPPHIRE_CONCAT(begin, 0)),                        \
macro(SAPPHIRE_CONCAT(begin, 1)),                        \
macro(SAPPHIRE_CONCAT(begin, 2)),                        \
macro(SAPPHIRE_CONCAT(begin, 3)),                        \
macro(SAPPHIRE_CONCAT(begin, 4)),                        \
macro(SAPPHIRE_CONCAT(begin, 5)),                        \
macro(SAPPHIRE_CONCAT(begin, 6)),                        \
macro(SAPPHIRE_CONCAT(begin, 7)),                        \
macro(SAPPHIRE_CONCAT(begin, 8)),                        \
macro(SAPPHIRE_CONCAT(begin, 9)),                        \
macro(SAPPHIRE_CONCAT(begin, a)),                        \
macro(SAPPHIRE_CONCAT(begin, b)),                        \
macro(SAPPHIRE_CONCAT(begin, c)),                        \
macro(SAPPHIRE_CONCAT(begin, d)),                        \
macro(SAPPHIRE_CONCAT(begin, e)),                        \
macro(SAPPHIRE_CONCAT(begin, f))  

#define SAPPHIRE_ADDRESSER_NEST2(macro, begin)           \
SAPPHIRE_ADDRESSER_NEST1(macro, SAPPHIRE_CONCAT(begin, 0)), \
SAPPHIRE_ADDRESSER_NEST1(macro, SAPPHIRE_CONCAT(begin, 1)), \
SAPPHIRE_ADDRESSER_NEST1(macro, SAPPHIRE_CONCAT(begin, 2)), \
SAPPHIRE_ADDRESSER_NEST1(macro, SAPPHIRE_CONCAT(begin, 3)), \
SAPPHIRE_ADDRESSER_NEST1(macro, SAPPHIRE_CONCAT(begin, 4)), \
SAPPHIRE_ADDRESSER_NEST1(macro, SAPPHIRE_CONCAT(begin, 5)), \
SAPPHIRE_ADDRESSER_NEST1(macro, SAPPHIRE_CONCAT(begin, 6)), \
SAPPHIRE_ADDRESSER_NEST1(macro, SAPPHIRE_CONCAT(begin, 7)), \
SAPPHIRE_ADDRESSER_NEST1(macro, SAPPHIRE_CONCAT(begin, 8)), \
SAPPHIRE_ADDRESSER_NEST1(macro, SAPPHIRE_CONCAT(begin, 9)), \
SAPPHIRE_ADDRESSER_NEST1(macro, SAPPHIRE_CONCAT(begin, a)), \
SAPPHIRE_ADDRESSER_NEST1(macro, SAPPHIRE_CONCAT(begin, b)), \
SAPPHIRE_ADDRESSER_NEST1(macro, SAPPHIRE_CONCAT(begin, c)), \
SAPPHIRE_ADDRESSER_NEST1(macro, SAPPHIRE_CONCAT(begin, d)), \
SAPPHIRE_ADDRESSER_NEST1(macro, SAPPHIRE_CONCAT(begin, e)), \
SAPPHIRE_ADDRESSER_NEST1(macro, SAPPHIRE_CONCAT(begin, f))  


#define SAPPHIRE_ADDRESSER_THUNK0_DEFINE(hex) (intptr_t)&f<hex * sizeof(intptr_t)>
#define SAPPHIRE_ADDRESSER_TABLE_DEFINE(hex) (intptr_t)&ThunkTable::table

#define SAPPHIRE_ADDRESSER_THUNK0_SET() SAPPHIRE_ADDRESSER_NEST2(SAPPHIRE_ADDRESSER_THUNK0_DEFINE, 0x)
#define SAPPHIRE_ADDRESSER_TABLE_SET() SAPPHIRE_ADDRESSER_NEST2(SAPPHIRE_ADDRESSER_TABLE_DEFINE, 0x)

using namespace sapphire::addresser;

namespace {
	template<ptrdiff_t index>
	SAPPHIRE_HIDDEN ptrdiff_t f() {
		return index;
	}

	using thunk0_table_t = intptr_t[0x100];
	using table_table_t = intptr_t[0x100];

	struct SAPPHIRE_HIDDEN ThunkTable {
		static inline thunk0_table_t table = {
			SAPPHIRE_ADDRESSER_THUNK0_SET()
		};
	};

	class SAPPHIRE_HIDDEN TableTable {
		friend class sapphire::addresser::Addresser;

		static inline table_table_t table = {
			SAPPHIRE_ADDRESSER_TABLE_SET()
		};
	};
}

Addresser::MultipleInheritance* Addresser::instance() {
	return reinterpret_cast<Addresser::MultipleInheritance*>(&TableTable::table);
}
