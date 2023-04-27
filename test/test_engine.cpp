#include "gtest/gtest.h"

#include "../src/engine.h"

#include <type_traits>

TEST(engine, create_division_by_2_error){
	ASSERT_NO_THROW(
		tobor::v1_0::division_by_2_error error;
	);
}

TEST(engine, create_blocked_center_error){
	ASSERT_NO_THROW(
		tobor::v1_0::blocked_center_error error;
	);
}

TEST(engine, create_wall_type_std_constructor) {
	ASSERT_FALSE(
		std::is_default_constructible<tobor::v1_0::wall_type>::value // convert into static assert! ###
	);
}

TEST(engine, create_wall_type_bool_conversion) {
	ASSERT_NO_THROW(
		auto t_wall = tobor::v1_0::wall_type(true);
		auto f_wall = tobor::v1_0::wall_type(false);
	);
}

TEST(engine, create_world) {
	ASSERT_NO_THROW(
		tobor::v1_0::tobor_world world;
	);
}

TEST(engine, create_universal_field_id) {
	ASSERT_NO_THROW(
		auto field_id = tobor::v1_0::universal_field_id();
	);
}

TEST(engine, universal_field_id_consistency) {
	// ### check that id conversion has no inconsistencies...
	ASSERT_NO_THROW(
		auto field_id = 0;
	);
}


