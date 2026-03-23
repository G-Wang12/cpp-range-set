# cpp-range-set
some c++ exercise

## To run
run 'g++ main.cpp' to get an executable that does the following:
take max_value_count and max_point_distance as cli arguments and produce "output.txt" in format "start_point:end_point:value_count"

run the executable with 3 optional flags: --gen, --no-points, --no-ranges.

by default, we will feed the points and ranges from points.txt and ranges.txt to our range_set object before writing the calculated value to output.txt

--gen makes the program randomly generate all points and ranges in points.txt and ranges.txt

--no-points indicates we don't want to add the points from points.txt into our range_set instance
--no-ranges indicates we don't want to add the rangess from range.txt into range_set instance

By default we add all points from points.txt and all ranges from range.txt into range_set instance

## Assumptions
- For vector<range_t> get_ranges(), we assumed that we can merge ranges even if they don't overlap as long as the new ranges fit the limits of value_count and point_distance. we implemented it so it greedily merges all ranges until the given limits of value_count and point_distance is reached.

- We assumed max_value_count and max_point_distance will be unsigned 32 bit integers.

- we assume it is possible for us to receive start and end points where start is bigger than end. To solve that edge case, we just swap start and end points when it is being added to range.