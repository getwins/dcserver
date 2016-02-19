
#ifndef _FTD_CHP_HPP_
#define _FTD_CHP_HPP_
//  (C) Copyright huiping.chen 2015.

#include <boost/static_assert.hpp>

#if defined _MSC_VER
	#define _FTD_ALIGN_ 
	#pragma pack(push,1)
#elif defined __GNU__
	#define _FTD_ALIGN_ __attribute__ ((packed))
#else
	#define _FTD_ALIGN_ 
#endif

struct ftd_header_st
{
	char type;
	char extend_length;
	short int ftdc_length;
}_FTD_ALIGN_;
BOOST_STATIC_ASSERT(sizeof(ftd_header_st) == 4);

#define FTD_TYPE_NONE 0x00
#define FTD_TYPE_FTDC 0x01
#define FTD_TYPE_COMPRESSED 0x02

struct ftd_extend_header_st
{
	char tags[127];
}_FTD_ALIGN_;
BOOST_STATIC_ASSERT(sizeof(ftd_extend_header_st) == 127);

#define FTD_EXTEND_TAG_NONE 0x00
#define FTD_EXTEND_TAG_DATETIME 0x01
#define FTD_EXTEND_TAG_COMPRESSED_METHOD 0x02
#define FTD_EXTEND_TAG_SESSION_STATE 0x03
#define FTD_EXTEND_TAG_KEEPALIVE 0x04
#define FTD_EXTEND_TAG_TRADEDATE 0x05
#define FTD_EXTEND_TAG_TARGET 0x06

#define FTD_EXTEND_COMPRESSED_METHOD_NONE 0
#define FTD_EXTEND_COMPRESSED_METHOD_LZ 1

struct ftdc_header_st
{
	unsigned char version;
	unsigned int tid;
	char chain;
	unsigned short seris;
	unsigned int seqno;
	unsigned short field_count;
	unsigned short ftdc_content_length;
}_FTD_ALIGN_;
BOOST_STATIC_ASSERT(sizeof(ftdc_header_st) == 16);

#define FTDC_CHAIN_SINGLE 'S'
#define FTDC_CHAIN_FIRST 'F'
#define FTDC_CHAIN_MIDDLE 'C'
#define FTDC_CHAIN_LAST 'L'

#define FTDC_MAX_LENGTH 4096
#define FTDC_CONTENT_MAX_LENGTH (FTDC_MAX_LENGTH - sizeof(ftdc_header_st))

typedef char ftdc_buff_t[FTDC_MAX_LENGTH];
typedef char ftdc_content_buff_t[FTDC_CONTENT_MAX_LENGTH];


struct ftdc_field_st
{
	int fid;
	short field_length;
	char dataitem[1];
};

inline int ftdc_field_entity_size(const ftdc_field_st &field) { return sizeof(field) - 1 + field.field_length; }

//class ftd_basetype
//{
//	ftd_basetype(int length, int precision, std::vector<std::string> enumvals);
//};
//
//class ftd_base_char_type : public ftd_basetype
//{
//	ftd_base_char_type() : ftd_basetype(1, 0, )
//};
//
//class ftd_base_float_type : public ftd_basetype{};
//class ftd_base_int_type : public ftd_basetype{};
//class ftd_base_number_type : public ftd_basetype{};
//class ftd_base_string_type : public ftd_basetype{};
//class ftd_base_word_type : public ftd_basetype{};

typedef enum {
	ftd_char_type,
	ftd_float_type,
	ftd_int_type,
	ftd_number_type,
	ftd_string_type,
	ftd_word_type,
	ftd_unknown_type
} ftd_item_type_t;

#if defined _MSC_VER
	#pragma pack(pop)
#endif

#undef __PACKED__

#endif