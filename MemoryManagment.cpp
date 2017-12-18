#include "Header.h"
int Quantum = 0 , Switch = 0;
int timestep = 0;

void PrintingOutput(vector<process> Output);
void PrintingLog(deque<process> Log);

void ArrivingProcess(vector<process> pro, deque <process> &curr, int time)
{
	for (int i = 0; i < pro.size(); i++)
	{
		if (pro[i].arrival_time == time)
		{
			curr.push_back(pro[i]);
		}
	}
}

int main()
{
	vector<process> processes, output; //reciving all processes from input file
	deque <process> current;
	ifstream inFile("Processes.txt");
	if (inFile.is_open())
	{
		processes = read_process(inFile, Quantum, Switch); //return vector of all processes
	}
	else
	{
		cout << "Unable to open file";
	}

	int temp = Quantum;
	bool Way = false;
	bool Switched = false;

	while (output.size() < processes.size())
	{
		if (!Switched)
		{
			ArrivingProcess(processes, current, timestep);
		}
		else
		{
			ArrivingProcess(processes, current, timestep-1);
			ArrivingProcess(processes, current, timestep);
			Switched = false;
		}

		if (current.size() > 0)
		{
			if (current.front().run > Quantum || Way == true)
			{
				Way = true;
				if (temp > 0)
				{
					temp--;
				}
				else
				{
					current.front().run -= Quantum;
					cout << "At time " << timestep << " process " << current.front().ID << " stopped";
					cout << " arrivial " << current.front().arrival_time;
					cout << " runtime " << current.front().run_time << " run remaining " << current.front().run << endl;
					process traverse = current.front();
					current.pop_front();
					current.push_back(traverse);
					temp = Quantum;
					Way = false;
					timestep ++;
					Switched = true;
				}
			}
			else
			{
				if (current.front().run > 0)
				{
					current.front().run--;
				}
				else
				{
					current.front().finish_time = timestep;
					output.push_back(current.front());
					current.pop_front();
					temp = Quantum;
					timestep ++;
					Switched = true;
				}
			}
		}
		timestep++;
	}
	//PrintingLog(current);
	sort(output.begin(), output.end(), by_arrival());
	PrintingOutput(output);
	system("Pause");
	return 0;
}

void PrintingOutput(vector<process> Output)
{
	FILE *fp = freopen("output.txt", "w", stdout);
	cout << "Quantum " << Quantum << endl;
	cout << "Switch " << Switch << endl;
	cout << "process_id	" << "run_time	" << "arrival_time	" << "finish_time		" << "mem_size	" << "mem_start	" << "mem_end" << endl;
	for (int i = 0; i < Output.size(); i++)
	{
		cout << Output[i].ID << "			" << Output[i].run_time << "			" << Output[i].arrival_time << "				" << Output[i].finish_time << "				";
		cout << Output[i].mem_size << "			" << Output[i].mem_start << "			" << Output[i].mem_end << endl;
	}
	fclose(fp);
}

void PrintingLog(deque<process> Log)
{
	cout << "Queue: ";
	for (int i = 0; i < Log.size(); i++)
	{
		cout << Log[i].ID << " ";
	}
	cout << endl;
}