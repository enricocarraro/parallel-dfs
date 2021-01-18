#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "Graph.hpp"


// READ_TYPE = 1 => C++ ifstream
// READ_TYPE = 0 => C fscanf
#define READ_TYPE 0

using namespace std;

int main(int argc, const char *argv[])
{
	setbuf(stdout, nullptr);
	// 1 parameter of format .gra is required.
	if (argc != 3)
	{
		cout << "Error: Missing parameter" << endl;
		return -1;
	}
	string graname(argv[1]);

	auto itotal = std::chrono::steady_clock::now();

#if READ_TYPE == 1
	std::ifstream ifs(graname, std::ifstream::in);
	if (!ifs.is_open())
	{
		cout << "Error: File doesn't exist." << endl;
		return -1;
	}
#else
	FILE *fp;
	if ((fp = fopen(graname.c_str(), "r")) == NULL)
	{
		cout << "Error: File doesn't exist." << endl;
		return -1;
	}
#endif
	if (strcmp(argv[2], "parallel") == 0)
	{
		auto istart = std::chrono::steady_clock::now();
#if READ_TYPE == 1
		Graph gp(ifs);
		ifs.close();
#else
		Graph gp(fp);
        fclose(fp);
#endif
		gp.sortVectors();
		auto iend = std::chrono::steady_clock::now();
		// gp.printGraph();
		std::chrono::duration<double> ielapsed_seconds = iend - istart;
		std::cout << "Init elapsed time: " << ielapsed_seconds.count() << "s\n";
		auto start = std::chrono::steady_clock::now();
		gp.buildDT();
		auto end = std::chrono::steady_clock::now();
		std::chrono::duration<double> elapsed_seconds = end - start;
		std::cout << "DT elapsed time: " << elapsed_seconds.count() << "s\n";
		auto sstart = std::chrono::steady_clock::now();
		gp.computeSubDTSize();
		auto send = std::chrono::steady_clock::now();
		std::chrono::duration<double> selapsed_seconds = send - sstart;
		std::cout << "subgraph size elapsed time: " << selapsed_seconds.count() << "s\n";
		auto ppstart = std::chrono::steady_clock::now();
		gp.computePrePostOrder();
		gp.computeLabels();
		auto ppend = std::chrono::steady_clock::now();
		std::chrono::duration<double> ppelapsed_seconds = ppend - ppstart;
		std::cout << "pre post elapsed time: " << ppelapsed_seconds.count() << "s\n";
		//gp.printGraph();
		auto itotalend = std::chrono::steady_clock::now();
		std::chrono::duration<double> pptotal = itotalend - itotal;
		cout << "total time taken:" << pptotal.count() << endl;
		//gp.printNodesStatus();
	}
	else if (strcmp(argv[2], "sequential") == 0)
	{
		auto istart = std::chrono::steady_clock::now();
#if READ_TYPE == 1
		Graph gs(ifs);
		ifs.close();
#else
		Graph gs(fp);
        fclose(fp);
#endif
		gs.sortVectors();
		auto iend = std::chrono::steady_clock::now();
		std::chrono::duration<double> ielapsed_seconds = iend - istart;
		std::cout << "Init elapsed time: " << ielapsed_seconds.count() << "s\n";
		auto start = std::chrono::steady_clock::now();
		gs.sequentialDFS();
		auto end = std::chrono::steady_clock::now();
		std::chrono::duration<double> elapsed_seconds = end - start;
		std::cout << "Sequential elapsed time: " << elapsed_seconds.count() << "s\n";
		//gs.printGraph();
		//  gs.printNodesStatus();
	}
	return 0;
}
