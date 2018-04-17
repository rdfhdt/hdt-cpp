#include <tuple>

int
main(int argc, char **argv)
{
	std::tuple<int, int> t = { 1, 1};
	std::tuple<int, int> u;
	t.swap(u);
	return 0;
}
