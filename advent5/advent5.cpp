#include "advent5.h"
#include "../advent/advent_utils.h"

#define ENABLE_DAY5DBG 1
#ifdef NDEBUG
#define DAY5DBG 0
#else
#define DAY5DBG ENABLE_DAY5DBG
#endif

#if DAY5DBG
	#include <iostream>
#endif

namespace
{
#if DAY5DBG
	std::ostream & log = std::cout;
#else
	struct {	template <typename T> auto& operator<<(const T&) const noexcept { return *this; } } log;
#endif
}

#include "small_vector.h"
#include "range_contains.h"
#include "parse_utils.h"
#include "to_value.h"
#include "istream_block_iterator.h"
#include "string_line_iterator.h"
#include "int_range.h"
#include "sorted_vector.h"

#include <algorithm>
#include <vector>
#include <array>
#include <numeric>

namespace
{
	using ID = std::uint64_t;

	using IDRange = utils::int_range<ID>;

	using IDList = utils::small_vector<IDRange,24>;

	enum class IDType
	{
		seed,
		soil,
		fertilizer,
		water,
		light,
		temperature,
		humidity,
		location,
		NUM
	};

	constexpr std::size_t to_idx(IDType id) noexcept
	{
		return static_cast<std::size_t>(id);
	}

	IDType to_id_type(std::string_view str)
	{
		// Use the 4th digit(idx=3) to identify each thing because that digit is unique.
		AdventCheck(str.size() >= 4);
#define ADD_CASE(ref_char , test_type)					\
			case ref_char:								\
			static_assert(#test_type[3] == ref_char);	\
			AdventCheck(str == #test_type);				\
			return IDType::test_type
		switch(str[3])
		{
		ADD_CASE('d',seed);
		ADD_CASE('l',soil);
		ADD_CASE('t',fertilizer);
		ADD_CASE('e',water);
		ADD_CASE('h',light);
		ADD_CASE('p',temperature);
		ADD_CASE('i',humidity);
		ADD_CASE('a',location);
		default:
			break;
		}
		AdventUnreachable();
		return IDType::NUM;
#undef ADD_CASE
	}

	class Region
	{
	private:
		ID low = 0;
		ID high = 0;
		std::int64_t diff = 0;
	public:
		Region(ID input_start, ID output_start, ID range_len)
			: low{input_start} , high{input_start + range_len}, diff{static_cast<int64_t>(output_start) - static_cast<int64_t>(input_start)}
			{
				AdventCheck(input_start < static_cast<ID>(std::numeric_limits<int64_t>::max()));
				AdventCheck(output_start < static_cast<ID>(std::numeric_limits<int64_t>::max()));
				AdventCheck(high > low);
			}
		bool is_in_range(ID in) const noexcept { return utils::range_contains_exc(in,low,high); }
		struct ConvertResult
		{
			ID result_id = 0;
			int64_t distance_to_end_of_range;
		};
		ConvertResult convert_id(ID in) const noexcept
		{
			if(is_in_range(in))
			{
				AdventCheck(high > in);
				const ID distance = high - in;
				AdventCheck(distance < static_cast<ID>(std::numeric_limits<int64_t>::max()));
				return ConvertResult{in + diff , static_cast<int64_t>(distance)};
			}
			else
			{
				return ConvertResult{in,static_cast<int64_t>(low) - static_cast<int64_t>(in)};
			}
		}
		auto operator<=>(const Region& other) const noexcept = default;
		auto operator<=>(ID id) const noexcept { return low <=> id; }
	};

	Region to_region(std::string_view line)
	{
		AdventCheck(!line.empty());
		using namespace utils;
		auto [output_str, input_str, len_str] = get_string_elements(line,' ',0,1,2);
		const ID input = to_value<ID>(input_str);
		const ID output = to_value<ID>(output_str);
		const ID len = to_value<ID>(len_str);
		return Region{input,output,len};
	}

	class Transform
	{
		IDType output_type = IDType::NUM;
		utils::sorted_vector<Region> transforms;
	public:
		explicit Transform(IDType outputs) : output_type{outputs}
		{
			AdventCheck(outputs != IDType::NUM);
			transforms.reserve(50);
		}
		Transform() {}
		IDType get_output_type() const noexcept { return output_type; }
		void add_region(const Region& region)
		{
			if(transforms.size() == transforms.capacity())
			{
				log << "WARNING: Transform has had to expand capacity. Please up the reserved size to avoid unnecessary allocations.";
			}
			transforms.push_back(region);
		}

		struct TransformResult
		{
			ID new_id = 0;
			IDType new_type = IDType::NUM;
			std::optional<int64_t> distance_to_next_range;
		};

		TransformResult transform_id(ID in) const noexcept
		{
			auto find_fn = [in](const Region& r)
				{
					return r.is_in_range(in) || r >= in;
				};
			const auto relevant_transform = stdr::find_if(transforms, find_fn);
			if(relevant_transform == end(transforms))
			{
				return TransformResult{ in , get_output_type() , std::nullopt };
			}

			const Region::ConvertResult result = relevant_transform->convert_id(in);
			AdventCheck(result.distance_to_end_of_range >= 0);
			return TransformResult{ result.result_id , get_output_type(), result.distance_to_end_of_range };
		}
	};

	struct ParseTransformResult
	{
		Transform transform;
		IDType input_id;
	};

	ParseTransformResult to_transform(std::string_view str)
	{
		AdventCheck(!str.empty());
		const auto [header , data] = utils::split_string_at_first(str,'\n');
		const auto [in_type,out_type] = [header]()
		{
			std::string_view in_to_out = utils::remove_specific_suffix(header," map:");
			const auto [in_str,out_str] = utils::get_string_elements(in_to_out,'-',0,2);
			AdventCheck(utils::get_string_element(in_to_out,'-',1) == "to");
			return std::pair{to_id_type(in_str),to_id_type(out_str)};
		}();
		Transform result{out_type};
		for(std::string_view line : utils::string_line_range{ data })
		{
			const Region new_region = to_region(line);
			result.add_region(new_region);
		}
		return { result , in_type };
	}

	class TransformSet
	{
		std::array<Transform,to_idx(IDType::NUM)> transforms;
	public:
		void add_transform(ParseTransformResult new_transform)
		{
			const std::size_t new_idx = to_idx(new_transform.input_id);
			AdventCheck(new_idx < to_idx(IDType::NUM));
			AdventCheck(transforms[new_idx].get_output_type() == IDType::NUM);
			transforms[to_idx(new_transform.input_id)] = std::move(new_transform.transform);
		}

		struct ConvertResult
		{
			ID id = 0;
			std::optional<int64_t> distance_to_end_of_range;
		};

		ConvertResult convert_id(ID in_id, IDType from, IDType to) const noexcept
		{
			AdventCheck(from != IDType::NUM);
			AdventCheck(to != IDType::NUM);
			const Transform& transform = transforms[to_idx(from)];
			const auto [new_id , new_type , distance_to_end] = transform.transform_id(in_id);
			if(new_type == to)
			{
				return ConvertResult{ new_id , distance_to_end };
			}
			ConvertResult result = convert_id(new_id,new_type,to);
			auto combine_distances = [](std::optional<int64_t> left, std::optional<int64_t> right)
			{
				if(!left.has_value()) return right;
				if(!right.has_value()) return left;
				return *left < *right ? left : right;
			};
			result.distance_to_end_of_range = combine_distances(distance_to_end, result.distance_to_end_of_range);
			return result;
		}
	};

	TransformSet to_transform_set(std::istream& input)
	{
		TransformSet result;
		for(std::string_view block : utils::istream_block_range{input})
		{
			AdventCheck(!block.empty());
			ParseTransformResult parse_result = to_transform(block);
			result.add_transform(std::move(parse_result));
		}
		return result;
	}

	template <AdventDay Day>
	IDList get_seeds(std::istream& input)
	{
		std::string header_storage;
		std::getline(input,header_storage);
		std::string_view header = header_storage;
		header = utils::remove_specific_prefix(header, "seeds: ");

		IDList result;
		while(!header.empty())
		{
			auto [next_id_str , rest1] = utils::split_string_at_first(header, ' ');
			header = rest1;
			const ID next_id = utils::to_value<ID>(next_id_str);
			if constexpr (Day == AdventDay::one)
			{
				result.emplace_back(next_id, next_id + 1);
			}
			if constexpr (Day == AdventDay::two)
			{
				auto [length_str , rest2] = utils::split_string_at_first(header, ' ');
				header = rest2;
				const ID length = utils::to_value<ID>(length_str);
				result.emplace_back(next_id, next_id + length);
			}
		}
		std::getline(input,header_storage);
		AdventCheck(header_storage.empty());
		return result;
	}

	int64_t solve_generic(const IDList& ids, std::istream& input)
	{
		const TransformSet transform_set = to_transform_set(input);
		auto transform_fn = [&transform_set](const IDRange& id_range)
		{
			auto it = begin(id_range);
			ID best_id = std::numeric_limits<ID>::max();
			while(it < end(id_range))
			{
				const ID id = *it;
				const auto [out_id , distance_to_end] = transform_set.convert_id(id,IDType::seed , IDType::location);
				best_id = std::min(best_id, out_id);
				if(distance_to_end.has_value()) it += *distance_to_end;
				else it = end(id_range);
			}
			return best_id;
		};
		auto reduce_fn = [](ID a, ID b) { return std::min(a,b); };
		const ID result = std::transform_reduce(begin(ids),end(ids),std::numeric_limits<ID>::max(),reduce_fn,transform_fn);
		AdventCheck(result < static_cast<ID>(std::numeric_limits<int64_t>::max()));
		return static_cast<int64_t>(result);
	}

	int64_t solve_p1(std::istream& input)
	{
		const IDList seeds = get_seeds<AdventDay::one>(input);
		return solve_generic(seeds,input);
	}
}

namespace
{
	int64_t solve_p2(std::istream& input)
	{
		const IDList seeds = get_seeds<AdventDay::two>(input);
		return solve_generic(seeds, input);
	}
}

ResultType advent5_internal::p1_a(std::istream& input, uint64_t seed)
{
	std::string dummy;
	std::getline(input,dummy);
	std::getline(input,dummy);
	const IDRange range{ seed , seed + 1 };
	const IDList ids{ range };
	return solve_generic(ids, input);
}

ResultType testcase_five_p1_b(std::istream& input)
{
	return solve_p1(input);
}

ResultType testcase_five_p2_b(std::istream& input)
{
	return solve_p2(input);
}

ResultType advent_five_p1()
{
	auto input = advent::open_puzzle_input(5);
	return solve_p1(input);
}

ResultType advent_five_p2()
{
	auto input = advent::open_puzzle_input(5);
	return solve_p2(input);
}

#undef DAY5DBG
#undef ENABLE_DAY5DBG