////////////////////////////////////////////////////////////////////////////////
// maxweight.hh
//
// Compute the set of cargo items that maximizes the weight loaded in trucks, 
// within a given overall volume, with the greedy method or exhaustive optimization.
//
///////////////////////////////////////////////////////////////////////////////


#pragma once


#include <cassert>
#include <cmath>
#include <fstream>
#include <iostream>
#include <memory>
#include <queue>
#include <sstream>
#include <string>
#include <vector>


// One cargo item available for loading.
class CargoItem
{
	//
	public:
		
		//
		CargoItem
		(
			const std::string& description,
			double volume_cubic,
			double weight_TEU
		)
			:
			_description(description),
			_volume_cubic(volume_cubic),
			_weight_TEU(weight_TEU)
		{
			assert(!description.empty());
			assert(volume_cubic > 0);
		}
		
		//
		const std::string& description() const { return _description; }
		double volume() const { return _volume_cubic; }
		double weight() const { return _weight_TEU; }
	
	//
	private:
		
		// Human-readable description of the good, e.g. "yellow soybean". Must be non-empty.
		std::string _description;
		
		// Volume of cargo item, in cubioc meters; Must be positive
		double _volume_cubic;
		
		// Weight of the cargo item in TEUs; most be non-negative.
		double _weight_TEU;
};


// Alias for a vector of shared pointers to CargoItem objects.
typedef std::vector<std::shared_ptr<CargoItem>> CargoVector;


// Load all the valid cargo items from the CSV database
// Cargo items that are missing fields, or have invalid values, are skipped.
// Returns nullptr on I/O error.
std::unique_ptr<CargoVector> load_cargo_database(const std::string& path)
{
	std::unique_ptr<CargoVector> failure(nullptr);
	
	std::ifstream f(path);
	if (!f)
	{
		std::cout << "Failed to load cargo database; Cannot open file: " << path << std::endl;
		return failure;
	}
	
	std::unique_ptr<CargoVector> result(new CargoVector);
	
	size_t line_number = 0;
	for (std::string line; std::getline(f, line); )
	{
		line_number++;
		
		// First line is a header row
		if ( line_number == 1 )
		{
			continue;
		}
		
		std::vector<std::string> fields;
		std::stringstream ss(line);
		
		for (std::string field; std::getline(ss, field, '^'); )
		{
			fields.push_back(field);
		}
		
		if (fields.size() != 3)
		{
			std::cout
				<< "Failed to load cargo database: Invalid field count at line " << line_number << "; Want 3 but got " << fields.size() << std::endl
				<< "Line: " << line << std::endl
				;
			return failure;
		}
		
		std::string
			descr_field = fields[0],
			volume_cubic_field = fields[1],
			weight_TEU_field = fields[2]
			;
		
		auto parse_dbl = [](const std::string& field, double& output)
		{
			std::stringstream ss(field);
			if ( ! ss )
			{
				return false;
			}
			
			ss >> output;
			
			return true;
		};
		
		std::string description(descr_field);
		double volume_cubic, weight_TEU;
		if (
			parse_dbl(volume_cubic_field, volume_cubic)
			&& parse_dbl(weight_TEU_field, weight_TEU)
		)
		{
			result->push_back(
				std::shared_ptr<CargoItem>(
					new CargoItem(
						description,
						volume_cubic,
						weight_TEU
					)
				)
			);
		}
	}

	f.close();
	
	return result;
}


// Convenience function to compute the total volume and weight in a CargoVector.
// Provide the CargoVector as the first argument
// The next two arguments will return the volume and weight back to the caller.
void sum_cargo_vector
(
	const CargoVector& goods,
	double& total_volume,
	double& total_weight
)
{
	total_volume = total_weight = 0;
	for (auto& item : goods)
	{
		total_volume += item->volume();
		total_weight += item->weight();
	}
}


