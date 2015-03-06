#include "gmock/gmock.h"

#define private public

#include "bdebug.h"
#include "loc.h"
#include "span_lookup_table.h"

#include <string>
using std::string;
using testing::ElementsAre;

/*
 * These tests are for the SpanLookupTable, maps Locs to SpanEntries
 * containing info for converting from Locs to U64 strips.
 */

class SpanLookupTableFixture : public testing::Test {
public:
	SpanLookupTableFixture() {
		buildSpanTable(19);
	}
};


TEST_F(SpanLookupTableFixture, ZeroZeroE) {
	Loc l(0,0);
	SpanEntry *item = &spanLookupTable[E_DIR][l._value];
	EXPECT_EQ(0, item->_minIndex);
	EXPECT_EQ(4, item->_maxIndex);
	EXPECT_EQ(0, item->_locIndex);
	EXPECT_EQ(l, item->_baseLoc);
	EXPECT_EQ(Loc(1,0), item->_offsetPerIndex);
}

TEST_F(SpanLookupTableFixture, OneZeroE) {
	Loc l(1,0);
	SpanEntry *item = &spanLookupTable[E_DIR][l._value];
	EXPECT_EQ(0, item->_minIndex);
	EXPECT_EQ(5, item->_maxIndex);
	EXPECT_EQ(1, item->_locIndex);
	EXPECT_EQ(Loc(0,0), item->_baseLoc);
	EXPECT_EQ(Loc(1,0), item->_offsetPerIndex);
}

TEST_F(SpanLookupTableFixture, ZeroOneE) {
	Loc l(0,1);
	SpanEntry *item = &spanLookupTable[E_DIR][l._value];
	EXPECT_EQ(0, item->_minIndex);
	EXPECT_EQ(4, item->_maxIndex);
	EXPECT_EQ(0, item->_locIndex);
	EXPECT_EQ(Loc(0,1), item->_baseLoc);
	EXPECT_EQ(Loc(1,0), item->_offsetPerIndex);
}

TEST_F(SpanLookupTableFixture, MaxMaxE) {
	Loc l(18,18);
	SpanEntry *item = &spanLookupTable[E_DIR][l._value];
	EXPECT_EQ(14, item->_minIndex);
	EXPECT_EQ(18, item->_maxIndex);
	EXPECT_EQ(18, item->_locIndex);
	EXPECT_EQ(Loc(0,18), item->_baseLoc);
	EXPECT_EQ(Loc(1,0), item->_offsetPerIndex);
}

TEST_F(SpanLookupTableFixture, MidMidE) {
	Loc l(9,9);
	SpanEntry *item = &spanLookupTable[E_DIR][l._value];
	EXPECT_EQ(5, item->_minIndex);
	EXPECT_EQ(13, item->_maxIndex);
	EXPECT_EQ(9, item->_locIndex);
	EXPECT_EQ(Loc(0,9), item->_baseLoc);
	EXPECT_EQ(Loc(1,0), item->_offsetPerIndex);
}
