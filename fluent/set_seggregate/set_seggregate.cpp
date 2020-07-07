#include <iostream>
#include <map>
#include <vector>

template<class LeftRange, class RightRange,
	class OutputItLeft, class OutputItBoth, class OutputItRight, class Compare>
void set_segregate(LeftRange const& leftRange, RightRange const& rightRange,
		OutputItLeft leftOnly, OutputItBoth both, OutputItRight rightOnly,
		Compare comp)
{
	auto itLeft = leftRange.begin();
	auto itRight = rightRange.begin();
	while (itLeft != leftRange.end())
	{
		if (itRight == rightRange.end())
		{
			std::copy(itLeft, leftRange.end(), leftOnly);
			return;
		}

		if (comp(*itLeft, *itRight))
		{
			*leftOnly++ = *itLeft++;
		}
		else
		{
			if (!comp(*itRight, *itLeft))
			{
				*both++ = *itLeft++;
				++itRight;
			}
			else
			{
				*rightOnly++ = *itRight++;
			}
		}
	}
	std::copy(itRight, rightRange.end(), rightOnly);
}

template<typename T>
std::ostream& operator<< (std::ostream& out, const std::vector<T>& container)
{
	bool bFirst=true;
	out << '{';
	for(const auto& item: container)
	{
		if (bFirst == false)
			out << ", ";
		else
			bFirst = false;
		out << item;
	}
	out << '}';
	return out;
}

int main(int argc, char* const argv[])
{
	std::vector<int> left = {1, 2, 3, 5, 7, 9};
	std::vector<int> right = {3, 4, 5, 6, 7};

	std::vector<int> leftOnly;
	std::vector<int> both;
	std::vector<int> rightOnly;

	set_segregate(left, right, std::back_inserter(leftOnly), std::back_inserter(both), std::back_inserter(rightOnly), std::less<>{});

	std::cout << "left  = " << left << std::endl;
	std::cout << "right = " << right << std::endl;
	std::cout << "----" << std::endl;
	std::cout << "leftOnly = " << leftOnly << std::endl;
	std::cout << "both     = " << both << std::endl;
	std::cout << "rightOnly= " << rightOnly << std::endl;
	// leftOnly contains {1, 2, 9};
	// both contains {3, 5, 7};
	// rightOnly contains {4, 6};
}