// Convenience function to print out each CargoItem in a CargoVector,
// followed by the total volume and weight in it.
void print_cargo_vector(const CargoVector& goods)
{
	std::cout << "*** cargo Vector ***" << std::endl;
	
	if ( goods.size() == 0 )
	{
		std::cout << "[empty cargo list]" << std::endl;
	}
	else
	{
		for (auto& item : goods)
		{
			std::cout
				<< "Ye olde " << item->description()
				<< " ==> "
				<< "volume of " << item->volume() << " cubic meters"
				<< "; weight in TEUs = " << item->weight()
				<< std::endl
				;
		}
		
		double total_volume, total_weight;
		sum_cargo_vector(goods, total_volume, total_weight);
		std::cout
			<< "> Grand total volume: " << total_volume << " cubic meters" << std::endl
			<< "> Grand total weight: " << total_weight << " TEUs " << std::endl
			;
	}
}


// Filter the vector source, i.e. create and return a new CargoVector containing 
// the subset of the cargo items in source that match given criteria.
// This is intended to:
//	1) filter out cargo with zero or negative weight that are irrelevant to our optimization
//	2) limit the size of inputs to the exhaustive optimization algorithm since it will probably be slow.
//
// Each cargo item that is included must have at minimum min_weight and at most max_weight.
//	(i.e., each included cargo item's weight must be between min_weight and max_weight (inclusive).
//
// In addition, the the vector includes only the first total_size cargo items that match these criteria.
std::unique_ptr<CargoVector> filter_cargo_vector
(
	const CargoVector& source,
	double min_weight,
	double max_weight,
	size_t total_size
)
{
// TODO: implement this function, then delete the return statement below
std::unique_ptr<CargoVector> cargo(new CargoVector);

//For each item in the vector, if the item's weight is in between the min_weight and max_weight, insert to vector
for(size_t i = 0; i < source.size() && cargo->size() < total_size; i++) {
	if(source[i]->weight() >= min_weight && source[i]->weight() <= max_weight) {
		cargo->push_back(source[i]);
	}
}

return cargo;
}


// Compute the optimal set of cargo items with a greedy algorithm.
// Specifically, among the cargo items that fit within a total_volume space,
// choose the goods whose weight-per-volume is greatest.
// Repeat until no more cargo items can be chosen, either because we've run out of cargo items,
// or run out of space.
std::unique_ptr<CargoVector> greedy_max_weight
(
	const CargoVector& goods,
	double total_volume
)
{
// TODO: implement this function, then delete the return statement below
std::unique_ptr<CargoVector> todo(new CargoVector(goods));
std::unique_ptr<CargoVector> result(new CargoVector);
size_t result_volume = 0;

while(!todo->empty()) {
	double ratio = 0;
	int index = 0;
	int volume = 0;
	for(size_t i = 0; i < todo->size(); i++) { 
		if(ratio < todo->at(i)->weight()/todo->at(i)->volume()) {
			ratio = todo->at(i)->weight()/todo->at(i)->volume();
			index = i;
			volume = todo->at(i)->volume();
		}
	}

	if((result_volume + volume) <= total_volume) {
		result->push_back(todo->at(index));
		result_volume += volume;
	}

	todo->erase(todo->begin() + index);
}

return result;
}


// Compute the optimal set of cargo items with a exhaustive search algorithm.
// Specifically, among all subsets of cargo items, return the subset whose volume 
// in cubic meters fits within the total_volume budget and whose total weight is greatest.
// To avoid overflow, the size of the cargo items vector must be less than 64.
std::unique_ptr<CargoVector> exhaustive_max_weight
(
	const CargoVector& goods,
	double total_volume
)
{
// TODO: implement this function, then delete the return statement below
std::unique_ptr<CargoVector> best(new CargoVector);
int n = goods.size();
double candidateTotalVolume = 0;
double candidateTotalWeight = 0;
double bestWeight;

for (int i = 0; i <= (pow(2,n)-1); i++) {
	std::unique_ptr<CargoVector> candidate(new CargoVector);
	for (int j = 0; j <= (n-1); j++) {
		if(((i >> j) & 1) == 1) {
			candidate->push_back(goods[j]);
		}
	}
	sum_cargo_vector(*candidate, candidateTotalVolume, candidateTotalWeight);

	if(candidateTotalVolume <= total_volume) {
		if(best == NULL || candidateTotalWeight > bestWeight) {
			*best = *candidate;
			bestWeight = candidateTotalWeight;
		}
	}
}

return best;
}
