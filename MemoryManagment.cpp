#include "Header.h"
void PrintingOutput(vector<process> Output); //to print process in output file
std::ofstream out("log.txt", std::ofstream::out); //out << to out in the log file
int Quantum = 0, Switch = 0 , timestep = 0;

void ArrivingProcess(vector<process> pro, deque <process> &curr, process &check, int time) //check arriced process in a certain time step
{
	for (int i = 0; i < pro.size(); i++)
	{
		if (pro[i].arrival_time == time) //at the timestep push the process in the deque
		{
			curr.push_back(pro[i]);
		}
	}

	if (check.ID != curr.front().ID) //to print the queue of the process in each timestep when front process changes
	{
		out << "Queue: ";
		for (int i = 0; i < curr.size(); i++)
		{
			out << curr[i].ID;
			if (i != curr.size() - 1)
			{
				out << ", ";
			}
		}
		check = curr.front();
		out << endl << "Executing process " << curr.front().ID << "	: started at " << timestep;
	}
}

int main()
{
	vector<process> processes, output; //reciving all processes from input file then put it sorted in outputvector to print it
	deque <process> current; //the deque of RoundRobin => deque to access any needed element
	process check; //to check if it is not equal to current front so print queue list
	ifstream inFile("Processes.txt"); //input file
	if (inFile.is_open())
	{
		processes = read_process(inFile, Quantum, Switch); //return vector of all processes
	}
	else
	{
		cout << "Unable to open file";
	}
	int temp = Quantum; //used if process runtime > quantum
	bool Way = false; //to continue in the path of runtime > quantum
	bool Switched = false; //if there is switch so check arrived process in current time step and during switching time

	while (output.size() < processes.size()) //run until the output vector equals the original processes vector
	{
		if (!Switched)
		{
			ArrivingProcess(processes, current,check, timestep);
		}
		else
		{
			ArrivingProcess(processes, current,check, timestep - 3);
			ArrivingProcess(processes, current,check, timestep - 2);
			ArrivingProcess(processes, current,check, timestep - 1);
			ArrivingProcess(processes, current,check, timestep);
			Switched = false;
		}

		if (current.size() > 0)
		{
			if (current.front().run > Quantum || Way == true)
			{
				Way = true;
				if (temp > 0) 
				{
					//decrement the temp (= Quantum) till it equal zero then subtract process remaining runtime by the original Quantum
					temp--;
				}
				else
				{
					current.front().run -= Quantum;
					out << ",	stopped at " << timestep << ", " << current.front().mem_rem << " remaining, memory starts at " << current.front().mem_start << " and ends at " << current.front().mem_end << endl;
					process traverse = current.front();
					current.pop_front();
					current.push_back(traverse);
					temp = Quantum;
					Way = false;
					timestep++;
					out << "Process switching	: started at " << timestep;
					timestep += 2;
					out << ",	finished at " << timestep << endl;
					Switched = true;
				}
			}
			else //runtime < quantium
			{
				if (current.front().run > 0) 
				{
					current.front().run--;
				}
				else
				{
					current.front().finish_time = timestep;
					out << ",	finished at " << timestep << ", memory starts at " << current.front().mem_start << " and ends at " << current.front().mem_end << endl;
					output.push_back(current.front()); //insert the process in the output vector to print them
					current.pop_front();
					temp = Quantum;
					if (current.size() > 0) 
					{
						//print only if there is reamining process after it
						timestep++;
						out << "Process switching	: started at " << timestep;
						timestep += 2;
						out << ",	finished at " << timestep << endl;
					}
					Switched = true;
				}
			}
		}
		//time step is incremented in every cycle
		timestep++;
	}
	out.close(); //close log file
	sort(output.begin(), output.end(), by_arrival()); //sort vector to output it
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